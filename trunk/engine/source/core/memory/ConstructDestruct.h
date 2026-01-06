/*****************************************************************************************************/
// @author hannibal
// @date   2021/06/18
// @brief  构造析构
/*****************************************************************************************************/
#pragma once

#include <type_traits>
#include "core/Compiler.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class ConstructDestruct final
{
public:
	template<typename T>
	static typename std::enable_if<!std::is_trivially_constructible<T>::value>::type ConstructItem(T* dst)
	{
		new(dst) T();
	}
	template<typename T>
	static typename std::enable_if<std::is_trivially_constructible<T>::value>::type ConstructItem(T* dst)
	{
	}
	template<typename T>
	static typename std::enable_if<!std::is_trivially_constructible<T>::value>::type ConstructItems(T* dst, int count)
	{
		while (count--)
		{
			new(dst) T();
			++(T*&)dst;
		}
	}
	template<typename T>
	static typename std::enable_if<std::is_trivially_constructible<T>::value>::type ConstructItems(T* dst, int count)
	{
	}
	template<class T, class... Args>
	static void ConstructItemArgs(T* dst, Args&&...args)
	{
		new(dst) T(std::forward<Args>(args)...);
	}
	template<typename T>
	static typename std::enable_if<!std::is_trivially_destructible<T>::value>::type DestructItem(T* dst)
	{
		dst->~T();
	}
	template<typename T>
	static typename std::enable_if<std::is_trivially_destructible<T>::value>::type DestructItem(T* dst)
	{
	}

	//删除不带析构函数的对象
	//NOTE:不需要提供非POT的DestructItems，如果执行，就报编译错误
	template<typename T>
	static typename std::enable_if<std::is_trivially_destructible<T>::value>::type DestructItems(T* dst)
	{
	}

	template<typename T>
	static typename std::enable_if<!std::is_trivially_destructible<T>::value>::type DestructItems(T* dst, int count)
	{
		while (count--)
		{
			dst->~T();
			++dst;
		}
	}
	template<typename T>
	static typename std::enable_if<std::is_trivially_destructible<T>::value>::type DestructItems(T* dst, int count)
	{
	}
};
DC_END_NAMESPACE