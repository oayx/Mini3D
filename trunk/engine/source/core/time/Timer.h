 
/*****************************************************************************************************/
// @author hannibal
// @date   2020/02/12
// @brief  定时器
/*****************************************************************************************************/
#pragma once

#include "core/Object.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
typedef std::function<void()> TimerCallback0;
typedef std::function<void(String)> TimerCallback1;

/**
* 定时器内容
*/
class TimerEntity Final : public object
{
public:
	TimerEntity(uint id_, float rate_, int ticks_, TimerCallback0 callback_);
	TimerEntity(uint id_, float rate_, int ticks_, TimerCallback1 callback_, String info);
	~TimerEntity();

public:
	void Update(float elapse);

public:
	uint				id;
	bool				isActive;
	float				mRate;
	int					mTicks;
	int					mTicksElapsed;
	float				mLast;

	TimerCallback0		mCallBack;
	TimerCallback1		mCallBack1;
	String				mInfo;
};

/********************************************************************/
/**
* 定时器
*/
class ENGINE_DLL Timer Final : public Object
{
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(Timer);
	BEGIN_DERIVED_REFECTION_TYPE(Timer, Object)
	END_DERIVED_REFECTION_TYPE;

public:
	// <summary>
	// 增加定时器
	// </summary>
	// <param name="rate">触发频率(单位s)</param>
	// <param name="callBack">触发回调函数</param>
	// <returns>新定时器id</returns>
	static uint AddLoop(float rate, TimerCallback0 callBack);
	static uint AddLoop(float rate, TimerCallback1 callBack, String info);
	static uint AddOnce(float rate, TimerCallback0 callBack);
	static uint AddOnce(float rate, TimerCallback1 callBack, String info);
	// <summary>
	// 增加定时器，可以指定循环次数
	// </summary>
	// <param name="rate">触发频率(单位s)</param>
	// <param name="ticks">循环次数，如果是0则不会自动删除</param>
	// <param name="callBack">触发回调函数</param>
	// <param name="info">回调参数</param>
	// <returns>新定时器id</returns>
	static uint AddTimer(float rate, int ticks, TimerCallback0 callBack);
	static uint AddTimer(float rate, int ticks, TimerCallback1 callBack, String info);
	// <summary>
	// 下一帧执行一次
	// </summary>
	// <param name="callBack"></param>
	static void CallLater(TimerCallback0 callBack);
	static void CallLater(TimerCallback1 callBack, String info);

	// <summary>
	// 移除定时器
	// </summary>
	// <param name="timerId">Timer GUID</param>
	static void RemoveTimer(uint timerId);
	static void RemoveAllTimer();

	static void Update();
	//是否受TimeScale影响
	static void UseUnscaledTime(bool b) { _useUnscaledTime = b; }

private:
	// <summary>
	// 移除过期定时器
	// </summary>
	static void Remove();

private:
	static uint	m_idCounter;
	static bool _useUnscaledTime;
	static List<TimerEntity> m_listTimers;
};//Timer
DC_END_NAMESPACE
