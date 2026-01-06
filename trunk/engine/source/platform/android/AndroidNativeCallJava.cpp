#if defined(DC_PLATFORM_ANDROID)
#include <jni.h>
#include "runtime/Application.h"

extern JNIEnv* g_env;
extern jobject g_jni_obj;

//调用android方法
DC_BEGIN_NAMESPACE
int android_call_activity_method_int(const char* name, const char* sig, ...)
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

String android_call_activity_method_string(const char* name, const char* sig, ...)
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
void android_show_keyboard()
{
	//int state = android_call_activity_method_int("isKeyboardOpen", "()I");
	//Debuger::Log("---state:%d", state);
	//if (state == 0)
	{
		android_call_activity_method_int("switchKeyboard", "()I");
	}
	//int result = android_call_activity_method_int("showKeyboard", "()I");
	//Debuger::Log("---result:%d", result);
}
void android_hide_keyboard()
{
	//int state = android_call_activity_method_int("isKeyboardOpen", "()I");
	//Debuger::Log("---state:%d", state);
	//if (state != 0)
	{
		android_call_activity_method_int("switchKeyboard", "()I");
	}
	//int result = android_call_activity_method_int("hideKeyboard", "()I");
	//Debuger::Log("---result:%d", result);
}
//请求退出应用
void android_quit_application()
{
    android_call_activity_method_int("quitApplication", "()I");
}
DC_END_NAMESPACE
#endif