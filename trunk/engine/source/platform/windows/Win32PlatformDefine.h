#pragma once
#if defined(DC_PLATFORM_WIN32)

#include <winsock2.h>
#include <windows.h>
#include <mmsystem.h>
#include <time.h>
#include <io.h>

#include "core/BaseType.h"
#include "platform/windows/Win32Platform.h"

#pragma comment(lib, "winmm")
#pragma comment(lib, "ws2_32")

#undef min
#undef max

DC_BEGIN_NAMESPACE

typedef Win32Platform Platform;

#if DC_DEBUG
	#define HR(hr) do { AssertEx(SUCCEEDED(hr), "GetLastError:%u", ::GetLastError()); } while(0)
#else
	#define HR(hr) UNUSED(hr)
#endif


DC_END_NAMESPACE

#endif /*DC_PLATFORM_WIN32*/

/*
编译器版本的_MSC_VER值：
Visual Studio 6.0				1200
Visual Studio .NET 2002 (7.0)	1300
Visual Studio .NET 2003 (7.1)	1310
Visual Studio 2005 (8.0)		1400
Visual Studio 2008 (9.0)		1500
Visual Studio 2010 (10.0)		1600
Visual Studio 2012 (11.0)		1700
Visual Studio 2013 (12.0)		1800
Visual Studio 2015 (14.0)		1900
Visual Studio 2017 RTW (15.0)	1910
Visual Studio 2017 version 15.3	1911
Visual Studio 2017 version 15.5	1912
Visual Studio 2017 version 15.6	1913
Visual Studio 2017 version 15.7	1914
Visual Studio 2017 version 15.8	1915
Visual Studio 2017 version 15.9	1916
Visual Studio 2019 RTW (16.0)	1920
Visual Studio 2019 version 16.1	1921
Visual Studio 2019 version 16.2	1922
Visual Studio 2019 version 16.3	1923
其中MS VC++ 14.0表示Visual C++的版本为14.0，后面括号中的Visual Studio 2015，表明该VC++包含在微软开发工具Visual Studio 2015中
*/

/*判断win版本(_WIN32_WINNT >= _WIN32_WINNT_WIN10)
#define _WIN32_WINNT_NT4                    0x0400 // Windows NT 4.0
#define _WIN32_WINNT_WIN2K                  0x0500 // Windows 2000
#define _WIN32_WINNT_WINXP                  0x0501 // Windows XP
#define _WIN32_WINNT_WS03                   0x0502 // Windows Server 2003
#define _WIN32_WINNT_WIN6                   0x0600 // Windows Vista
#define _WIN32_WINNT_VISTA                  0x0600 // Windows Vista
#define _WIN32_WINNT_WS08                   0x0600 // Windows Server 2008
#define _WIN32_WINNT_LONGHORN               0x0600 // Windows Vista
#define _WIN32_WINNT_WIN7                   0x0601 // Windows 7
#define _WIN32_WINNT_WIN8                   0x0602 // Windows 8
#define _WIN32_WINNT_WINBLUE                0x0603 // Windows 8.1
#define _WIN32_WINNT_WINTHRESHOLD           0x0A00 // Windows 10
#define _WIN32_WINNT_WIN10                  0x0A00 // Windows 10
*/