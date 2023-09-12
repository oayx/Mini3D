
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
class ENGINE_DLL Debuger Final
{
	friend class Application;
	typedef Delegate<LogMsgType, const char*> LogDelegate;
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(Debuger);
	
	static void Initialize();
	static void Destroy();

public:
	//写日志接口
	static void Debug(const char* format, ...);
	static void Log(const char* format, ...);
	static void Warning(const char* format, ...);
	static void Error(const char* format, ...);
	static void Exception(const char* format, ...);
	static void Output(const char* format, ...);
	static void Message(const char* title, const char* format, ...);

public:
	static void SetLogToFileEnable(bool b);
	static void RegisterLogMessageReceived(Function<void(LogMsgType, const char*)>& action) { _logHandle.Bind(action); }
	static void UnregisterLogMessageReceived(Function<void(LogMsgType, const char*)>& action) { _logHandle.Unbind(action); }

private:
	static void WriteLog(LogMsgType level, const char* msg);
	static void WriteLogToConsole(LogMsgType level, const char* msg);
	static void WriteLogToFile(LogMsgType level, const char* msg);

private:
	static const int BufferSize = 1024 * 8;
	static LogDelegate _logHandle;
	static FileDataStream* _fileStream;
};//Log
DC_END_NAMESPACE