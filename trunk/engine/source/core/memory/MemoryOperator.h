#include "Memory.h" 

USING_NAMESPACE_DC;

#if defined(DC_OPERATOR_NEW)
inline void* operator new(std::size_t n) noexcept(false) 
{ 
	return Memory::Alloc((uint32_t)n);
}
inline void* operator new[](std::size_t n) noexcept(false)
{ 
	return Memory::Alloc((uint32_t)n);
}
inline void operator delete(void* p) noexcept
{
	Memory::Free(p);
}
inline void operator delete[](void* p) noexcept
{
	Memory::Free(p);
}
inline void* operator new  (std::size_t n, const std::nothrow_t& tag) noexcept
{ 
	return Memory::Alloc((uint32_t)n);
}
inline void* operator new[](std::size_t n, const std::nothrow_t& tag) noexcept
{
	return Memory::Alloc((uint32_t)n);;
}

#if (__cplusplus >= 201402L || _MSC_VER >= 1916)
inline void operator delete  (void* p, std::size_t n) noexcept
{ 
	Memory::Free(p, (uint32_t)n);
}
inline void operator delete[](void* p, std::size_t n) noexcept
{
	Memory::Free(p, (uint32_t)n);
}
#endif

#if (__cplusplus > 201402L || defined(__cpp_aligned_new))
inline void* operator new(std::size_t n, std::align_val_t al)   noexcept(false)
{
	return Memory::AlignedAlloc(n, (uint32_t)al);
}
inline void* operator new[](std::size_t n, std::align_val_t al) noexcept(false)
{
	return Memory::AlignedAlloc(n, (uint32_t)al);
}
inline void* operator new  (std::size_t n, std::align_val_t al, const std::nothrow_t&) noexcept
{
	return Memory::AlignedAlloc(n, (uint32_t)al);
}
inline void* operator new[](std::size_t n, std::align_val_t al, const std::nothrow_t&) noexcept
{
	return Memory::AlignedAlloc(n, (uint32_t)al);
}
inline void operator delete (void* p, std::align_val_t al) noexcept
{
	Memory::AlignedFree(p, (uint32_t)al);
} 
inline void operator delete[](void* p, std::align_val_t al) noexcept
{
	Memory::AlignedFree(p, (uint32_t)al);
}
inline void operator delete  (void* p, std::size_t n, std::align_val_t al) noexcept
{
	Memory::AlignedFree(p, (uint32_t)al);
}
inline void operator delete[](void* p, std::size_t n, std::align_val_t al) noexcept
{
	Memory::AlignedFree(p, (uint32_t)al);
}
#endif //__cplusplus
#endif //DC_OPERATOR_NEW