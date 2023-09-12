#include "Debuger.h"
#include "file/File.h"
#include "file/Path.h"
#include "memory/Memory.h"
#include "time/Time.h"
#include "stream/DataStream.h"
#include "runtime/Application.h"
#include "platform/PlatformDefine.h"
#if defined(DC_PLATFORM_ANDROID)
#include "android/log.h"
#endif
 
DC_BEGIN_NAMESPACE
#if defined(DC_PLATFORM_WIN32)
	int TypeToColor[5] = { FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
		FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
		FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN,
		FOREGROUND_INTENSITY | FOREGROUND_RED,
		FOREGROUND_INTENSITY | FOREGROUND_RED };
#endif

char* TypeToString[5] = { "Debug", "Log","Warning","Error", "Exception" };
/********************************************************************/
Debuger::LogDelegate Debuger::_logHandle;
FileDataStream* Debuger::_fileStream = nullptr;
void Debuger::Initialize()
{
	SetLogToFileEnable(true);
}
void Debuger::Destroy()
{
	if (_fileStream != nullptr)
	{
		_fileStream->Flush();
		SAFE_DELETE(_fileStream);
	}
}
void Debuger::Debug(const char* format, ...)
{
#if DC_DEBUG
	char msg[BufferSize] = { 0 };
	va_list list;
	va_start(list, format);
	vsnprintf(msg, BufferSize, format, list);//vsnprintf会自动在写入字符的后面加上停止符\0，实际有效的是BufferSize-1，返回值是源字符串的大小(巨坑)
	va_end(list);
	WriteLog(LogMsgType::Debug, msg);
#endif
}
void Debuger::Log(const char* format, ...)
{
	char msg[BufferSize] = { 0 };
	va_list list;
	va_start(list, format);
	vsnprintf(msg, BufferSize, format, list);
	va_end(list);
	WriteLog(LogMsgType::Log, msg);
}
void Debuger::Warning(const char* format, ...)
{
	char msg[BufferSize] = { 0 };
	va_list list;
	va_start(list, format);
	vsnprintf(msg, BufferSize, format, list);
	va_end(list);
	WriteLog(LogMsgType::Warning, msg);
}
void Debuger::Error(const char* format, ...)
{
	char msg[BufferSize] = { 0 };
	va_list list;
	va_start(list, format);
	vsnprintf(msg, BufferSize, format, list);
	va_end(list);
	WriteLog(LogMsgType::Error, msg);
}
void Debuger::Exception(const char* format, ...)
{
	char msg[BufferSize] = { 0 };
	va_list list;
	va_start(list, format);
	vsnprintf(msg, BufferSize, format, list);
	va_end(list);
	WriteLog(LogMsgType::Exception, msg);
}
void Debuger::Output(const char* format, ...)
{
	char msg[BufferSize] = { 0 };
	va_list list;
	va_start(list, format);
	vsnprintf(msg, BufferSize - 1, format, list);//预留\n位置
	va_end(list);
#if defined(DC_PLATFORM_WIN32)
	msg[strlen(msg)] = '\n';
	::OutputDebugStringA(msg);
#else
	WriteLog(LogMsgType::Log, msg);
#endif
}
void Debuger::Message(const char* title, const char* format, ...)
{
	char msg[BufferSize] = { 0 };
	va_list list;
	va_start(list, format);
	vsnprintf(msg, BufferSize, format, list);
	va_end(list);
#if defined(DC_PLATFORM_WIN32)
	::MessageBoxA(NULL, msg, title, MB_OK);
#else
	WriteLog(LogMsgType::Log, msg);
#endif
}
void Debuger::WriteLog(LogMsgType type, const char* msg)
{
	static bool is_log_handle = false;
	if (is_log_handle)return;

	WriteLogToConsole(type, msg);
	WriteLogToFile(type, msg);

	{
		is_log_handle = true;
		_logHandle(type, msg);
		is_log_handle = false;
	}
}
void Debuger::WriteLogToConsole(LogMsgType type, const char* msg)
{
	const char* sz_type = TypeToString[(int)type];
#if defined(DC_PLATFORM_WIN32)
	::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE), TypeToColor[int(type)]);
	printf("[%s]:%s, frame:%d\n", sz_type, msg, Time::GetFrameCount());
#elif defined(DC_PLATFORM_LINUX)
	printf("[%s]:%s, frame:%d\n", sz_type, msg, Time::GetFrameCount());
#elif defined(DC_PLATFORM_ANDROID)
	int log_type = ANDROID_LOG_DEBUG;
	switch (type)
	{
	case LogMsgType::Debug:		log_type = ANDROID_LOG_DEBUG;break;
	case LogMsgType::Log:		log_type = ANDROID_LOG_INFO; break;
	case LogMsgType::Warning:	log_type = ANDROID_LOG_WARN; break;
	case LogMsgType::Error:		log_type = ANDROID_LOG_ERROR; break;
	case LogMsgType::Exception: log_type = ANDROID_LOG_ERROR; break;
	}
	__android_log_print(log_type, "Engine", "%s\n", msg);
#endif
}
void Debuger::WriteLogToFile(LogMsgType type, const char* msg)
{
	if (_fileStream == nullptr)return;

	const char* sz_type = TypeToString[(int)type];
	char full_msg[BufferSize + 100] = {0};
#if defined(DC_PLATFORM_WIN32)
	char* break_line = "\r\n";
#elif defined(DC_PLATFORM_LINUX) || defined(DC_PLATFORM_ANDROID) || defined(DC_PLATFORM_MAC) || defined(DC_PLATFORM_IOS)
	const char* break_line = "\n";
#endif
	snprintf(full_msg, BufferSize + 100, "[%s]:%s,frame:%d%s", sz_type, msg, Time::GetFrameCount(), break_line);
	_fileStream->Write(full_msg, 0, strlen(full_msg));
	_fileStream->Flush();
}
void Debuger::SetLogToFileEnable(bool b)
{
	if (b)
	{
		if (_fileStream == nullptr)
		{
			String output_file = Path::Combine(Application::GetPersistentDataPath(), "output.txt");
			_fileStream = FileDataStream::Create(output_file, "w");
		}
	}
	else
	{
		if (_fileStream != nullptr)
		{
			_fileStream->Flush();
			SAFE_DELETE(_fileStream);
		}
	}
}
DC_END_NAMESPACE