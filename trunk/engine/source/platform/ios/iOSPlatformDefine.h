#pragma once
#if defined(DC_PLATFORM_IOS)

#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <sys/time.h>
#include <sys/stat.h>

#include "core/BaseType.h"
#include "platform/ios/iOSPlatform.h"

DC_BEGIN_NAMESPACE

typedef iOSPlatform Platform;

DC_END_NAMESPACE

#endif /*DC_PLATFORM_IOS*/
