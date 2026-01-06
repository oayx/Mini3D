 
/*****************************************************************************************************/
// @author hannibal
// @date   2015/04/21
// @brief  基本宏定义
/*****************************************************************************************************/
#pragma once

#include <limits.h>
#include "Compiler.h"

#pragma warning( disable: 4996 )
#pragma warning( disable: 4018 )
#pragma warning( disable: 4267 )
#pragma warning( disable: 4244 )
#pragma warning( disable: 4251 )
///////////////////////////////////////////////////////////////////////////////////////
//基本数据宏定义

// Limits
#define MIN_float 			-(3.402823466e+38f)
#define MIN_double 			-(1.7976931348623158e+308)
#define MAX_float 			(3.402823466e+38f)
#define MAX_double 			(1.7976931348623158e+308)

//浮点数转换
#define FToDW(f)			(*(DWORD*)(&(f)))          //float类型转换DWORD为型
#define DWToF(f)			(*(float*)(&(f)))          //DWORD类型转换为float型

#undef min
#undef max

//格式化
#if defined(DC_PLATFORM_WIN32)
	#undef  FD_SETSIZE
	#define FD_SETSIZE		1024
#endif
	//windows下，新的long long int，已经支持%lld
	//#define I64FMT		"%016I64X"					//十六进制输出: 0表示空余的位用零填充，16表示显示16位数字(格式化的字符数宽度)，I64表示64位int值（I32表示32位int值，i必须大写）, x表示十六进制形式显示
	//#define I64FMTD		"%I64u"						//uint64
	//#define SI64FMTD		"%I64d"						//int64
	#define I64FMT			"%016llX"
	#define I64FMTD			"%llu"
	#define SI64FMTD		"%lld"

//枚举操作
#define DECLARE_ENUM_OPERATORS(T) \
    inline T operator~ (T a) { return (T)~(int)a; } \
    inline T operator| (T a, T b) { return (T)((int)a | (int)b); } \
    inline bool operator! (T a) { return (int)a == 0; } \
    inline int operator& (T a, T b) { return ((int)a & (int)b); } \
    inline T operator^ (T a, T b) { return (T)((int)a ^ (int)b); } \
    inline T& operator|= (T& a, T b) { return (T&)((int&)a |= (int)b); } \
    inline T& operator&= (T& a, T b) { return (T&)((int&)a &= (int)b); } \
    inline T& operator^= (T& a, T b) { return (T&)((int&)a ^= (int)b); } \
	inline bool operator== (int a, T b) { return (a == (int)b); } \
	inline bool operator== (T a, int b) { return ((int)a == b); } \
	inline bool operator!= (int a, T b) { return (a != (int)b); } \
	inline bool operator!= (T a, int b) { return ((int)a != b); }

//无效索引
#define INVALID_INDEX		(-1)
///////////////////////////////////////////////////////////////////////////////////////
//架构
enum class ArchitectureType : unsigned char
{
	/// <summary>
	/// Anything or not important.
	/// </summary>
	AnyCPU = 0,

	/// <summary>
	/// The x86 32-bit.
	/// </summary>
	x86 = 1,

	/// <summary>
	/// The x86 64-bit.
	/// </summary>
	x64 = 2,

	/// <summary>
	/// The ARM 32-bit.
	/// </summary>
	ARM = 3,

	/// <summary>
	/// The ARM 64-bit.
	/// </summary>
	ARM64 = 4,
};
DECLARE_ENUM_OPERATORS(ArchitectureType);
//平台
enum class PlatformType : unsigned char
{
	/// <summary>
	/// Running on Windows.
	/// </summary>
	Windows = 1,

	/// <summary>
	/// Running on Linux system.
	/// </summary>
	Linux = 2,

	/// <summary>
	/// Running on Android.
	/// </summary>
	Android = 3,

	/// <summary>
	/// Running on Mac.
	/// </summary>
	Mac = 4,

	/// <summary>
	/// Running on iOS.
	/// </summary>
	iOS = 5,

	/// <summary>
	/// Running on WASM.
	/// </summary>
	WASM = 6,
};
DECLARE_ENUM_OPERATORS(PlatformType);

//平台
#if defined(DC_PLATFORM_WIN32)
	// Platform description
	//WIN32宏   --只要包含了 Windows.h，那么 WIN32 常量是肯定定义了的，所以不能用于判断平台环境
	//_WIN32	--32位和64位程序都有，且总是定义的.
	//_WIN64    --只有64位程序才有
	#if defined(_WIN64) 
		#define PLATFORM_64BITS 1
		#define PLATFORM_ARCH_X64 1
		#define PLATFORM_ARCH ArchitectureType::x64
	#else
		#define PLATFORM_64BITS 0
		#define PLATFORM_ARCH_X86 1
		#define PLATFORM_ARCH ArchitectureType::x86
	#endif
	#define PLATFORM_DESKTOP 1
	#define PLATFORM_TYPE PlatformType::Windows
