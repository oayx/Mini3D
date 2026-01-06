#pragma once
#if defined(DC_PLATFORM_WASM)

#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <sys/time.h>
#include <sys/stat.h>

#include <emscripten.h>

#include "core/BaseType.h"
#include "platform/wasm/WASMNative.h"
#include "platform/wasm/WASMPlatform.h"

DC_BEGIN_NAMESPACE

typedef WASMPlatform Platform;

DC_END_NAMESPACE

#endif /*DC_PLATFORM_WASM*/
