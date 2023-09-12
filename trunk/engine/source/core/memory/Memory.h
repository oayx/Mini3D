/*****************************************************************************************************/
// @author hannibal
// @date   2021/05/18
// @brief  内存管理
/*****************************************************************************************************/
#pragma once

#include "CrtAllocate.h"
#include "DebugAllocate.h"
#include "DefaultAllocate.h"
#include "ConstructDestruct.h"

//重定义new
#if defined(DC_DEBUG) && !defined(DC_MALLOC) && !defined(DC_DEBUG_MALLOC)
	#define _CRTDBG_MAP_ALLOC
	#include <crtdbg.h>
	#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#else
	#define DBG_NEW new
#endif

DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL Memory Final
{
	friend class ProgressLife;
private:
	Memory() = delete;
	Memory(const Memory&) = delete;
	Memory(const Memory&&) = delete;
	Memory& operator=(const Memory&) = delete;
	Memory& operator=(const Memory&&) = delete;

public://分配銷毀
	static void* Alloc(uint32_t size)
	{
		return GetMalloc().Alloc(size);
	}
	static void* Alloc(uint32_t size, const char* file, int line)
	{
		return GetMalloc().Alloc(size);
	}
	static void* AlignedAlloc(uint32_t size, uint32_t alignment)
	{
		return GetMalloc().AlignedAlloc(size, alignment);
	}
	static void* Realloc(void* addr, uint32_t size)
	{
		return GetMalloc().Realloc(addr, size);
	}
	static void Free(void* addr)
	{
		GetMalloc().Free(addr);
	}
	static void Free(void* addr, uint32_t size)
	{
		GetMalloc().Free(addr, size);
	}
	static void AlignedFree(void* addr, uint32_t alignment)
	{
		GetMalloc().AlignedFree(addr, alignment);
	}

private:
	static CrtMalloc& GetCrtMalloc()
	{
		static CrtMalloc _crtMalloc;
		return _crtMalloc;
	}
	static DebugMalloc& GetDebugAllocate()
	{
		static DebugMalloc _debugMalloc;
		return _debugMalloc;
	}
	static DefaultMalloc& GetDefaultMalloc()
	{
		static DefaultMalloc _defaultMalloc;
		return _defaultMalloc;
	}
	
	static Malloc& GetMalloc() noexcept
	{
#if defined(DC_MALLOC)
		return GetDefaultMalloc();
#elif defined(DC_DEBUG_MALLOC)
		return GetDebugAllocate(); 
#else
		return GetCrtMalloc();
#endif
	}

public://内存操作
	static void Copy(void* dst, const void* src, uint32_t size)
	{
		::memcpy(dst, src, static_cast<size_t>(size));
	}
	static void Set(void* dst, uint32_t size, int value)
	{
		::memset(dst, value, static_cast<size_t>(size));
	}
	static void Clear(void* dst, uint32_t size)
	{
		::memset(dst, 0, static_cast<size_t>(size));
	}
	static int Compare(const void* buf1, const void* buf2, uint32_t size)
	{
		return ::memcmp(buf1, buf2, static_cast<size_t>(size));
	}
};

/********************************************************************/
//根据指针值删除内存
#define SAFE_DELETE(x)				{if( (x)!=NULL ) { delete(x); (x)=NULL; }}

//根据指针值删除数组类型内存
#define SAFE_DELETE_ARRAY(x)		{if( (x)!=NULL ) { delete[](x); (x)=NULL; }}

//根据指针调用free接口
#ifndef SAFE_FREE
#define SAFE_FREE(x)				{if( (x)!=NULL ) { Memory::Free(x); (x)=NULL; }}
#endif
//根据指针调用Release接口
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x)				{if( (x)!=NULL ) { (x)->Release(); (x)=NULL; }}
#endif
//释放句柄
#ifndef SAFE_CLOSEHANDLE
#define SAFE_CLOSEHANDLE(x)			{if( (x)!=NULL ) { ::CloseHandle(x); (x)=NULL; }}
#endif
/********************************************************************/
template<class T>
inline T* New()
{
#if defined(DC_MALLOC) || defined(DC_DEBUG_MALLOC)
	T* ptr = (T*)Memory::Alloc(sizeof(T));
	ConstructDestruct::ConstructItem(ptr);
	return ptr;
#else
	return DBG_NEW T();
#endif
}
template<class T, class... Args>
inline T* New(Args&&...args)
{
#if defined(DC_MALLOC) || defined(DC_DEBUG_MALLOC)
	T* ptr = (T*)Memory::Alloc(sizeof(T));
	ConstructDestruct::ConstructItemArgs(ptr, std::forward<Args>(args)...);
	return ptr;
#else
	return DBG_NEW T(std::forward<Args>(args)...);
#endif
}
template<class T>
inline T* NewArray(uint32_t count)
{
#if defined(DC_MALLOC) || defined(DC_DEBUG_MALLOC)
	T* ptr = (T*)Memory::Alloc(sizeof(T) * count);
	ConstructDestruct::ConstructItems(ptr, count);
	return ptr;
#else
	return DBG_NEW T[count];
#endif
}
template<class T>
inline void Delete(T* ptr)
{
#if defined(DC_MALLOC) || defined(DC_DEBUG_MALLOC)
	ConstructDestruct::DestructItem(ptr);
	Memory::Free(ptr);
#else
	SAFE_DELETE(ptr);
#endif
}
template<class T>
inline void DeleteArray(T* ptr)
{
#if defined(DC_MALLOC) || defined(DC_DEBUG_MALLOC)
	ConstructDestruct::DestructItems(ptr);
	Memory::Free(ptr);
#else
	SAFE_DELETE_ARRAY(ptr);
#endif
}
template<class T>
inline void DeleteArray(T* ptr, uint32_t count)
{
#if defined(DC_MALLOC) || defined(DC_DEBUG_MALLOC)
	ConstructDestruct::DestructItems(ptr, count);
	Memory::Free(ptr);
#else
	SAFE_DELETE_ARRAY(ptr);
#endif
}

//创建
#define DEFAULT_CREATE(T) \
public: \
template<class... Args> \
static T* Create(Args&&...args) \
{ \
	return DBG_NEW T(std::forward<Args>(args)...); \
} \
private:

//友元
#define FRIEND_CONSTRUCT_DESTRUCT(T) \
friend T* New<T>(); \
friend T* NewArray<T>(uint32_t count); \
friend void Delete<T>(T* ptr); \
friend void DeleteArray<T>(T* ptr, uint32_t count);\
friend class ConstructDestruct; \
friend class ConstructorImpl<T>;

DC_END_NAMESPACE