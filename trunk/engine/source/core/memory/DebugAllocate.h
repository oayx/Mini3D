
/*****************************************************************************
* Author： hannibal
* Description：调试模式下的内存分配
*****************************************************************************/
#pragma once

#include <cmath>
#include <mutex>
#include <atomic>
#include <stdint.h>
#include "Allocate.h"
#include "core/Assert.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class DebugMalloc final : public Malloc
{
	friend class Memory;

	enum
	{
		BEGIN_MASK = 0xDEADC0DE,
		END_MASK = 0xDEADC0DE,
		RECORD_NUM = 32, //必须大于2
		CALLSTACK_NUM = 32
	};
	typedef struct Block
	{
		uint32_t size = 0;			//申请空间的大小
		bool alignment = false;		//是否字节对齐
		Block* prev = nullptr;		//前一个节点
		Block* next = nullptr;		//后一个节点
	}Block;

private:
	DebugMalloc() = default;
	DebugMalloc(const DebugMalloc&) = delete;
	DebugMalloc(const DebugMalloc&&) = delete;
	DebugMalloc& operator=(const DebugMalloc&) = delete;
	DebugMalloc& operator=(const DebugMalloc&&) = delete;
	~DebugMalloc() = default;

public:
	virtual void* Alloc(uint32_t size)override { return AlignedAlloc(size, 0); }
	virtual void* AlignedAlloc(uint32_t size, uint32_t alignment)override;
	virtual void* Realloc(void* addr, uint32_t size)override;
	virtual void Free(void* addr)override { AlignedFree(addr, 0); }
	virtual void Free(void* addr, uint32_t bytes)override { AlignedFree(addr, 0); }
	virtual void AlignedFree(void* addr, uint32_t alignment)override;

	void Output();

private:
	void InsertBlock(Block* block);
	void RemoveBlock(Block* block);

private:
	std::mutex _mutex;
	Block* _head = nullptr;
	Block* _tail = nullptr;
	std::atomic_uint64_t _numNewCalls = 0;
	std::atomic_uint64_t _numDeleteCalls = 0;
	std::atomic_uint64_t _numBlocks = 0;
	std::atomic_uint64_t _numBytes = 0;
	std::atomic_uint64_t _maxNumBytes = 0;
	std::atomic_uint64_t _maxNumBlocks = 0;
	std::atomic_uint64_t _sizeRecord[RECORD_NUM] = {0};
};

inline void* DebugMalloc::AlignedAlloc(uint32_t size, uint32_t alignment)
{
	_numNewCalls++;
	uint32_t uiExtendedSize = sizeof(Block) + sizeof(uint32_t) + size + sizeof(uint32_t);
	char* addr = (char*)::malloc(uiExtendedSize);
	if (!addr)
		return nullptr;

	Block* block = (Block*)addr;
	InsertBlock(block);
	block->size = size;
	block->alignment = (alignment > 0) ? true : false;

	addr += sizeof(Block);
	uint32_t* begin_mask = (uint32_t *)(addr);
	*begin_mask = BEGIN_MASK;

	addr += sizeof(uint32_t);
	uint32_t* end_mask = (uint32_t *)(addr + size);
	*end_mask = END_MASK;

	_numBlocks++;
	_numBytes += size;
	if (_numBytes > _maxNumBytes)
	{
		_maxNumBytes.exchange(_numBytes);
	}
	if (_numBlocks > _maxNumBlocks)
	{
		_maxNumBlocks.exchange(_numBlocks);
	}

	uint32_t uiTwoPowerI = 1;
	int i;
	for (i = 0; i <= RECORD_NUM - 2; i++, uiTwoPowerI <<= 1)
	{
		if (size <= uiTwoPowerI)
		{
			_sizeRecord[i]++;
			break;
		}
	}
	if (i == RECORD_NUM - 1)
	{
		_sizeRecord[i]++;
	}

	return (void*)addr;
}
inline void* DebugMalloc::Realloc(void* addr, uint32_t size)
{
	if (!addr || !size)return nullptr;
	char* old_ptr = (char*)addr;

	old_ptr -= sizeof(uint32_t);
	uint32_t *begin_mask = (uint32_t*)old_ptr;
	AssertEx(*begin_mask == BEGIN_MASK, "");

	old_ptr -= sizeof(Block);
	Block* old_block = (Block*)old_ptr;

	char* new_ptr = nullptr;
	{//创建新的
		new_ptr = (char*)Alloc(size);
		::memcpy(new_ptr, addr, old_block->size);
	}
	{//删除旧的
		Free(addr);
	}
	return new_ptr;
}
inline void DebugMalloc::AlignedFree(void* addr, uint32_t alignment)
{
	if (!addr)return;

	_numDeleteCalls++;

	char* ptr = (char*)addr;
	ptr -= sizeof(uint32_t);
	uint32_t *begin_mask = (uint32_t *)(ptr);
	AssertEx(*begin_mask == BEGIN_MASK, "");

	ptr -= sizeof(Block);
	Block* block = (Block*)ptr;
	RemoveBlock(block);

	AssertEx(_numBlocks > 0 && _numBytes >= block->size, "");
	AssertEx(block->alignment == alignment > 0, "");

	uint32_t * end_mask = (uint32_t *)(ptr + sizeof(Block) + sizeof(uint32_t) + block->size);
	AssertEx(*end_mask == END_MASK, "");

	_numBlocks--;
	_numBytes -= block->size;

	::free(ptr);
}
inline void DebugMalloc::InsertBlock(Block* block)
{
	std::lock_guard<std::mutex> lock(_mutex);
	if (_tail)
	{
		block->prev = _tail;
		block->next = nullptr;
		_tail->next = block;
		_tail = block;
	}
	else
	{
		block->prev = nullptr;
		block->next = nullptr;
		_head = block;
		_tail = block;
	}
}
inline void DebugMalloc::RemoveBlock(Block* block)
{
	std::lock_guard<std::mutex> lock(_mutex);
	if (block->prev)
		block->prev->next = block->next;
	else
		_head = block->next;

	if (block->next)
		block->next->prev = block->prev;
	else
		_tail = block->prev;
}
inline void DebugMalloc::Output()
{
	Debuger::Output("---------------------DebugMalloc-------------------------");
	Debuger::Output("new call count:%llu, free call count:%llu", (uint64_t)_numNewCalls, (uint64_t)_numDeleteCalls);
	Debuger::Output("max num blocks:%llu, max num bytes:%llu", (uint64_t)_maxNumBlocks, (uint64_t)_maxNumBytes);
	for (int i = 0; i < RECORD_NUM; ++i)
	{
		Debuger::Output("size layout:%u, count:%llu", (uint32_t)::pow(2, i), (uint64_t)_sizeRecord[i]);
	}
	Debuger::Output("unfree memory count:%llu, size:%llu", (uint64_t)_numBlocks, (uint64_t)_numBytes);
}
DC_END_NAMESPACE