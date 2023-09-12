
/*****************************************************************************
* Author： hannibal
* Description：stl分配器
*****************************************************************************/
#pragma once

#include <stdint.h>
#include "Memory.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
template<class T>
class STLMalloc
{
public:
	//以下的 typedef 是必须的,因为在容器中有类似 _alloc::pointer 的语法 
	typedef T 			value_type;
	typedef T* 			pointer;
	typedef T&			reference;
	typedef const T*	const_pointer;
	typedef const T& 	const_reference;
	typedef size_t		size_type;
	typedef ptrdiff_t	difference_type;

	using propagate_on_container_move_assignment = std::true_type;
	using is_always_equal = std::true_type;

	template<class U>
	struct rebind
	{
		using other = STLMalloc<U>;
	};

	STLMalloc() {}
	STLMalloc(const STLMalloc&) = default;
	template<class U>
	STLMalloc(const STLMalloc<U>&) {}
	STLMalloc<T>& operator=(const STLMalloc&)
	{
		return *this;
	}
	template<class U>
	STLMalloc& operator=(const STLMalloc<U>&)
	{
		return *this;
	}

	pointer allocate()
	{
		return allocate(1);
	}
	pointer allocate(size_type size, const void* = 0)
	{
		return (pointer)Memory::Alloc(size * sizeof(T));
	}
	void deallocate(pointer p)
	{
		deallocate(p, 1);
	}
	void deallocate(pointer p, size_type n)
	{
		Memory::Free(p, n * sizeof(T));
	}
	void construct(pointer p, const_reference x)
	{
		new(p) T(x);
	}
	void destory(pointer p)
	{
		p->~T();
	}
	pointer address(reference x)const noexcept
	{
		return (pointer)&x;
	}
	const_pointer address(const_reference x)const noexcept
	{
		return (const_pointer)&x;
	}
	size_type max_size() const noexcept
	{
		return size_type(UINT_MAX / sizeof(T));
	}
};
#if defined(DC_MALLOC) || defined(DC_DEBUG_MALLOC)
	template<class T, class _Other>
	inline bool operator==(const STLMalloc<T>&, const STLMalloc<_Other>&) noexcept
	{
		return (true);
	}
	template<class T, class _Other>
	inline bool operator!=(const STLMalloc<T>&, const STLMalloc<_Other>&) noexcept
	{
		return (false);
	}
	#define STLAlloc STLMalloc
#else
	#define STLAlloc std::allocator
#endif
DC_END_NAMESPACE