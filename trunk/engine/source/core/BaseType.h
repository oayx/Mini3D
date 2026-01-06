 
/*****************************************************************************************************/
// @author hannibal
// @date   2015/04/21
// @brief  基础数据类型定义
/*****************************************************************************************************/
#pragma once

///////////////////////////////////////////////////////////////////////////////////////
//c头文件
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>
#include <assert.h>
#include <wchar.h>
#include <stdint.h>

//标准库
#include <string>
#include <cstring>
#include <vector>
#include <list>
#include <map>
#include <deque>
#include <queue>
#include <stack>
#include <set>
#include <array>
#include <algorithm>
#include <functional>
#include <atomic>
//多线程
#include <thread>
#include <mutex>
#include <atomic>
#include <future>
//流
#include <iostream>
#include <sstream>
#include <fstream>
//其他
#include <variant>
#include <limits>

#include "BaseType.h"
#include "Compiler.h"
#include "Enums.h"
#include "RTTI.h"
#include "memory/Memory.h"
#include "reflection/Reflection.h"

#include "tracy/tracy/Tracy.hpp"

DC_BEGIN_NAMESPACE
///////////////////////////////////////////////////////////////////////////////////////
//类型重定义
typedef std::uint8_t		byte;
typedef std::uint16_t		ushort;
typedef std::uint32_t		uint;
//typedef unsigned long		ulong;//在windows始终为4字节；在linux在32位机器是4字节，在64位机器是8字节；尽量不用这种类型
typedef std::int64_t		int64;//8字节
typedef std::uint64_t		uint64;//8字节

#ifndef DC_PLATFORM_WIN32
typedef std::uint32_t		DWORD;//windows 在64位和32位系统下这个值始终是32位的；linux long 32系统是4字节，64位系统是8字节；这里统一定义成4字节
typedef std::int16_t		WORD;
typedef int              BOOL;
#endif

//回调
typedef std::function<void()> Action;


///////////////////////////////////////////////////////////////////////////////////////
#ifdef TRACY_ENABLE // Use Tracy profiler
	#define DC_PROFILE ZoneScoped
	#define DC_PROFILE_NAME(name) ZoneScopedN(#name)
    /// Macro for scoped profiling with a name and color.
    #define DC_PROFILE_COLOR(name, color) ZoneScopedNC(#name, color)
    /// Macro for scoped profiling with a dynamic string name.
    #define DC_PROFILE_STR(nameStr, size) ZoneName(nameStr, size)
    /// Macro for marking a game frame.
    #define DC_PROFILE_FRAME FrameMark
    /// Macro for recording name of current thread.
    #define DC_PROFILE_THREAD(name) tracy::SetThreadName(name)
    /// Macro for scoped profiling of a function.
    #define DC_PROFILE_FUNCTION ZoneScopedN(__FUNCTION__)

    /// Color used for highlighting event.
    #define DC_PROFILE_EVENT_COLOR tracy::Color::OrangeRed
    /// Color used for highlighting resource.
    #define DC_PROFILE_RESOURCE_COLOR tracy::Color::MediumSeaGreen
#else // Tracy profiler off
	#define DC_PROFILE
	#define DC_PROFILE_NAME(name)
    #define DC_PROFILE_COLOR(name, color)
    #define DC_PROFILE_STR(nameStr, size)
    #define DC_PROFILE_FRAME
    #define DC_PROFILE_THREAD(name)
    #define DC_PROFILE_FUNCTION

    #define DC_PROFILE_EVENT_COLOR
    #define DC_PROFILE_RESOURCE_COLOR
#endif


///////////////////////////////////////////////////////////////////////////////////////
//所有class的基类
class ENGINE_DLL object
{
	DECLARE_ALLOCATOR;
	BEGIN_REFECTION_TYPE(object)
	END_FINAL_REFECTION_TYPE;
};

//键-值对
template<class K, class V>
class ENGINE_DLL KeyValuePair : public object
{
public:
	KeyValuePair(K k, V v) : key(k), value(v)
	{
	}
	KeyValuePair(const KeyValuePair<K, V>& f) : key(f.key), value(f.value)
	{
	}
	const KeyValuePair<K, V>& operator=(const KeyValuePair<K, V>& f) noexcept
	{
		key = f.key; value = f.value;
		return *this;
	}
	constexpr bool operator> (const KeyValuePair<K, V>& f)const noexcept
	{
		return key > f.key;
	}
	constexpr bool operator< (const KeyValuePair<K,V>& f)const noexcept
	{
		return key < f.key;
	}	
	constexpr bool operator== (const KeyValuePair<K, V>& f)const noexcept
	{
		return key == f.key && value == f.value;
	}
	constexpr bool operator!= (const KeyValuePair<K, V>& f)const noexcept
	{
		return key != f.key || value != f.value;
	}
public:
	K key;
	V value;
};

//渲染API
enum class RendererAPI : byte
{
	DirectX9,
	DirectX11,
	DirectX12,
	OpenGL,
	OpenGLES,
	Vulkan,
	Metal,
};
DECLARE_ENUM_OPERATORS(RendererAPI);

//图片类型
enum class ImageType : byte
{
	Undefined = 0,
	BMP,
	JPEG,
	TGA,
	PNG,
	GIF,
	HDR,
	RAW,
	DDS,
	PKM,
	PVR,
	ASTC,
	CUBE,		//天空盒
	Max
};
DECLARE_ENUM_OPERATORS(ImageType);

//对齐方式
enum class AligeType : byte
{
	None = 0,
	Right,
	RightBottom,
	Bottom,
	LeftBottom,
	Left,
	LeftTop,
	Top,
	RightTop,
	Middle,
};
DECLARE_ENUM_OPERATORS(AligeType);

//模拟空间
enum class SimulationSpace : byte
{
	Local = 0,
	World
};
DECLARE_ENUM_OPERATORS(SimulationSpace);

//时间缩放
enum class DeltaTime : byte
{
	Scaled = 0,
	Unscaled
};
DECLARE_ENUM_OPERATORS(DeltaTime);

DC_END_NAMESPACE
/*
类型	win32	win64	linux32	linux64
char	1		1		1		1
short	2		2		2		2
int		4		4		4		4
long	4		4		4		8
longlong8		8		8		8
float	4		4		4		4
double	8		8		8		8
void*	4		8		4		8
*/