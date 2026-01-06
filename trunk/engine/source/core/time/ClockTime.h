 
/*****************************************************************************************************/
// @author hannibal
// @date   2015/05/22
// @brief  时间统计
/*****************************************************************************************************/
#pragma once

#include "core/Object.h"
#include <chrono>

DC_BEGIN_NAMESPACE
/********************************************************************/
/**
* 计时器
*/
class ENGINE_DLL ClockTime final : public Object
{
	BEGIN_DERIVED_REFECTION_TYPE(ClockTime, Object)
	END_REFECTION_TYPE;

public:
	ClockTime() : _begin(std::chrono::high_resolution_clock::now()) {}
	ClockTime(const ClockTime& other) :_begin(other._begin) {}
	ClockTime(ClockTime&& other) :_begin(std::move(other._begin)) {}
	ClockTime& operator =(const ClockTime& other) noexcept { _begin = other._begin; return *this; }
	ClockTime& operator =(ClockTime&& other) noexcept { _begin = std::move(other._begin); return *this; }

	void reset() { _begin = std::chrono::high_resolution_clock::now(); }

	//默认输出毫秒
	int64_t millisecond() const
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - _begin).count();
	}

	//微秒
	int64_t micro() const
	{
		return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - _begin).count();
	}

	//纳秒
	int64_t nano() const
	{
		return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - _begin).count();
	}

	//秒
	int64_t seconds() const
	{
		return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - _begin).count();
	}

	//分
	int64_t minutes() const
	{
		return std::chrono::duration_cast<std::chrono::minutes>(std::chrono::high_resolution_clock::now() - _begin).count();
	}

	//时
	int64_t hours() const
	{
		return std::chrono::duration_cast<std::chrono::hours>(std::chrono::high_resolution_clock::now() - _begin).count();
	}

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> _begin;
};//_ClockTime_H
DC_END_NAMESPACE
