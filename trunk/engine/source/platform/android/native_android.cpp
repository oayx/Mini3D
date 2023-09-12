#if defined(DC_PLATFORM_ANDROID)
#include <jni.h>
#include <android/native_window_jni.h>
#include <android/asset_manager_jni.h>
#include "platform/common/egl/EGL_Context.h"
#include "platform/WindowManager.h"
#include "runtime/input/Input.h"
#include "runtime/input/KeyboardInput.h"
#include "runtime/input/TouchInput.h"
#include "runtime/resources/Resources.h"
#include "runtime/Application.h"
 
using namespace dc;

struct TouchEvent
{
    int act;
    int index;
    int id;
    int count;
    long long time;
    float xys[20];
};

static JNIEnv* g_env;
static jobject g_jni_obj;

static int call_activity_method_int(const char* name, const char* sig, ...);
static String call_activity_method_string(const char* name, const char* sig, ...);
static void extract_assets_if_needed(jobject asset_manager, const String& package_path, const String& data_path, bool always_extract);
static void on_key_down(int key_code, int unicode_char);
static void on_key_up(int key_code, int unicode_char);
static void on_touch(const TouchEvent& touch);
static void swap_bytes(void* bytes, int size);

extern "C"
{
    void Java_cn_oayx_engine_JNI_engineCreate(JNIEnv* env, jobject obj, jobject surface, jint width, jint height, jobject asset_manager)
    {
        g_env = env;
		g_jni_obj = obj;

        Debuger::Log("engineCreate begin");
        Debuger::Log("surface w: %d h: %d", width, height);

        auto package_path = call_activity_method_string("getPackagePath", "()Ljava/lang/String;");
        auto data_files_path = call_activity_method_string("getFilesDirPath", "()Ljava/lang/String;");
        Debuger::Log("package_path: %s", package_path.c_str());
        Debuger::Log("data_files_path: %s", data_files_path.c_str());

        extract_assets_if_needed(asset_manager, package_path, data_files_path, true);
		Application::SetAssetsPath(data_files_path + "/assets");
		Application::SetPersistentDataPath("sdcard/backup");

		ANativeWindow* window = ANativeWindow_fromSurface(env, surface);
		Application::Initialize();
		WindowDesc window_desc;
		window_desc.name = "android";
		window_desc.hWnd = window;
		window_desc.width = width;
		window_desc.height = height;
		window_desc.fps = 60;
		window_desc.antiAlias = 4;
		window_desc.sync = 0;
		window_desc.flags = WindowFlag::FullScreen;
		Application::CreateRenderWindow(window_desc);
    }

    void Java_cn_oayx_engine_JNI_engineDestroy(JNIEnv* env, jobject obj)
    {
        g_env = env;
        g_jni_obj = obj;

		Debuger::Log("engineDestroy");
		Application::Shutdown();
    }

    void Java_cn_oayx_engine_JNI_engineSurfaceResize(JNIEnv* env, jobject obj, jobject surface, jint width, jint height)
    {
        g_env = env;
        g_jni_obj = obj;

        Debuger::Log("engineSurfaceResize begin");
        Debuger::Log("surface w: %d h: %d", width, height);

        ANativeWindow* window = ANativeWindow_fromSurface(env, surface);
		WindowResizeDesc desc(window, width, height, true, true);
		Application::Resize(desc);
    }

    void Java_cn_oayx_engine_JNI_engineSurfaceDestroy(JNIEnv* env, jobject obj)
    {
        g_env = env;
        g_jni_obj = obj;

		EGL_Context::Destroy();
        Debuger::Log("engineSurfaceDestroy");
    }

    void Java_cn_oayx_engine_JNI_enginePause(JNIEnv* env, jobject obj)
    {
        g_env = env;
        g_jni_obj = obj;
		Application::Pause(true);
    }

    void Java_cn_oayx_engine_JNI_engineResume(JNIEnv* env, jobject obj)
    {
        g_env = env;
		g_jni_obj = obj;
		Application::Pause(false);
    }

    void Java_cn_oayx_engine_JNI_engineDraw(JNIEnv* env, jobject obj)
	{
        g_env = env;
        g_jni_obj = obj;
        Application::MainLoop(60);
    }

    void Java_cn_oayx_engine_JNI_engineKeyDown(JNIEnv* env, jobject obj, jint key_code, jint unicode_char)
    {
        g_env = env;
        g_jni_obj = obj;

        Debuger::Log("engineKeyDown: %d, %d", key_code, unicode_char);
        on_key_down(key_code, unicode_char);
    }

    void Java_cn_oayx_engine_JNI_engineKeyUp(JNIEnv* env, jobject obj, jint key_code, jint unicode_char)
    {
        g_env = env;
        g_jni_obj = obj;

        Debuger::Log("engineKeyUp: %d, %d", key_code, unicode_char);
        on_key_up(key_code, unicode_char);
    }

    void Java_cn_oayx_engine_JNI_engineTouch(JNIEnv* env, jobject obj, jbyteArray touch_data)
    {
        g_env = env;
        g_jni_obj = obj;

        jsize data_size = env->GetArrayLength(touch_data);
        jbyte* data = env->GetByteArrayElements(touch_data, nullptr);
        if (data != nullptr)
        {
            TouchEvent e;
			::memcpy(&e, data, sizeof(e) - sizeof(e.xys));
            swap_bytes(&e.act, sizeof(e.act));
            swap_bytes(&e.index, sizeof(e.index));
            swap_bytes(&e.id, sizeof(e.id));
            swap_bytes(&e.count, sizeof(e.count));
            swap_bytes(&e.time, sizeof(e.time));

            if (e.count <= 10)
            {
                int offset = sizeof(e) - sizeof(e.xys);
				::memcpy(e.xys, &data[offset], sizeof(float) * 2 * e.count);

                for (int i = 0; i < e.count; ++i)
                {
                    float x = e.xys[i * 2];
                    float y = e.xys[i * 2 + 1];
                    swap_bytes(&x, sizeof(x));
                    swap_bytes(&y, sizeof(y));
                    y = (float) WindowManager::GetWindow(TargetDisplay::Display1)->GetHeight() - y - 1;
                    e.xys[i * 2] = x;
                    e.xys[i * 2 + 1] = y;
                }

                on_touch(e);
            }

            env->ReleaseByteArrayElements(touch_data, data, JNI_ABORT);
        }
    }
}
#pragma region 键盘输入+屏幕触摸
#define AMOTION_EVENT_ACTION_DOWN 0				//手指按下
#define AMOTION_EVENT_ACTION_UP 1				//手指抬起
#define AMOTION_EVENT_ACTION_MOVE 2				//手势移动
#define AMOTION_EVENT_ACTION_CANCEL 3			//取消事件
#define AMOTION_EVENT_ACTION_POINTER_DOWN 5		//多个手指按下
#define AMOTION_EVENT_ACTION_POINTER_UP 6		//多个手指抬起

