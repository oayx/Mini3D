
/*****************************************************************************
* Author： hannibal
* Description：内存分配
*****************************************************************************/
#pragma once

#include "core/Compiler.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class Malloc
{
	friend class Memory;

public:
	virtual void* Alloc(uint32_t size) = 0;
	virtual void* Realloc(void* addr, uint32_t size) = 0;
	virtual void Free(void* addr) = 0;
	virtual void Free(void* addr, uint32_t n) = 0;
	virtual void* AlignedAlloc(uint32_t size, uint32_t alignment) = 0;
	virtual void AlignedFree(void* addr, uint32_t alignment) = 0;
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
DC_END_NAMESPACE