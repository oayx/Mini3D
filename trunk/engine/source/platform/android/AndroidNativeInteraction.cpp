#if defined(DC_PLATFORM_ANDROID)
#include <jni.h>
#include "runtime/input/Input.h"
#include "runtime/input/KeyboardInput.h"
#include "runtime/input/TouchInput.h"
#include "runtime/Application.h"
#include "AndroidNative.h"

extern JNIEnv* g_env;
extern jobject g_jni_obj;

//键盘输入+屏幕触摸
DC_BEGIN_NAMESPACE
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

void android_on_key_down(int key_code, int unicode_char)
{
    int key = get_key(key_code);
    if (key >= 0)
    {
		KeyboardInput::HandleKeyDown(key);
    }
}

void android_on_key_up(int key_code, int unicode_char)
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

void android_touch_begin(const void* event)
{
    const AndroidTouchEvent* t = (const AndroidTouchEvent*) event;

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

void android_touch_update(const void* event)
{
    const AndroidTouchEvent* t = (const AndroidTouchEvent*) event;

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

void android_on_touch(const AndroidTouchEvent& touch)
{
    switch (touch.act)
    {
        case AMOTION_EVENT_ACTION_DOWN:
        case AMOTION_EVENT_ACTION_POINTER_DOWN:
            android_touch_begin(&touch);
            break;

        case AMOTION_EVENT_ACTION_MOVE:
        case AMOTION_EVENT_ACTION_UP:
        case AMOTION_EVENT_ACTION_POINTER_UP:
        case AMOTION_EVENT_ACTION_CANCEL:
            android_touch_update(&touch);
            break;

        default:
            break;
    }
}

void android_swap_bytes(void* bytes, int size)
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
DC_END_NAMESPACE
#endif