static int get_key(int key_code)
{
    int key = -1;

	if (key_code >= 7 && key_code < 7 + 10)
	{//数字键
		key = (int)KeyCode::Alpha0 + key_code - 7;
	}
	else if (key_code >= 144 && key_code < 144 + 10)
	{//小键盘数字键
		key = (int)KeyCode::Keypad0 + key_code - 144;
	}
	else if (key_code >= 29 && key_code <= 29 + 'z' - 'a')
	{//小写
		key = (int)KeyCode::A + key_code - 29;
	}
	//else if (key_code >= 59 && key_code <= 59 + 'z' - 'a')
	//{//大写
	//	key = (int)KeyCode::A + key_code - 59;
	//}
	else if (key_code >= 131 && key_code < 131 + 12)
	{//F1-F12
		key = (int)KeyCode::F1 + key_code - 131;
	}

	if (key == -1)
	{
		switch (key_code)
		{
		case 3: break;//KEYCODE_HOME
		case 4: break;//返回键KEYCODE_BACK
		case 5: break;//拨号键KEYCODE_CALL
		case 6: break;//挂机键KEYCODE_ENDCALL
		case 24: break;//音量增加键KEYCODE_VOLUME_UP
		case 25: break;//音量减小键KEYCODE_VOLUME_DOWN
		case 82:key = (int)KeyCode::Menu; break;//菜单键KEYCODE_MENU

		case 66: key = (int)KeyCode::Return; break;
		case 111: key = (int)KeyCode::Escape; break;
		case 19: key = (int)KeyCode::UpArrow; break;
		case 20: key = (int)KeyCode::DownArrow; break;
		case 21: key = (int)KeyCode::LeftArrow; break;
		case 22: key = (int)KeyCode::RightArrow; break;
		case 61: key = (int)KeyCode::Tab; break;
		case 92: key = (int)KeyCode::PageUp; break;
		case 93: key = (int)KeyCode::PageDown; break;
		case 67: key = (int)KeyCode::Backspace; break;
		case 112: key = (int)KeyCode::Delete; break;
		case 124: key = (int)KeyCode::Insert; break;
		case 143: key = (int)KeyCode::Numlock; break;
		case 115: key = (int)KeyCode::CapsLock; break;
		case 121: key = (int)KeyCode::Break; break;
		case 116: key = (int)KeyCode::ScrollLock; break;
			
		case 57: key = (int)KeyCode::LeftAlt; break;
		case 58: key = (int)KeyCode::RightAlt; break;
		case 113: key = (int)KeyCode::LeftControl; break;
		case 114: key = (int)KeyCode::RightControl; break;
		case 59: key = (int)KeyCode::LeftShift; break;
		case 60: key = (int)KeyCode::RightShift; break;

		case 81: key = (int)KeyCode::Plus; break;
		case 69: key = (int)KeyCode::Minus; break;
		case 76: key = (int)KeyCode::Slash; break;
		case 70: key = (int)KeyCode::Equals; break;
		case 75: key = (int)KeyCode::Quote; break;
		case 73: key = (int)KeyCode::Backslash; break;
		case 55: key = (int)KeyCode::Comma; break;
		case 56: key = (int)KeyCode::Period; break;
		case 71: key = (int)KeyCode::LeftBracket; break;
		case 72: key = (int)KeyCode::RightBracket; break;
		case 74: key = (int)KeyCode::Semicolon; break;
		case 62: key = (int)KeyCode::Space; break;

		case 154: key = (int)KeyCode::KeypadDivide; break;
		case 155: key = (int)KeyCode::KeypadMultiply; break;
		case 156: key = (int)KeyCode::KeypadMinus; break;
		case 157: key = (int)KeyCode::KeypadPlus; break;
		case 158: key = (int)KeyCode::KeypadPeriod; break;
		case 160: key = (int)KeyCode::KeypadEnter; break;
		}
	}

    return key;
}

