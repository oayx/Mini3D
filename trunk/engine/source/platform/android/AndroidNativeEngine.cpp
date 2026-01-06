#if defined(DC_PLATFORM_ANDROID)
#include <jni.h>
#include <android/native_window_jni.h>
#include <android/asset_manager_jni.h>
#include "platform/common/egl/EGL_Context.h"
#include "platform/WindowManager.h"
#include "runtime/Application.h"
#include "AndroidNative.h"

JNIEnv* g_env;
jobject g_jni_obj;

DC_BEGIN_NAMESPACE

int android_call_activity_method_int(const char* name, const char* sig, ...);
String android_call_activity_method_string(const char* name, const char* sig, ...);
void android_extract_assets_if_needed(jobject asset_manager, const String& package_path, const String& data_path, bool always_extract);
void android_on_key_down(int key_code, int unicode_char);
void android_on_key_up(int key_code, int unicode_char);
void android_on_touch(const AndroidTouchEvent& touch);
void android_swap_bytes(void* bytes, int size);

extern "C"
{
    void Java_cn_oayx_engine_JNI_engineCreate(JNIEnv* env, jobject obj, jobject surface, jint width, jint height, jobject asset_manager)
    {
        g_env = env;
		g_jni_obj = obj;

        Debuger::Log("engineCreate begin");
        Debuger::Log("surface w: %d h: %d", width, height);

        auto package_path = android_call_activity_method_string("getPackagePath", "()Ljava/lang/String;");
        auto data_files_path = android_call_activity_method_string("getFilesDirPath", "()Ljava/lang/String;");
        Debuger::Log("package_path: %s", package_path.c_str());
        Debuger::Log("data_files_path: %s", data_files_path.c_str());

        android_extract_assets_if_needed(asset_manager, package_path, data_files_path, true);
		Application::SetAssetsPath(data_files_path + "/assets");
		Application::SetPersistentDataPath("sdcard/backup");

		Application::Initialize();

		WindowDesc window_desc;
		window_desc.name = "android";
		window_desc.hWnd = ANativeWindow_fromSurface(env, surface);
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

	void Java_cn_oayx_engine_JNI_engineSurfaceCreated(JNIEnv* env, jobject obj, jobject surface, jint width, jint height)
	{
		g_env = env;
		g_jni_obj = obj;

		Debuger::Log("engineSurfaceCreated surface w: %d h: %d", width, height);
	}

    void Java_cn_oayx_engine_JNI_engineSurfaceResize(JNIEnv* env, jobject obj, jobject surface, jint width, jint height)
    {
        g_env = env;
        g_jni_obj = obj;

        Debuger::Log("engineSurfaceResize surface w: %d h: %d", width, height);

        ANativeWindow* window = ANativeWindow_fromSurface(env, surface);
		WindowResizeDesc desc(window, width, height, true, true);
		Application::Resize(desc);
    }

    void Java_cn_oayx_engine_JNI_engineSurfaceDestroy(JNIEnv* env, jobject obj, jobject surface)
    {
        g_env = env;
		g_jni_obj = obj;

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

        //Debuger::Log("engineKeyDown: %d, %d", key_code, unicode_char);
        android_on_key_down(key_code, unicode_char);
    }

    void Java_cn_oayx_engine_JNI_engineKeyUp(JNIEnv* env, jobject obj, jint key_code, jint unicode_char)
    {
        g_env = env;
        g_jni_obj = obj;

        //Debuger::Log("engineKeyUp: %d, %d", key_code, unicode_char);
        android_on_key_up(key_code, unicode_char);
    }

    void Java_cn_oayx_engine_JNI_engineTouch(JNIEnv* env, jobject obj, jbyteArray touch_data)
    {
        g_env = env;
        g_jni_obj = obj;

        jsize data_size = env->GetArrayLength(touch_data);
        jbyte* data = env->GetByteArrayElements(touch_data, nullptr);
        if (data != nullptr)
        {
            AndroidTouchEvent e;
			::memcpy(&e, data, sizeof(e) - sizeof(e.xys));
            android_swap_bytes(&e.act, sizeof(e.act));
            android_swap_bytes(&e.index, sizeof(e.index));
            android_swap_bytes(&e.id, sizeof(e.id));
            android_swap_bytes(&e.count, sizeof(e.count));
            android_swap_bytes(&e.time, sizeof(e.time));

            if (e.count <= 10)
            {
                int offset = sizeof(e) - sizeof(e.xys);
				::memcpy(e.xys, &data[offset], sizeof(float) * 2 * e.count);

                for (int i = 0; i < e.count; ++i)
                {
                    float x = e.xys[i * 2];
                    float y = e.xys[i * 2 + 1];
                    android_swap_bytes(&x, sizeof(x));
                    android_swap_bytes(&y, sizeof(y));
                    y = (float) WindowManager::GetWindow(TargetDisplay::Display1)->GetHeight() - y - 1;
                    e.xys[i * 2] = x;
                    e.xys[i * 2 + 1] = y;
                }

                android_on_touch(e);
            }

            env->ReleaseByteArrayElements(touch_data, data, JNI_ABORT);
        }
    }
}
DC_END_NAMESPACE
#endif