
/*****************************************************************************************************/
// @author hannibal
// @date   2020/05/28
// @brief  dll处理
/*****************************************************************************************************/
#pragma once
 
#include "Debuger.h"

DC_BEGIN_NAMESPACE
#if DC_PLATFORM_WIN32
	typedef HMODULE LIB_HANDLE;
	#define LOAD_DY_LIB(handle, name) \
					{ \
						handle = ::LoadLibraryA(name); \
						if (handle == nullptr) \
						{ \
							Debuger::Error("open dynamic lib failed: %s", name); \
							return; \
						} \
						Debuger::Log("load dll:%s", name);\
					}
	#define GET_DY_FUNC(handle, func) ::GetProcAddress(handle, func)
	#define FREE_DY_LIB(handle) { if (handle) ::FreeLibrary(handle); handle = nullptr; }
#elif defined(DC_PLATFORM_LINUX) || defined(DC_PLATFORM_ANDROID) || defined(DC_PLATFORM_MAC) || defined(DC_PLATFORM_IOS)
	typedef void* LIB_HANDLE;
	#define LOAD_DY_LIB(handle, name) \
				{ \
					handle = ::dlopen(name, RTLD_LAZY); \
					if (handle == nullptr) \
					{ \
						Debuger::Error("open dynamic lib failed: %s %s", name, ::dlerror()); \
						return; \
					} \
					Debuger::Log("load dll:%s", name);\
				}
	#define GET_DY_FUNC(handle, func) ::dlsym(handle, func)
	#define FREE_DY_LIB(handle) { if (handle) ::dlclose(handle); handle = nullptr; }
#endif
DC_END_NAMESPACE