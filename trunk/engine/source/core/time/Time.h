 
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
		Snprintf(ch, sizeof(ch), "%d-%d-%d %d:%d:%d", year, month, day, hour, minute, second);
		return String(ch);
	}
	//格式:yyyy-mm-dd
	String ToShortDateString()
	{
		char ch[128] = { 0 };
		Snprintf(ch, sizeof(ch), "%d-%d-%d", year, month, day);
		return String(ch);
	}
};

/********************************************************************/
class ENGINE_DLL Time final : public Object
{
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(Time);
	BEGIN_DERIVED_REFECTION_TYPE(Time, Object)
	END_REFECTION_TYPE;

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
	static float GetTime() { return _time; }
	static float GetRealTimeSinceStartup();
	static float GetDeltaTime() { return _timeDelta * _timeScale; }
	static float GetUnscaledDeltaTime() { return _timeDelta; }
	

	static void SetTimeScale(float scale) { _timeScale = scale; }
	static float GetTimeScale() { return _timeScale; }

	static int GetFPS() { return _fps; }
	static int GetFrameCount() { return _frameCount; }

private:
	inline static ClockTime _clockTime;
	inline static float _timeDelta = 0;
	inline static float _timeRecord = -1;
	inline static float _time = 0;
	//1：“timeScale不会影响Update和LateUpdate的执行速度”
	//2：“FixedUpdate是根据时间来的，所以timeScale只会影响FixedUpdate的速度”
	inline static float _timeScale = 1.0f;
	inline static int _frameCount = -1;
	inline static int _frameRecord = 0;
	inline static int _fps = 0;
};
DC_END_NAMESPACE
