 
/*****************************************************************************************************/
// @author hannibal
// @date   2020/07/30
// @brief  时间统计
/*****************************************************************************************************/
#pragma once

#include "ClockTime.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
//时间定义
const unsigned int TIME_1Min	= 60;			//1分钟
const unsigned int TIME_2Min	= 120;			//2分钟
const unsigned int TIME_5Min	= 300;			//5分钟
const unsigned int TIME_10Min	= 600;			//10分钟
const unsigned int TIME_1Hour	= 3600;			//1小时
const unsigned int TIME_24Hour	= 86400;		//24小时
const unsigned int TIME_48Hour	= 172800;		//48小时保护时间
const unsigned int TIME_3Day	= 259200;       //3天
const unsigned int TIME_7Day	= 604800;       //7天

/********************************************************************/
//日期格式
struct DateTime
{
	int year;
	int month;			// Month. [0-11]
	int day;			// Day. [1-31]
	int hour;			// Hours. [0-23]
	int minute;			// Minutes. [0-59]
	int second;			// Seconds. [0-60] (1 leap second)
	int milli_second;
	int week_day;		// Day of week. [0-6]

	//格式:yyyy-mm-dd hh:mm:ss
	String ToString()
	{
		char ch[128] = {0};
		Sprintf(ch, "%d-%d-%d %d:%d:%d", year, month, day, hour, minute, second);
		return String(ch);
	}
	//格式:yyyy-mm-dd
	String ToShortDateString()
	{
		char ch[128] = { 0 };
		Sprintf(ch, "%d-%d-%d", year, month, day);
		return String(ch);
	}
};

/********************************************************************/
class ENGINE_DLL Time Final : public Object
{
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(Time);
	BEGIN_DERIVED_REFECTION_TYPE(Time, Object)
	END_DERIVED_REFECTION_TYPE;

public:
	static void Initialize();
	static void Update();

	//本地日期
	static DateTime GetDate();
	//1970到现在经过的毫秒(本地时间)
	static int64 GetTimeMS();
	//1970到现在经过的毫秒(UTC时间)
	static int64 GetUTCTimeMS();

	//启动后经过时间，相同帧调用值一样
	static float GetTime() { return m_time; }
	static float GetRealTimeSinceStartup();
	static float GetDeltaTime() { return m_time_delta * m_time_scale; }
	static float GetUnscaledDeltaTime() { return m_time_delta; }
	

	static void SetTimeScale(float scale) { m_time_scale = scale; }
	static float GetTimeScale() { return m_time_scale; }

	static int GetFPS() { return m_fps; }
	static int GetFrameCount() { return m_frame_count; }

private:
	static ClockTime m_clock_time;
	static float m_time_delta;
	static float m_time_record;
	static float m_time;
	//1：“timeScale不会影响Update和LateUpdate的执行速度”
	//2：“FixedUpdate是根据时间来的，所以timeScale只会影响FixedUpdate的速度”
	static float m_time_scale;
	static int m_frame_count;
	static int m_frame_record;
	static int m_fps;
};
DC_END_NAMESPACE
