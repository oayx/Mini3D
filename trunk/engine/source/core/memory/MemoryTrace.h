#pragma once
#if defined(DC_PLATFORM_WIN32)

#include "platform/windows/Win32PlatformDefine.h"

DC_BEGIN_NAMESPACE

class MemoryTrace
{
public:
	MemoryTrace()
	{
#if defined(DC_DEBUG) && !defined(DC_MALLOC) && !defined(DC_DEBUG_MALLOC)
		_CrtDumpMemoryLeaks();//内存泄露检查
#endif
	}
	~MemoryTrace()
	{
	}
};
inline static MemoryTrace g_MemoryTrace;

DC_END_NAMESPACE
#endif