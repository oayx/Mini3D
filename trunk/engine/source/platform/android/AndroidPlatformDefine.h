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
#include "platform/android/AndroidNative.h"
#include "platform/android/AndroidPlatform.h"

DC_BEGIN_NAMESPACE

typedef AndroidPlatform Platform;

DC_END_NAMESPACE

#endif /*DC_PLATFORM_ANDROID*/
