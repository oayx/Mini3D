#include "Time.h"
#include "platform/PlatformDefine.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(Time, Object);
ClockTime Time::m_clock_time;
float Time::m_time_delta = 0;
float Time::m_time_record = -1;
float Time::m_time = 0;
float Time::m_time_scale = 1.0f;
int Time::m_frame_count = -1;
int Time::m_frame_record;
int Time::m_fps;

void Time::Initialize()
{
	Update();
}
void Time::Update()
{
	float time = GetRealTimeSinceStartup();
	m_time_delta = time - m_time;
	m_time = time;

	if (m_time_record < 0)
	{
		m_time_record = GetRealTimeSinceStartup();
		m_frame_record = GetFrameCount();
	}

	int frame = GetFrameCount();
	if (time - m_time_record >= 1)
	{
		m_fps = frame - m_frame_record;
		m_time_record = time;
		m_frame_record = frame;
	}

	if (m_frame_count < 0)
	{
		m_frame_count = 0;
	}
	else
	{
		m_frame_count++;
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
	SYSTEMTIME sys_time;
	::GetLocalTime(&sys_time);

	struct tm tm;
	tm.tm_sec = sys_time.wSecond;
	tm.tm_min = sys_time.wMinute;
	tm.tm_hour = sys_time.wHour;
	tm.tm_mday = sys_time.wDay;
	tm.tm_mon = sys_time.wMonth - 1;
	tm.tm_year = sys_time.wYear - 1900;
	tm.tm_isdst = -1;

	t = mktime(&tm) * (int64)1000 + sys_time.wMilliseconds;
#elif defined(DC_PLATFORM_LINUX) || defined(DC_PLATFORM_ANDROID) || defined(DC_PLATFORM_MAC) || defined(DC_PLATFORM_IOS)
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
	int64 time = m_clock_time.micro();
	return (float)time / 1000000.0f;
}
DC_END_NAMESPACE
