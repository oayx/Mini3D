#pragma once


//平台依赖
#if defined(DC_PLATFORM_WIN32)
#	include "platform/windows/Win32PlatformDefine.h"
#elif defined(DC_PLATFORM_LINUX)
#	include "platform/linux/LinuxPlatformDefine.h"
#elif defined(DC_PLATFORM_ANDROID)
#	include "platform/android/AndroidPlatformDefine.h"
#elif defined(DC_PLATFORM_WASM)
#	include "platform/wasm/WASMPlatformDefine.h"
#elif defined(DC_PLATFORM_MAC)
#	include "platform/mac/MacPlatformDefine.h"
#elif defined(DC_PLATFORM_IOS)
#	include "platform/ios/iOSPlatformDefine.h"
#endif


DC_BEGIN_NAMESPACE

DC_END_NAMESPACE