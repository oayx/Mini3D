#include "Timer.h"
#include "Time.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
TimerEntity::TimerEntity(uint id_, float rate_, int ticks_, TimerCallback0 callback_)
{
	id = id_;
	mRate = rate_ < 0 ? 0 : rate_;
	mTicks = ticks_ < 0 ? 0 : ticks_;
	mCallBack = callback_;
	mCallBack1 = nullptr;
	mInfo = "";
	mLast = 0;
	mTicksElapsed = 0;
	isActive = true;
}

TimerEntity::TimerEntity(uint id_, float rate_, int ticks_, TimerCallback1 callback_, String info)
{
	id = id_;
	mRate = rate_ < 0 ? 0 : rate_;
	mTicks = ticks_ < 0 ? 0 : ticks_;
	mCallBack = nullptr;
	mCallBack1 = callback_;
	mInfo = info;
	mLast = 0;
	mTicksElapsed = 0;
	isActive = true;
}

TimerEntity::~TimerEntity()
{
}

void TimerEntity::Update(float elapse)
{
	mLast += elapse;
	if (isActive && mLast >= mRate)
	{
		if (mCallBack1 != nullptr)
			mCallBack1(mInfo);
		else if (mCallBack != nullptr)
			mCallBack();

		mLast = 0;
		mTicksElapsed++;

		if (mTicks > 0 && mTicksElapsed >= mTicks)
		{
			Timer::RemoveTimer(id);
		}
	}
}

/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(Timer, Object);
uint Timer::m_idCounter = 0;
bool Timer::_useUnscaledTime = false;
List<TimerEntity> Timer::m_listTimers;
uint Timer::AddLoop(float rate, TimerCallback0 callBack)
{
	return AddTimer(rate, 0, callBack);
}
uint Timer::AddLoop(float rate, TimerCallback1 callBack, String info)
{
	return AddTimer(rate, 0, callBack, info);
}
uint Timer::AddOnce(float rate, TimerCallback0 callBack)
{
	return AddTimer(rate, 1, callBack);
}
uint Timer::AddOnce(float rate, TimerCallback1 callBack, String info)
{
	return AddTimer(rate, 1, callBack, info);
}
uint Timer::AddTimer(float rate, int ticks, TimerCallback0 callBack)
{
	if (rate < 0) rate = 0;
	TimerEntity newTimer(++m_idCounter, rate, ticks, callBack);
	m_listTimers.Add(newTimer);
	if (rate == 0) newTimer.Update(0);
	return newTimer.id;
}
uint Timer::AddTimer(float rate, int ticks, TimerCallback1 callBack, String info)
{
	if (rate < 0) rate = 0;
	TimerEntity newTimer(++m_idCounter, rate, ticks, callBack, info);
	m_listTimers.Add(newTimer);
	if (rate == 0) newTimer.Update(0);
	return newTimer.id;
}
void Timer::CallLater(TimerCallback0 callBack)
{
	TimerEntity newTimer(++m_idCounter, 0, 1, callBack);
	m_listTimers.Add(newTimer);
}
void Timer::CallLater(TimerCallback1 callBack, String info)
{
	TimerEntity newTimer(++m_idCounter, 0, 1, callBack, info);
	m_listTimers.Add(newTimer);
}
void Timer::RemoveTimer(uint timerId)
{
	if (timerId == 0) return;
	//修改状态
	for (TimerEntity& timer : m_listTimers)
	{
		if (timer.id == timerId)
		{
			timer.isActive = false;
			break;
		}
	}
}
void Timer::RemoveAllTimer()
{
	for (TimerEntity& timer : m_listTimers)
	{
		if (timer.id != 0)
		{//只删除非循环定时器
			timer.isActive = false;
		}
	}
}
void Timer::Update()
{
	Remove();
	if (!m_listTimers.IsEmpty())
	{
		//时间缩放
		float delta_time = _useUnscaledTime ? Time::GetUnscaledDeltaTime() : Time::GetDeltaTime();
		for (TimerEntity& timer : m_listTimers)
		{
			timer.Update(delta_time);
		}
	}
}
void Timer::Remove()
{
	if (!m_listTimers.IsEmpty())
	{
		std::list<TimerEntity>::iterator it = m_listTimers.begin();
		while (it != m_listTimers.end())
		{
			if (!(*it).isActive)
				m_listTimers.Remove(it++);
			else
				it++;
		}
	}
}
DC_END_NAMESPACE