static void on_key_down(int key_code, int unicode_char)
{
    int key = get_key(key_code);
    if (key >= 0)
    {
		KeyboardInput::HandleKeyDown(key);
    }
}

static void on_key_up(int key_code, int unicode_char)
{
    int key = get_key(key_code);
    if (key >= 0)
    {
		KeyboardInput::HandleKeyUp(key);
	}

	switch ((KeyCode)key)
	{
	case KeyCode::Backspace:unicode_char = key;
	case KeyCode::Return:unicode_char = key;
	case KeyCode::Tab:unicode_char = key;
	}
	if (unicode_char > 0)
	{
		if (unicode_char == 10)unicode_char = (int)KeyCode::Return;//转换成windows的回车键
		KeyboardInput::AddInputChar((ushort)unicode_char);
	}
}

static void touch_begin(const void* event)
{
    const TouchEvent* t = (const TouchEvent*) event;

    float x = t->xys[t->index * 2];
    float y = t->xys[t->index * 2 + 1];

    Touch touch;
    touch.deltaPosition = Vector2(0, 0);
    touch.deltaTime = 0;
    touch.time = t->time / 1000.0f;
    touch.fingerId = t->id;
    touch.phase = TouchPhase::Began;
    touch.tapCount = t->count;
    touch.position = Vector2(x, y);
	TouchInput::AddTouch(touch);
}

static void touch_update(const void* event)
{
    const TouchEvent* t = (const TouchEvent*) event;

    float x = t->xys[t->index * 2];
    float y = t->xys[t->index * 2 + 1];

    Touch touch;
    touch.deltaPosition = Vector2(0, 0);
    touch.deltaTime = 0;
    touch.time = t->time / 1000.0f;
    touch.fingerId = t->id;
    touch.tapCount = t->count;
    touch.position = Vector2(x, y);
    if (t->act == AMOTION_EVENT_ACTION_MOVE)
    {
        touch.phase = TouchPhase::Stationary;
    }
    else
    {
		touch.phase = TouchPhase::Ended;
    }
	TouchInput::UpdateTouch(touch);
}

static void on_touch(const TouchEvent& touch)
{
    switch (touch.act)
    {
        case AMOTION_EVENT_ACTION_DOWN:
        case AMOTION_EVENT_ACTION_POINTER_DOWN:
            touch_begin(&touch);
            break;

        case AMOTION_EVENT_ACTION_MOVE:
        case AMOTION_EVENT_ACTION_UP:
        case AMOTION_EVENT_ACTION_POINTER_UP:
        case AMOTION_EVENT_ACTION_CANCEL:
            touch_update(&touch);
            break;

        default:
            break;
    }
}

