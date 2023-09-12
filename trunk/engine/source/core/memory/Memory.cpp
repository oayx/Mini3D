#include "Memory.h" 
#include "core/Debuger.h"
#include "core/Assert.h"
#include "platform/PlatformDefine.h"


USING_NAMESPACE_DC;
#if defined(DC_OPERATOR_NEW)
void* operator new(std::size_t n) noexcept(false) 
{ 
	return Memory::Alloc((uint32_t)n);
}
void* operator new[](std::size_t n) noexcept(false) 
{ 
	return Memory::Alloc((uint32_t)n);
}
void operator delete(void* p) noexcept 
{
	Memory::Free(p);
}
void operator delete[](void* p) noexcept 
{
	Memory::Free(p);
}
void* operator new  (std::size_t n, const std::nothrow_t& tag) noexcept 
{ 
	return Memory::Alloc((uint32_t)n);
}
void* operator new[](std::size_t n, const std::nothrow_t& tag) noexcept 
{
	return Memory::Alloc((uint32_t)n);;
}

#if (__cplusplus >= 201402L || _MSC_VER >= 1916)
void operator delete  (void* p, std::size_t n) noexcept 
{ 
	Memory::Free(p, (uint32_t)n);
}
void operator delete[](void* p, std::size_t n) noexcept
{
	Memory::Free(p, (uint32_t)n);
}
#endif

#if (__cplusplus > 201402L || defined(__cpp_aligned_new))
void* operator new(std::size_t n, std::align_val_t al)   noexcept(false) 
{
	return Memory::AlignedAlloc(n, (uint32_t)al);
}
void* operator new[](std::size_t n, std::align_val_t al) noexcept(false) 
{
	return Memory::AlignedAlloc(n, (uint32_t)al);
}
void* operator new  (std::size_t n, std::align_val_t al, const std::nothrow_t&) noexcept 
{
	return Memory::AlignedAlloc(n, (uint32_t)al);
}
void* operator new[](std::size_t n, std::align_val_t al, const std::nothrow_t&) noexcept 
{
	return Memory::AlignedAlloc(n, (uint32_t)al);
}
void operator delete (void* p, std::align_val_t al) noexcept 
{
	Memory::AlignedFree(p, (uint32_t)al);
} 
void operator delete[](void* p, std::align_val_t al) noexcept 
{
	Memory::AlignedFree(p, (uint32_t)al);
}
void operator delete  (void* p, std::size_t n, std::align_val_t al) noexcept 
{
	Memory::AlignedFree(p, (uint32_t)al);
}
void operator delete[](void* p, std::size_t n, std::align_val_t al) noexcept 
{
	Memory::AlignedFree(p, (uint32_t)al);
}
#endif
#endif

DC_BEGIN_NAMESPACE
/********************************************************************/
static void OnLogMalloc(const char* msg, void* arg)
{
	Debuger::Output(msg, arg);
}
class ProgressLife
{
public:
	ProgressLife()
	{
#if defined(DC_PLATFORM_WIN32)
	#if defined(DC_DEBUG) && !defined(DC_MALLOC) && !defined(DC_DEBUG_MALLOC)
		_CrtDumpMemoryLeaks();//内存泄露检查
	#endif
		::SetConsoleOutputCP(65001);//解决控制台中文乱码
#endif
	}
	~ProgressLife()
	{
#if defined(DC_DEBUG_MALLOC)
		Memory::GetDebugAllocate().Output();
#endif
	}
	static ProgressLife g_ProgressLife;
};
ProgressLife ProgressLife::g_ProgressLife;
DC_END_NAMESPACE