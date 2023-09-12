#pragma once
#if defined(DC_PLATFORM_ANDROID)
 
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <sys/time.h>
#include <sys/stat.h>

#include "core/BaseType.h"
#include "platform/android/AndroidPlatform.h"

extern void java_show_keyboard();		//切换键盘
extern void java_hide_keyboard();		//切换键盘
extern void java_quit_application();	//退出应用

DC_BEGIN_NAMESPACE

typedef AndroidPlatform Platform;

DC_END_NAMESPACE

#endif /*DC_PLATFORM_ANDROID*/