static void swap_bytes(void* bytes, int size)
{
    char* p = (char*) bytes;
    int count = size / 2;
    for (int i = 0; i < count; ++i)
    {
        char t = p[i];
        p[i] = p[size - 1 - i];
        p[size - 1 - i] = t;
    }
}
#pragma endregion

#pragma region 解压资源
static void extract_assets(jobject asset_manager, const String& source, const String& dest)
{
    auto mgr = AAssetManager_fromJava(g_env, asset_manager);
    Vector<String> files;

    // file_list.txt is generated by copy_assets.py script.
    auto asset = AAssetManager_open(mgr, "file_list.txt", AASSET_MODE_BUFFER);
    if (asset)
    {
        auto size = AAsset_getLength(asset);
        auto asset_buffer = AAsset_getBuffer(asset);
        auto file_list = String((const char*)asset_buffer, size).Replace("\r\n", "\n");
        files = file_list.Split("\n");

        AAsset_close(asset);
    }
    else
    {
        Debuger::Log("open file_list.txt in assets failed");
    }

    for (int i = 0; i < files.Size(); i++)
    {
        const auto& file = files[i];
		//Debuger::Log("open file_list:%s", file.c_str());
        asset = AAssetManager_open(mgr, file.c_str(), AASSET_MODE_BUFFER);
        if (asset)
        {
            auto size = AAsset_getLength(asset);
            auto asset_buffer = AAsset_getBuffer(asset);
            String dest_filename = dest + "/" + file;
            auto dir = dest_filename.Substring(0, dest_filename.LastIndexOf("/"));
            Directory::Create(dir);

			Debuger::Log("WriteAllBytes:%s", dest_filename.c_str());
            File::WriteAllBytes(dest_filename, (byte*)asset_buffer, size);

            AAsset_close(asset);
        }
        else
        {
            Debuger::Error("open asset file:%s failed", file.c_str());
        }
    }
}
static void extract_assets_if_needed(jobject asset_manager, const String& package_path, const String& data_path, bool always_extract)
{
	bool extract = false;

	auto version_file = data_path + "/version.txt";
	if (!File::Exist(version_file))
	{
		extract = true;
	}
	else
	{
		auto version = File::ReadAllText(version_file);
		if (version != "1.0.0")
		{
			extract = true;
		}
	}

	if (extract || always_extract)
	{
		Debuger::Log("extract Assets");

        // unzip open apk file failed now,
        // so use asset manager instead.
        //File::Unzip(package_path, "assets/assets", data_path, true);
        extract_assets(asset_manager, "assets", data_path);

        File::WriteAllText(version_file, "1.0.0");
	}
	else
	{
		Debuger::Log("skip extract Assets");
	}
}
#pragma endregion

#pragma region 调用android方法
static int call_activity_method_int(const char* name, const char* sig, ...)
{
	va_list args;
	va_start(args, sig);

	jclass clazz = g_env->GetObjectClass(g_jni_obj);
	jmethodID methodID = g_env->GetMethodID(clazz, name, sig);
	int result = g_env->CallIntMethodV(g_jni_obj, methodID, args);
	g_env->DeleteLocalRef(clazz);

	va_end(args);

	return result;
}

static String call_activity_method_string(const char* name, const char* sig, ...)
{
	va_list args;
	va_start(args, sig);

	jclass clazz = g_env->GetObjectClass(g_jni_obj);
	jmethodID methodID = g_env->GetMethodID(clazz, name, sig);
	jstring str = (jstring)g_env->CallObjectMethodV(g_jni_obj, methodID, args);
	String result = g_env->GetStringUTFChars(str, nullptr);
	g_env->DeleteLocalRef(str);
	g_env->DeleteLocalRef(clazz);

	va_end(args);

	return result;
}
//切换键盘
void java_show_keyboard()
{
	//int state = call_activity_method_int("isKeyboardOpen", "()I");
	//Debuger::Log("---state:%d", state);
	//if (state == 0)
	{
		call_activity_method_int("switchKeyboard", "()I");
	}
	//int result = call_activity_method_int("showKeyboard", "()I");
	//Debuger::Log("---result:%d", result);
}
void java_hide_keyboard()
{
	//int state = call_activity_method_int("isKeyboardOpen", "()I");
	//Debuger::Log("---state:%d", state);
	//if (state != 0)
	{
		call_activity_method_int("switchKeyboard", "()I");
	}
	//int result = call_activity_method_int("hideKeyboard", "()I");
	//Debuger::Log("---result:%d", result);
}
//请求退出应用
void java_quit_application()
{
    call_activity_method_int("quitApplication", "()I");
}
#pragma endregion
#endif