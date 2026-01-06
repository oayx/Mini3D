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

DC_BEGIN_NAMESPACE

/********************************************************************/
//重定义new
#if defined(DC_DEBUG) && !defined(DC_MALLOC) && !defined(DC_DEBUG_MALLOC) && defined(DC_PLATFORM_WIN32)
	#define _CRTDBG_MAP_ALLOC
	#include <crtdbg.h>
	#define NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#else
	#define NEW new
#endif

/********************************************************************/
class ENGINE_DLL Memory final
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

public://对象创建和销毁
	template<class T>
	static T* New()
	{
#if defined(DC_MALLOC) || defined(DC_DEBUG_MALLOC)
		T* ptr = (T*)Memory::Alloc(sizeof(T));
		ConstructDestruct::ConstructItem(ptr);
		return ptr;
#else
		return NEW T();
#endif
	}
	template<class T, class... Args>
	static T* New(Args&&...args)
	{
#if defined(DC_MALLOC) || defined(DC_DEBUG_MALLOC)
		T* ptr = (T*)Memory::Alloc(sizeof(T));
		ConstructDestruct::ConstructItemArgs(ptr, std::forward<Args>(args)...);
		return ptr;
#else
		return NEW T(std::forward<Args>(args)...);
#endif
	}
	template<class T>
	static T* NewArray(uint32_t count)
	{
#if defined(DC_MALLOC) || defined(DC_DEBUG_MALLOC)
		T* ptr = (T*)Memory::Alloc(sizeof(T) * count);
		ConstructDestruct::ConstructItems(ptr, count);
		return ptr;
#else
		return NEW T[count];
#endif
	}
	template<class T>
	static void Delete(T* ptr)
	{
#if defined(DC_MALLOC) || defined(DC_DEBUG_MALLOC)
		ConstructDestruct::DestructItem(ptr);
		Memory::Free((void*)ptr);
#else
		delete ptr;
#endif
	}
	//POD数组的析构
	template<class T>
	static void DeleteArray(T* ptr)
	{
#if defined(DC_MALLOC) || defined(DC_DEBUG_MALLOC)
		ConstructDestruct::DestructItems(ptr);
		Memory::Free(ptr);
#else
		delete[] ptr;
#endif
	}
	//普通数组析构
	template<class T>
	static void DeleteArray(T* ptr, uint32_t count)
	{
#if defined(DC_MALLOC) || defined(DC_DEBUG_MALLOC)
		ConstructDestruct::DestructItems(ptr, count);
		Memory::Free(ptr);
#else
		delete[] ptr;
#endif
	}
};

/********************************************************************/
#if defined(DC_MALLOC) || defined(DC_DEBUG_MALLOC)
#define DECLARE_ALLOCATOR \
	public: \
	inline void* operator new(size_t size)			{ return Memory::Alloc(size, __FILE__ , __LINE__); }   \
	inline void  operator delete(void* ptr)         { Memory::Free(ptr); }   \
	inline void* operator new(size_t, void* ptr)	{ return ptr; }   \
	inline void  operator delete(void*, void*)      { }   \
	inline void* operator new[](size_t size)		{ return Memory::Alloc(size, __FILE__ , __LINE__); }   \
	inline void  operator delete[](void* ptr)       { Memory::Free(ptr); }   \
	inline void* operator new[](size_t, void* ptr)  { return ptr; }   \
	inline void  operator delete[](void*, void*)    { } \
	private:
#else
#define DECLARE_ALLOCATOR
#endif

/********************************************************************/
//根据指针值删除内存
#define SAFE_DELETE(x)				{if( (x)!=NULL ) { Memory::Delete(x); (x)=NULL; }}

//根据指针值删除数组类型内存
#define SAFE_DELETE_ARRAY(x)		{if( (x)!=NULL ) { Memory::DeleteArray(x); (x)=NULL; }}

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


//创建
#define DEFAULT_CREATE(T) \
public: \
template<class... Args> \
static T* Create(Args&&...args) \
{ \
	return Memory::New<T>(std::forward<Args>(args)...); \
} \
private:

//友元
#define FRIEND_CONSTRUCT_DESTRUCT(T) \
friend class Memory; \
friend class ConstructDestruct; \
friend class ConstructorImpl<T>;

DC_END_NAMESPACE