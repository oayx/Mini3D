
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

	// 容器特性
	using propagate_on_container_move_assignment = std::true_type;
	using propagate_on_container_copy_assignment = std::false_type;
	using propagate_on_container_swap = std::false_type;
	using is_always_equal = std::true_type;

	// 模板重绑定支持
	template<class U>
	struct rebind
	{
		using other = STLMalloc<U>;
	};

	// 构造函数和赋值操作符
	STLMalloc() noexcept = default;
	STLMalloc(const STLMalloc&) noexcept = default;
	template<class U>
	STLMalloc(const STLMalloc<U>&)noexcept {}

	STLMalloc<T>& operator=(const STLMalloc&)
	{
		return *this;
	}
	template<class U>
	STLMalloc& operator=(const STLMalloc<U>&)
	{
		return *this;
	}

	// 析构函数
	~STLMalloc() = default;

	pointer allocate()
	{
		return allocate(1);
	}
	pointer allocate(size_type size, const void* = nullptr)
	{
		if (size > max_size())
			throw std::bad_alloc();

		// 分配内存（添加对齐处理）
		const size_t bytes = size * sizeof(T);
		void* ptr = Memory::Alloc(bytes);
		if (!ptr) throw std::bad_alloc();
		return static_cast<pointer>(ptr);
	}
	void deallocate(pointer p)
	{
		deallocate(p, 1);
	}
	void deallocate(pointer p, size_type n)
	{
		if (p == nullptr)
			return; // 标准要求允许释放空指针

		// 获取实际分配的字节数（根据分配器实现）
		const size_t bytes = n * sizeof(T);
		const size_t alignment = alignof(T);

		// 使用合适的释放函数
		if (alignment > __STDCPP_DEFAULT_NEW_ALIGNMENT__) {
			// 过度对齐类型特殊处理
			Memory::Free(p, bytes);
		}
		else {
			// 标准对齐类型:DefaultMalloc不需要传递大小，内部有记录
			Memory::Free(p);
		}
	}

	// 对象构造/析构
	template<typename U, typename... Args>
	void construct(U* p, Args&&... args) 
	{
		::new(static_cast<void*>(p)) U(std::forward<Args>(args)...);
	}
	template<typename U>
	void destroy(U* p) 
	{
		p->~U();
	}

	// 地址获取
	pointer address(reference x)const noexcept
	{
		return (pointer)&x;
	}
	const_pointer address(const_reference x)const noexcept
	{
		return (const_pointer)&x;
	}

	// 最大可分配大小
	size_type max_size() const noexcept
	{
		return std::numeric_limits<size_type>::max() / sizeof(T);
	}

	// C++20 添加分配器比较支持
	bool operator==(const STLMalloc&) const noexcept 
	{
		return true;
	}
	bool operator!=(const STLMalloc& other) const noexcept 
	{
		return !(*this == other);
	}
};
#if defined(DC_MALLOC) || defined(DC_DEBUG_MALLOC)
	#define STLAlloc STLMalloc
#else
	#define STLAlloc std::allocator
#endif
DC_END_NAMESPACE