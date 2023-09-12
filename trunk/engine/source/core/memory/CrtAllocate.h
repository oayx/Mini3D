/*****************************************************************************************************/
// @author hannibal
// @date   2021/05/18
// @brief  c内存管理
/*****************************************************************************************************/
#pragma once

#include <stdlib.h>
#include <stdint.h>
#include "Allocate.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class CrtMalloc Final : public Malloc
{
	friend class Memory;

private:
	CrtMalloc() = default;
	CrtMalloc(const CrtMalloc&) = delete;
	CrtMalloc(const CrtMalloc&&) = delete;
	CrtMalloc& operator=(const CrtMalloc&) = delete;
	CrtMalloc& operator=(const CrtMalloc&&) = delete;
	~CrtMalloc() = default;

public://内存
	virtual void* Alloc(uint32_t size)override
	{
		return ::malloc((size_t)size);
	}
	virtual void* Realloc(void* addr, uint32_t size)override
	{
		return ::realloc(addr, (size_t)size);
	}
	virtual void Free(void* addr)override
	{
		::free(addr);
	}
	virtual void Free(void* addr, uint32_t n)override
	{
		::free(addr);
	}
	virtual void* AlignedAlloc(uint32_t size, uint32_t alignment)override
	{
		return ::malloc((size_t)size);
		//if (alignment & (alignment - 1))
		//{
		//	return nullptr;
		//}
		////维护FreeBlock(void)指针占用的内存大小-----用sizeof获取
		//int pSize = sizeof(void*);
		//// 求得让FreeBlock
		//// 内存对齐所需要的内存大小
		//size_t requestedSize = size_t(size + alignment - 1 + pSize);
		//// 分配的大小
		//void* rawMemory = ::malloc(requestedSize);
		//// 实际分配的内存地址（差不多就是指针运算数字形式）
		//uintptr_t start = (uintptr_t)rawMemory + pSize;

		//// 此处的 & 能高位保留，是由于~导致其他位无关位都变成了1
		//void* aligned = (void*)((start + alignment - 1) & ~(alignment - 1));

		//// 维护一个指向malloc()真正分配的内存的指针
		//*(void**)((uintptr_t)aligned - pSize) = rawMemory;

		//return aligned;     //因为做了向上的舍入，所以不需要多余的清理操作
	}
	virtual void AlignedFree(void* addr, uint32_t alignment)override
	{
		::free(addr);
		//void* mem = *(void**)((uintptr_t)addr - sizeof(void*));
		//::free(mem);
	}
};
DC_END_NAMESPACE