#elif defined(DC_PLATFORM_LINUX)
	#if defined(_LINUX64)
		#define PLATFORM_64BITS 1
		#define PLATFORM_ARCH_X64 1
		#define PLATFORM_ARCH ArchitectureType::x64
	#else
		#define PLATFORM_64BITS 0
		#define PLATFORM_ARCH_X86 1
		#define PLATFORM_ARCH ArchitectureType::x86
	#endif
	#define PLATFORM_DESKTOP 1
	#define PLATFORM_TYPE PlatformType::Linux
#elif defined(DC_PLATFORM_ANDROID)
	#if defined(__arm__)
		#define PLATFORM_64BITS 0
		#define PLATFORM_ARCH ArchitectureType::ARM
		#define PLATFORM_ARCH_ARM 1
		#define PLATFORM_DEBUG_BREAK __asm__("trap")
	#elif defined(__aarch64__)
		#define PLATFORM_64BITS 1
		#define PLATFORM_ARCH_ARM64 1
		#define PLATFORM_ARCH ArchitectureType::ARM64
		#define PLATFORM_DEBUG_BREAK __asm__(".inst 0xd4200000")
	#elif defined(__i386__)
		#define PLATFORM_64BITS 0
		#define PLATFORM_ARCH_X86 1
		#define PLATFORM_ARCH ArchitectureType::x86
		#define PLATFORM_DEBUG_BREAK __asm__("int $3")
	#elif defined(__x86_64__)
		#define PLATFORM_64BITS 1
		#define PLATFORM_ARCH_X64 1
		#define PLATFORM_ARCH ArchitectureType::x64
		#define PLATFORM_DEBUG_BREAK __asm__("int $3")
	#else
		#error "Unknown Android ABI"
	#endif	
	#define PLATFORM_TYPE PlatformType::Android
#elif defined(DC_PLATFORM_WASM)
	#if defined(_WASM64)
		#define PLATFORM_64BITS 1
		#define PLATFORM_ARCH_X64 1
		#define PLATFORM_ARCH ArchitectureType::x64
	#else
		#define PLATFORM_64BITS 0
		#define PLATFORM_ARCH_X86 1
		#define PLATFORM_ARCH ArchitectureType::x86
	#endif
	#define PLATFORM_DESKTOP 1
	#define PLATFORM_TYPE PlatformType::WASM
#elif defined(DC_PLATFORM_MAC)
	#if defined(_ARM64) ||  defined(_X64)
		#define PLATFORM_64BITS 1
		#define PLATFORM_ARCH_X64 1
		#define PLATFORM_ARCH ArchitectureType::x64
	#else
		#define PLATFORM_64BITS 0
		#define PLATFORM_ARCH_X86 1
		#define PLATFORM_ARCH ArchitectureType::x86
	#endif
	#define PLATFORM_DESKTOP 1
	#define PLATFORM_TYPE PlatformType::Mac
#elif defined(DC_PLATFORM_IOS)
	#if defined(_ARM64) ||  defined(_X64)
		#define PLATFORM_64BITS 1
		#define PLATFORM_ARCH_X64 1
		#define PLATFORM_ARCH ArchitectureType::x64
	#else
		#define PLATFORM_64BITS 0
		#define PLATFORM_ARCH_X86 1
		#define PLATFORM_ARCH ArchitectureType::x86
	#endif
	#define PLATFORM_TYPE PlatformType::iOS
#else
	#error "Unknown PlatformType"
#endif

///////////////////////////////////////////////////////////////////////////////////////
//调试预定义宏定义
#if defined(DC_DEBUG)
	#define __ENTER_FUNCTION_FOXNET if(1){
	#define __LEAVE_FUNCTION_FOXNET }
#else
	#define __ENTER_FUNCTION_FOXNET if(1){
	#define __LEAVE_FUNCTION_FOXNET }
#endif


#if defined(DC_DEBUG)
	#define _MY_TRY 
	#define _MY_CATCH 
#else
	#define _MY_TRY try 
	#define _MY_CATCH catch(...){ Debuger::Error("Unknown exception");}
#endif

///////////////////////////////////////////////////////////////////////////////////////
//禁止默认拷贝构造函数和赋值函数
#define DISALLOW_COPY_ASSIGN(TypeName) \
	TypeName(const TypeName&) = delete; \
	TypeName(TypeName&&) = delete; \
	TypeName& operator=(const TypeName&) = delete; \
	TypeName& operator=(TypeName&&) = delete;
//禁止默认拷贝构造函数和赋值函数
#define DISALLOW_CONSTRUCTOR_COPY_ASSIGN(TypeName) \
	protected:\
	TypeName(){}\
	TypeName(const TypeName&) = delete; \
	TypeName(TypeName&&) = delete; \
	TypeName& operator=(const TypeName&) = delete; \
	TypeName& operator=(TypeName&&) = delete;

