#pragma once

#include "Allocate.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
//处理大块内存的申请与释放，直接与操作系统进行交易
class LargeMalloc Final
{
public:
	static void *Allocate(uint32_t n)
	{
		return ::malloc(n);
	}
	static void Deallocate(void *p, uint32_t n)
	{
		::free(p);
	}
}; 
/********************************************************************/
// 链表式内存管理
class DefaultMalloc Final : public Malloc
{
	friend class Memory;

	enum { _DEFAULT_ALLOC_CHUNK = 20 };             // 自由链表不够时，默认分配的节点数量
	enum { _ALIGN = 8 };                            // free_list分配节点的最小单位
	enum { _MAX_BYTES = 128 };                      // free_list分配节点的最大值
	enum { _N_FREE_LIST = _MAX_BYTES / _ALIGN };    // 自由链表的个数
	enum { _OFFSET_DATA = sizeof(uint32_t) };		// 有效数据偏移(size)

	typedef union Obj
	{
		union Obj* free_list_link;  // 未分配器：指向自由链表节点的指针
		uint32_t size;				// 分配后：客户数据(前4个字节保留大小，不给客户使用)
	}Obj;

private:
	DefaultMalloc() = default;
	DefaultMalloc(const DefaultMalloc&) = delete;
	DefaultMalloc(const DefaultMalloc&&) = delete;
	DefaultMalloc& operator=(const DefaultMalloc&) = delete;
	DefaultMalloc& operator=(const DefaultMalloc&&) = delete;
	~DefaultMalloc() = default;

public:
	virtual void* Alloc(uint32_t bytes)override;
	virtual void* AlignedAlloc(uint32_t bytes, uint32_t alignment)override;
	virtual void* Realloc(void* addr, uint32_t bytes)override;
	virtual void Free(void* addr)override;
	virtual void Free(void* addr, uint32_t bytes)override;
	virtual void AlignedFree(void* addr, uint32_t alignment)override;

private:
	// 把不足_ALIGN的位都舍掉，向上取到最小的_ALIGN的倍数，最终输出：0,8,16,24,32。。。
	uint32_t ROUND_UP(uint32_t bytes);
	// 获得链表索引，返回0,1,2。。。15
	uint32_t FREELIST_INDEX(uint32_t bytes);
	// 从内存池中取若干(20)个内存块，第一块返回给用户，其余挂到相应的自由链表中
	void* Refill(uint32_t bytes);
	//怎么拿到足够多的内存交给自由链表
	char* ChunkAlloc(uint32_t size, uint32_t &nobjs);

private:
	std::mutex _mutex;

	// 自由链表
	Obj* _free_list[_N_FREE_LIST] = {0};

