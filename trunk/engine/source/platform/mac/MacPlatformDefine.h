#pragma once
#if defined(DC_PLATFORM_MAC)

#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <sys/time.h>
#include <sys/stat.h>

#include "core/BaseType.h"
#include "platform/mac/MacPlatform.h"

DC_BEGIN_NAMESPACE

typedef MacPlatform Platform;

DC_END_NAMESPACE

#endif /*DC_PLATFORM_MAC*/
