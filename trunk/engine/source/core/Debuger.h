
/*****************************************************************************
* Author： hannibal
* Date：2009/11/21
* Description： 日志输出(控制台、vs编辑器、文件)
*****************************************************************************/
#pragma once
 
#include "event/Delegate.h"


DC_BEGIN_NAMESPACE
class FileDataStream;
/********************************************************************/
enum class LogMsgType
{
	Debug = 0,		//调试信息	
	Log = 1,		//普通信息	
	Warning = 2,	//警告信息
	Error = 3,		//错误信息
	Exception = 4,	//异常

	Max,
};
DECLARE_ENUM_OPERATORS(LogMsgType);

/********************************************************************/
class ENGINE_DLL Debuger final
{
	friend class Application;
	typedef Delegate<LogMsgType, const char*> LogDelegate;
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(Debuger);
	
	static void Initialize();
	static void Destroy();

public:
	//写日志接口
	static void Assert(bool expr) noexcept;
	static void Assert(bool expr, const char* format, ...) noexcept;
	static void Debug(const char* format, ...) noexcept;
	static void Log(const char* format, ...) noexcept;
	static void Warning(const char* format, ...) noexcept;
	static void Error(const char* format, ...) noexcept;
	static void Exception(const char* format, ...) noexcept;
	static void Output(const char* format, ...) noexcept;
	static void Message(const char* title, const char* format, ...) noexcept;

public:
	static void SetLogToFileEnable(bool b) noexcept;
	static void RegisterLogMessageReceived(Function<void(LogMsgType, const char*)>& action) noexcept { _logHandle.Bind(action); }
	static void UnregisterLogMessageReceived(Function<void(LogMsgType, const char*)>& action) noexcept { _logHandle.Unbind(action); }

private:
	static void WriteLog(LogMsgType level, const char* msg) noexcept;
	static void WriteLogToConsole(LogMsgType level, const char* msg) noexcept;
	static void WriteLogToFile(LogMsgType level, const char* msg) noexcept;

private:
	inline static constexpr int BufferSize = 1024 * 8;
	inline static LogDelegate _logHandle;
	inline static FileDataStream* _fileStream = nullptr;
};//Log
DC_END_NAMESPACE