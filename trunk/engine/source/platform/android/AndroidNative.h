
/*****************************************************************************
* Author： hannibal
* Description：
*****************************************************************************/
#pragma once
#if defined(DC_PLATFORM_ANDROID)

#include "platform/PlatformBase.h"

DC_BEGIN_NAMESPACE

struct AndroidTouchEvent
{
	int act;
	int index;
	int id;
	int count;
	long long time;
	float xys[20];
};

extern void android_show_keyboard();		//切换键盘
extern void android_hide_keyboard();		//切换键盘
extern void android_quit_application();	    //退出应用

DC_END_NAMESPACE
#endif /*DC_PLATFORM_ANDROID*/