	// 内存池
	char* _start_free = 0;
	char* _end_free = 0;
	uint32_t _heap_size = 0;
};
inline void* DefaultMalloc::Alloc(uint32_t bytes)
{
	Obj* result = nullptr;
	//总共需要申请的大小
	uint32_t size = bytes + _OFFSET_DATA;
	// 申请的内存太大，调用一级空间配置器
	if (size > (uint32_t)_MAX_BYTES)
	{
		result = (Obj*)LargeMalloc::Allocate(size);
		result->size = bytes;
		return (char*)result + _OFFSET_DATA;
	}

	{
		std::lock_guard<std::mutex> lock(_mutex);

		// 如果申请的内存小于 _MAX_ALIGN，则从自由链表中取
		uint32_t index = FREELIST_INDEX(size);
		result = _free_list[index];
		if (0 == result)
		{
			result = (Obj*)Refill(ROUND_UP(size));
			result->size = bytes;
			return (char*)result + _OFFSET_DATA;
		}
		else
		{
			_free_list[index] = result->free_list_link;
			result->size = bytes;
			return (char*)result + _OFFSET_DATA;
		}
	}
}
inline void* DefaultMalloc::AlignedAlloc(uint32_t bytes, uint32_t alignment)
{
	return nullptr;
}
inline void* DefaultMalloc::Realloc(void* addr, uint32_t bytes)
{
	if (!addr || !bytes)return nullptr;
	char* old_ptr = (char*)addr;
	old_ptr -= _OFFSET_DATA;
	Obj* old_obj = (Obj*)old_ptr;

	char* new_ptr = nullptr;
	{//创建新的
		new_ptr = (char*)Alloc(bytes);
		::memcpy(new_ptr, addr, old_obj->size);
		((Obj*)(new_ptr - _OFFSET_DATA))->size = bytes;
	}
	{//删除旧的
		Free(addr);
	}
	return new_ptr;
}
inline void DefaultMalloc::Free(void* addr)
{
	if (!addr)return;

	char* ptr = (char*)addr;
	ptr -= _OFFSET_DATA;
	Obj* obj = (Obj*)ptr;

	uint32_t size = obj->size + _OFFSET_DATA;
	if (size > (uint32_t)_MAX_BYTES)
	{
		LargeMalloc::Deallocate(ptr, size);
		return;
	}

	{
		std::lock_guard<std::mutex> lock(_mutex);
		uint32_t index = FREELIST_INDEX(size);
		obj->free_list_link = _free_list[index];
		_free_list[index] = obj->free_list_link;
	}
}
inline void DefaultMalloc::Free(void* addr, uint32_t bytes)
{
	if (!addr)return;
#if defined(DC_DEBUG)
	char* ptr = (char*)addr;
	ptr -= _OFFSET_DATA;
	Obj* obj = (Obj*)ptr;
	AssertEx(bytes == obj->size, "");
#endif
	Free(addr);
}
inline void DefaultMalloc::AlignedFree(void* addr, uint32_t alignment)
{
	if (!addr)return;
	Free(addr);
}
inline uint32_t DefaultMalloc::ROUND_UP(uint32_t bytes)
{
	return (((bytes)+_ALIGN - 1) & ~(_ALIGN - 1));
}
inline uint32_t DefaultMalloc::FREELIST_INDEX(uint32_t bytes)
{
	return ((bytes)+_ALIGN - 1) / (_ALIGN)-1;
}
inline void* DefaultMalloc::Refill(uint32_t bytes)
{
	// 默认一次取20个对象的空间
	uint32_t nobjs = _DEFAULT_ALLOC_CHUNK;
	char *chunk = ChunkAlloc(bytes, nobjs);

	// 当num为1的时候, 直接将block返回给用户即可. 如果不是1的话, 再返回之前要先将剩下个节点串接在自由链表上. 这也就是下面for循环的作用.
	if (1 == nobjs)
		return chunk;

	uint32_t index = FREELIST_INDEX(bytes);
	Obj **myFreeList = _free_list + index;

	Obj *result = (Obj *)chunk;
	Obj *cur_obj = nullptr;
	Obj *next_obj = (Obj *)(chunk + bytes);	// chunk的类型是char *, 加上bytes后，指向下一个节点的起始地址
	*myFreeList = next_obj;
	for (uint32_t i = 1; i < nobjs; ++i)		// 把从内存池中多取的内存挂到相应的自由链表中
	{
		cur_obj = next_obj;
		next_obj = (Obj *)((char *)next_obj + bytes);   //next_obj = next_obj + 1;

		cur_obj->free_list_link = next_obj;
	}
	cur_obj->free_list_link = nullptr;			// 把最后一个节点的 _freeListLink 置空

	return result;
}
inline char* DefaultMalloc::ChunkAlloc(uint32_t size, uint32_t &nobjs)
{
	char *result = nullptr;
	uint32_t total_bytes = size * nobjs;//总共需要分配的字节数，最终不一定够
	uint32_t bytes_left = _end_free - _start_free;// char *类型的指针直接相减，得到剩余内存的字节数

	// 1 内存池中的剩余内存足够分配nobjs个对象的空间
	if (bytes_left >= total_bytes)
	{
		result = _start_free;
		_start_free += total_bytes;
		return result;
	}
	// 2 内存池中剩余内存不够分配nobjs个对象的空间，但是至少能分配一个大小为size的空间
	else if (bytes_left > size)
	{
		nobjs = bytes_left / size;				// 修改能分配的对象空间个数
		total_bytes = size * nobjs;				// 重新修改总的字节数
		result = _start_free;                  // 返回值
		_start_free += total_bytes;			// 修改内存池起始地址
		return result;
	}
	// 3 内存池中的剩余内存连一个对象的空间都分不出来了
	else
	{
		// 内存池中可能还有一些零头，把这些内存分配到相应的自由链表中
		if (bytes_left > 0)
		{
			uint32_t index = FREELIST_INDEX(bytes_left);

			// 调整_free_list，将内存池中剩余的内存挂到自由链表中
			((Obj *)_start_free)->free_list_link = _free_list[index];//先保存
			_free_list[index] = (Obj *)_start_free;//再指向新的
		}

		// 内存池内存不足的解决方法 ......
		// 解决方法 1 ： 找堆(操作系统)要
		uint32_t bytes_to_get = 2 * total_bytes + ROUND_UP(_heap_size >> 4);    // 反馈调节用
		_start_free = (char *)::malloc(bytes_to_get);
		if (0 == _start_free)
		{// 堆空间内存不足， malloc失败

			// 解决方法 2 ：要在自由链表中检索较大的区块，
			uint32_t index = 0;
			for (uint32_t i = size; i < _MAX_BYTES; i += _ALIGN)
			{
				index = FREELIST_INDEX(i);
				Obj *tmp = _free_list[index];
				if (tmp)    // index下的自由链表不空
				{
					_free_list[index] = tmp->free_list_link;		// 把第一个不空的自由链表的第一个节点用tmp取下来，并把其他节点连接起来
					_start_free = (char*)tmp;
					_end_free = _start_free + i;					// 因为_start_free是char*类型的指针
					return ChunkAlloc(size, nobjs);					// 这个时候nobjs虽然没有改变，递归进去的时候，(如果需要)会改变的
				}
			}

			// 在自由链表中没有找到足够大的块(都用完了)，就会走到这里
			// 解决方法 3 ：看看一级空间配置器能不能出点力(以及空间配置器如果指定了内存不足时的处理函数，就有可能空出内存)

			// 接下来的一级空间配置器如果抛异常，就需要之前重置_end_free，如果不重置，下次进入ChunkAlloc时，_start_free == 0, _end_free没改，内存池会很大，其实并不是
			_end_free = 0;
			_start_free = (char*)LargeMalloc::Allocate(bytes_to_get);
		}

		_heap_size += bytes_to_get;
		_end_free = _start_free + bytes_to_get;

		return ChunkAlloc(size, nobjs);
	}
}
DC_END_NAMESPACE