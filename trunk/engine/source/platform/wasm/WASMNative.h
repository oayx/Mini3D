
/*****************************************************************************
* Author： hannibal
* Description：
*****************************************************************************/
#pragma once
#if defined(DC_PLATFORM_WASM)

#include "platform/PlatformBase.h"

DC_BEGIN_NAMESPACE

//设置主循环
extern void wasm_set_main_loop(void (*callback_func)(void), int fps);
//退出应用
extern void wasm_quit_application();	    

DC_END_NAMESPACE
#endif /*DC_PLATFORM_WASM*/
