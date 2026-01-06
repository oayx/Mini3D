 
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
class ENGINE_DLL IntervalTime final : public Object
{
	DISALLOW_COPY_ASSIGN(IntervalTime);
	BEGIN_DERIVED_REFECTION_TYPE(IntervalTime, Object)
	END_REFECTION_TYPE;

public:
	IntervalTime() {}
	/// <summary>
	/// 初始化定时器
	/// </summary>
	/// <param name="interval">触发间隔</param>
	/// <param name="start">是否第一帧开始执行</param>
	void Init(float interval, bool first_frame = false)
	{
		_active = true;
		_now_time = 0.0f;
		_interval_time = interval;
		if (first_frame) _now_time = _interval_time;
	}

	void Reset()
	{
		_now_time = 0.0f;
	}

	bool Update(float elapse_time)
	{
		if (!_active) return false;
		_now_time += elapse_time;
		if (_now_time >= _interval_time)
		{
			_now_time -= _interval_time;
			return true;
		}
		return false;
	}

private:
	bool _active = false;
	float _interval_time = 0.0f;
	float _now_time = 0.0f;
};

/********************************************************************/
//每几帧执行一次>
class ENGINE_DLL LogicIntervalTime : public Object
{
	DISALLOW_COPY_ASSIGN(LogicIntervalTime);
	BEGIN_DERIVED_REFECTION_TYPE(LogicIntervalTime, Object)
	END_REFECTION_TYPE;

public:
	LogicIntervalTime() {}

	void Init(uint interval, bool start = false)
	{
		_active = true;
		_interval_frame = interval;
		if (start) _now_frame = _interval_frame;
	}

	void Reset()
	{
		_now_frame = 0;
	}

	bool Update(uint elapse_frame)
	{
		if (!_active) return false;
		_now_frame += elapse_frame;
		if (_now_frame >= _interval_frame)
		{
			_now_frame -= _interval_frame;
			return true;
		}
		return false;
	}

private:
	bool _active = false;
	uint _interval_frame = 0;
	uint _now_frame = 0;
};
DC_END_NAMESPACE