///////////////////////////////////////////////////////////////////////////////////////
//文件名
#define MAX_FILE_PATH	1024
//名
#define MAX_FILE_NAME   128

///////////////////////////////////////////////////////////////////////////////////////
//标志位设置
#define INSERT_FLAG(value, flag)	{(value) |= (flag);}
#define REMOVE_FLAG(value, flag)	{(value) &= ~(flag);}
#define HAS_FLAG(value, flag)		(((value) & (flag)) != 0 ? true : false )
//判断某位是否被置
//15.14....3.2.1.0 
#define ISSET0(x)				((x)&0x1)
#define ISSET1(x)				((x)&0x2)
#define ISSET2(x)				((x)&0x4)
#define ISSET3(x)				((x)&0x8)
#define ISSET4(x)				((x)&0x10)
#define ISSET5(x)				((x)&0x20)
#define ISSET6(x)				((x)&0x40)
#define ISSET7(x)				((x)&0x80)
#define ISSET8(x)				((x)&0x100)
#define ISSET9(x)				((x)&0x200)
#define ISSET10(x)				((x)&0x400)
#define ISSET11(x)				((x)&0x800)
#define ISSET12(x)				((x)&0x1000)
#define ISSET13(x)				((x)&0x2000)
#define ISSET14(x)				((x)&0x4000)
#define ISSET15(x)				((x)&0x8000)

//位操作
// 64位
#define MAKE_PAIR64(l, h)		uint64( uint(l) | ( uint64(h) << 32 ) )
#define PAIR64_HIPART(x)		(uint)((uint64(x) >> 32) & 0x00000000FFFFFFFFLL)
#define PAIR64_LOPART(x)		(uint)(uint64(x)         & 0x00000000FFFFFFFFLL)
// 32位
#define MAKE_PAIR32(l, h)		uint( ushort(l) | ( uint(h) << 16 ) )
#define PAIR32_HIPART(x)		(ushort)((uint(x) >> 16) & 0x0000FFFF)
#define PAIR32_LOPART(x)		(ushort)(uint(x)         & 0x0000FFFF)

///////////////////////////////////////////////////////////////////////////////////////
//颜色
#define COLOR_A(col)			(((col)>>24)&0xff)
#define COLOR_R(col)			(((col)>>16)&0xff)
#define COLOR_G(col)			(((col)>>8)&0xff)
#define COLOR_B(col)			(((col))&0xff)
#define COLOR_MAKE(a,r,g,b)		((a&0xff)<<24 | (r&0xff)<<16 | (g&0xff)<<8 | (b&0xff))

///////////////////////////////////////////////////////////////////////////////////////
//数组
#define ARRAY_SIZE(array)		(sizeof(array)/sizeof(array[0]))

///////////////////////////////////////////////////////////////////////////////////////
//stl
#define CALLBACK_0(__selector__,__target__, ...) std::bind(&__selector__,__target__, ##__VA_ARGS__)
#define CALLBACK_1(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, ##__VA_ARGS__)
#define CALLBACK_2(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, std::placeholders::_2, ##__VA_ARGS__)
#define CALLBACK_3(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, ##__VA_ARGS__)
#define CALLBACK_4(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, ##__VA_ARGS__)
#define CALLBACK_5(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, ##__VA_ARGS__)

///////////////////////////////////////////////////////////////////////////////////////
//字符串宏
#if defined(DC_PLATFORM_WIN32)
#define Strcpy	strcpy_s
#define Strncpy	strncpy_s
#define Strcat strcat_s
#define Strchr strchr
#define Strncmp strncmp
#define Stricmp _stricmp
#define Strnicmp _strnicmp
#define Strlen strlen
#define Strnlen strnlen
#define Strstr strstr
#define Strtok strtok_s
#else
#define Strcpy strcpy
#define Strncpy strncpy
#define Strcat strcat
#define Strchr strchr
#define Strncmp strncmp
#define Stricmp strcasecmp
#define Strnicmp strncasecmp
#define Strlen strlen
#define Strnlen strnlen
#define Strstr strstr
#define Strtok strtok_r
#endif

#if defined(DC_PLATFORM_WIN32)
#define Sscanf sscanf_s
//#define Sprintf sprintf_s
#define Snprintf _snprintf_s
#define Vsnprintf vsnprintf_s
#else
#define Sscanf sscanf
//#define Sprintf sprintf
#define Snprintf snprintf
#define Vsnprintf vsnprintf
#endif

///////////////////////////////////////////////////////////////////////////////////////

//线程锁
#define LOCK(_mutex) std::lock_guard<std::mutex> lock(_mutex)

//未使用参数
#ifndef UNUSED
	#define UNUSED(x) (void)(x)
#endif