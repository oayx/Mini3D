#include "Atomic.h"
#include "platform/PlatformDefine.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
std::int64_t Atomic::Exchange(std::int64_t volatile* dst, std::int64_t exchange)
{
#if defined(DC_PLATFORM_WIN32)
	return InterlockedExchange64(dst, exchange);
#elif defined(DC_PLATFORM_LINUX) || defined(DC_PLATFORM_ANDROID)
	return __sync_lock_test_and_set(dst, exchange);
#else

#endif
}
int Atomic::CompareExchange(int volatile* dst, int exchange, int comperand)
{
#if defined(DC_PLATFORM_WIN32)
	static_assert(sizeof(int) == sizeof(long), "Invalid LONG size.");
	return _InterlockedCompareExchange((long volatile*)dst, (long)exchange, (long)comperand);
#elif defined(DC_PLATFORM_LINUX) || defined(DC_PLATFORM_ANDROID)
	return __sync_val_compare_and_swap(dst, comperand, exchange);
#else

#endif
}
std::int64_t Atomic::CompareExchange(std::int64_t volatile* dst, std::int64_t exchange, std::int64_t comperand)
{
#if defined(DC_PLATFORM_WIN32)
	return InterlockedCompareExchange64(dst, exchange, comperand);
#elif defined(DC_PLATFORM_LINUX) || defined(DC_PLATFORM_ANDROID)
	return __sync_val_compare_and_swap(dst, comperand, exchange);
#else

#endif
}
std::int64_t Atomic::Increment(std::int64_t volatile* dst)
{
#if defined(DC_PLATFORM_WIN32)
	return InterlockedIncrement64(dst);
#elif defined(DC_PLATFORM_LINUX) || defined(DC_PLATFORM_ANDROID)
	return __sync_add_and_fetch(dst, 1);
#else

#endif
}
std::int64_t Atomic::Decrement(std::int64_t volatile* dst)
{
#if defined(DC_PLATFORM_WIN32)
	return InterlockedDecrement64(dst);
#elif defined(DC_PLATFORM_LINUX) || defined(DC_PLATFORM_ANDROID)
	return __sync_sub_and_fetch(dst, 1);
#else

#endif
}
std::int64_t Atomic::Add(std::int64_t volatile* dst, std::int64_t value)
{
#if defined(DC_PLATFORM_WIN32)
	return InterlockedExchangeAdd64(dst, value);
#elif defined(DC_PLATFORM_LINUX) || defined(DC_PLATFORM_ANDROID)
	return __sync_fetch_and_add(dst, value);
#else

#endif
}
int Atomic::Read(int volatile* dst)
{
#if defined(DC_PLATFORM_WIN32)
	static_assert(sizeof(int) == sizeof(long), "Invalid LONG size.");
	return _InterlockedCompareExchange((long volatile*)dst, 0, 0);
#elif defined(DC_PLATFORM_LINUX) || defined(DC_PLATFORM_ANDROID)
	int result;
	__atomic_load(dst, &result, __ATOMIC_SEQ_CST);
	return result;
#else

#endif
}
std::int64_t Atomic::Read(std::int64_t volatile* dst)
{
#if defined(DC_PLATFORM_WIN32)
	return InterlockedCompareExchange64(dst, 0, 0);
#elif defined(DC_PLATFORM_LINUX) || defined(DC_PLATFORM_ANDROID)
	std::int64_t result;
	__atomic_load(dst, &result, __ATOMIC_SEQ_CST);
	return result;
#else

#endif
}
void Atomic::Store(int volatile* dst, int value)
{
#if defined(DC_PLATFORM_WIN32)
	static_assert(sizeof(int) == sizeof(long), "Invalid LONG size.");
	_InterlockedExchange((long volatile*)dst, (long)value);
#elif defined(DC_PLATFORM_LINUX) || defined(DC_PLATFORM_ANDROID)
	__atomic_store(dst, &value, __ATOMIC_SEQ_CST);
#else

#endif
}
void Atomic::Store(std::int64_t volatile* dst, std::int64_t value)
{
#if defined(DC_PLATFORM_WIN32)
	InterlockedExchange64(dst, value);
#elif defined(DC_PLATFORM_LINUX) || defined(DC_PLATFORM_ANDROID)
	__atomic_store(dst, &value, __ATOMIC_SEQ_CST);
#else

#endif
}
DC_END_NAMESPACE