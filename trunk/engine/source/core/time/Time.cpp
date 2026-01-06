#include "Time.h"
#include "platform/PlatformDefine.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(Time, Object);
void Time::Initialize()
{
	Update();
}
void Time::Update()
{
	float time = GetRealTimeSinceStartup();
	_timeDelta = time - _time;
	_time = time;

	if (_timeRecord < 0)
	{
		_timeRecord = GetRealTimeSinceStartup();
		_frameRecord = GetFrameCount();
	}

	int frame = GetFrameCount();
	if (time - _timeRecord >= 1)
	{
		_fps = frame - _frameRecord;
		_timeRecord = time;
		_frameRecord = frame;
	}

	if (_frameCount < 0)
	{
		_frameCount = 0;
	}
	else
	{
		_frameCount++;
	}
}
DateTime Time::GetDate()
{
	DateTime date;

	int64 t = GetTimeMS();
	date.milli_second = t % 1000;

	time_t ts = (time_t)(t / 1000);
	auto tm = localtime(&ts);

	date.year = tm->tm_year + 1900;
	date.month = tm->tm_mon + 1;
	date.day = tm->tm_mday;
	date.hour = tm->tm_hour;
	date.minute = tm->tm_min;
	date.second = tm->tm_sec;
	date.week_day = tm->tm_wday;

	return date;
}

int64 Time::GetTimeMS()
{
	int64 t;

#if defined(DC_PLATFORM_WIN32)
	SYSTEMTIME sysTime;
	::GetLocalTime(&sysTime);

	struct tm tm;
	tm.tm_sec = sysTime.wSecond;
	tm.tm_min = sysTime.wMinute;
	tm.tm_hour = sysTime.wHour;
	tm.tm_mday = sysTime.wDay;
	tm.tm_mon = sysTime.wMonth - 1;
	tm.tm_year = sysTime.wYear - 1900;
	tm.tm_isdst = -1;

	t = mktime(&tm) * (int64)1000 + sysTime.wMilliseconds;
#else
	struct timeval tv;
	gettimeofday(&tv, nullptr);
	t = tv.tv_sec;
	t *= 1000;
	t += tv.tv_usec / 1000;
#endif

	return t;
}

int64 Time::GetUTCTimeMS()
{
	int64 t = GetTimeMS();
	int ms = t % 1000;
	time_t ts = (time_t)(t / 1000);
	auto tm = gmtime(&ts);
	t = mktime(tm) * (int64)1000 + ms;

	return t;
}

float Time::GetRealTimeSinceStartup()
{
	int64 time = _clockTime.micro();
	return (float)time / 1000000.0f;
}
DC_END_NAMESPACE
