 
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
class ENGINE_DLL ClockTime Final : public Object
{
	DISALLOW_COPY_ASSIGN(ClockTime);
	BEGIN_DERIVED_REFECTION_TYPE(ClockTime, Object)
	END_DERIVED_REFECTION_TYPE;

public:
	ClockTime() : m_begin(std::chrono::high_resolution_clock::now()) {}
	void reset() { m_begin = std::chrono::high_resolution_clock::now(); }

	//默认输出毫秒
	int64_t millisecond() const
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - m_begin).count();
	}

	//微秒
	int64_t micro() const
	{
		return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - m_begin).count();
	}

	//纳秒
	int64_t nano() const
	{
		return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - m_begin).count();
	}

	//秒
	int64_t seconds() const
	{
		return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - m_begin).count();
	}

	//分
	int64_t minutes() const
	{
		return std::chrono::duration_cast<std::chrono::minutes>(std::chrono::high_resolution_clock::now() - m_begin).count();
	}

	//时
	int64_t hours() const
	{
		return std::chrono::duration_cast<std::chrono::hours>(std::chrono::high_resolution_clock::now() - m_begin).count();
	}

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> m_begin;
};//_ClockTime_H
DC_END_NAMESPACE
