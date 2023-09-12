
/*****************************************************************************
* Author： hannibal
* Description：原子操作
*****************************************************************************/
#pragma once

#include <stdint.h>
#include <string>
#include "Compiler.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL Atomic
{
public:
	//(相当于=)
	static std::int64_t Exchange(std::int64_t volatile* dst, std::int64_t exchange);
	//(相当于?:)
	static int			CompareExchange(int volatile* dst, int exchange, int comperand);
	//(相当于?:)
	static std::int64_t CompareExchange(std::int64_t volatile* dst, std::int64_t exchange, std::int64_t comperand);
	//(相当于++)
	static std::int64_t Increment(std::int64_t volatile* dst);
	//(相当于--)
	static std::int64_t Decrement(std::int64_t volatile* dst);
	//(相当于+)
	static std::int64_t Add(std::int64_t volatile* dst, std::int64_t value);
	//取值
	static int			Read(int volatile* dst);
	//取值
	static std::int64_t Read(std::int64_t volatile* dst);
	//(相当于=)
	static void			Store(int volatile* dst, int value);
	//(相当于=)
	static void			Store(std::int64_t volatile* dst, std::int64_t value);
};
DC_END_NAMESPACE