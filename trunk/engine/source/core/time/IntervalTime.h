 
/*****************************************************************************************************/
// @author hannibal
// @date   2020/05/22
// @brief  精确间隔循环
/*****************************************************************************************************/
#pragma once

#include "core/Object.h"
#include <chrono>

DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL IntervalTime Final : public Object
{
	DISALLOW_COPY_ASSIGN(IntervalTime);
	BEGIN_DERIVED_REFECTION_TYPE(IntervalTime, Object)
	END_DERIVED_REFECTION_TYPE;

public:
	IntervalTime() {}
	/// <summary>
	/// 初始化定时器
	/// </summary>
	/// <param name="interval">触发间隔</param>
	/// <param name="start">是否第一帧开始执行</param>
	void Init(float interval, bool first_frame = false)
	{
		m_active = true;
		m_now_time = 0.0f;
		m_interval_time = interval;
		if (first_frame) m_now_time = m_interval_time;
	}

	void Reset()
	{
		m_now_time = 0.0f;
	}

	bool Update(float elapse_time)
	{
		if (!m_active) return false;
		m_now_time += elapse_time;
		if (m_now_time >= m_interval_time)
		{
			m_now_time -= m_interval_time;
			return true;
		}
		return false;
	}

private:
	bool m_active = false;
	float m_interval_time = 0.0f;
	float m_now_time = 0.0f;
};

/********************************************************************/
//每几帧执行一次>
class ENGINE_DLL LogicIntervalTime : public Object
{
	DISALLOW_COPY_ASSIGN(LogicIntervalTime);
	BEGIN_DERIVED_REFECTION_TYPE(LogicIntervalTime, Object)
	END_DERIVED_REFECTION_TYPE;

public:
	LogicIntervalTime() {}

	void Init(uint interval, bool start = false)
	{
		m_active = true;
		m_interval_frame = interval;
		if (start) m_now_frame = m_interval_frame;
	}

	void Reset()
	{
		m_now_frame = 0;
	}

	bool Update(uint elapse_frame)
	{
		if (!m_active) return false;
		m_now_frame += elapse_frame;
		if (m_now_frame >= m_interval_frame)
		{
			m_now_frame -= m_interval_frame;
			return true;
		}
		return false;
	}

private:
	bool m_active = false;
	uint m_interval_frame = 0;
	uint m_now_frame = 0;
};
DC_END_NAMESPACE
