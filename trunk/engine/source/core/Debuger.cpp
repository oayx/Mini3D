#include "Debuger.h"
#include "Encoding.h"
#include "file/File.h"
#include "file/Path.h"
#include "memory/Memory.h"
#include "time/Time.h"
#include "stream/DataStream.h"
#include "runtime/Application.h"
#include "platform/PlatformDefine.h"
#if defined(DC_PLATFORM_ANDROID)
#include "android/log.h"
#elif defined(DC_PLATFORM_WASM)
#include <emscripten/console.h>
#endif
 
DC_BEGIN_NAMESPACE
#if defined(DC_PLATFORM_WIN32)
	int TypeToColor[5] = { FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
		FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
		FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN,
		FOREGROUND_INTENSITY | FOREGROUND_RED,
		FOREGROUND_INTENSITY | FOREGROUND_RED };
#endif

const char* TypeToString[5] = { "Debug", "Log","Warning","Error", "Exception" };
/********************************************************************/
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
void Debuger::Assert(bool expr) noexcept
{
}
void Debuger::Assert(bool expr, const char* format, ...) noexcept
{
}
void Debuger::Debug(const char* format, ...) noexcept
{
#if DC_DEBUG
	char msg[BufferSize] = { 0 };
	va_list list;
	va_start(list, format);
	vsnprintf(msg, BufferSize, format, list);//vsnprintf最多向 buf 写入 size-1 个字符，然后在 buf[size-1] 处写 '\0'（即总共 size 字节，包括结尾 '\0'）
	va_end(list);
	WriteLog(LogMsgType::Debug, msg);
#endif
}
void Debuger::Log(const char* format, ...) noexcept
{
	char msg[BufferSize] = { 0 };
	va_list list;
	va_start(list, format);
	vsnprintf(msg, BufferSize, format, list);
	va_end(list);
	WriteLog(LogMsgType::Log, msg);
}
void Debuger::Warning(const char* format, ...) noexcept
{
	char msg[BufferSize] = { 0 };
	va_list list;
	va_start(list, format);
	vsnprintf(msg, BufferSize, format, list);
	va_end(list);
	WriteLog(LogMsgType::Warning, msg);
}
void Debuger::Error(const char* format, ...) noexcept
{
	char msg[BufferSize] = { 0 };
	va_list list;
	va_start(list, format);
	vsnprintf(msg, BufferSize, format, list);
	va_end(list);
	WriteLog(LogMsgType::Error, msg);
}
void Debuger::Exception(const char* format, ...) noexcept
{
	char msg[BufferSize] = { 0 };
	va_list list;
	va_start(list, format);
	vsnprintf(msg, BufferSize, format, list);
	va_end(list);
	WriteLog(LogMsgType::Exception, msg);
}
void Debuger::Output(const char* format, ...) noexcept
{
	char msg[BufferSize] = { 0 };
	va_list list;
	va_start(list, format);
	vsnprintf(msg, BufferSize, format, list);
	va_end(list);
#if defined(DC_PLATFORM_WIN32)
	::OutputDebugStringW(Encoding::Utf8ToWChar(msg).c_str());
#else
	//TODO:其他平台实现
#endif
}
void Debuger::Message(const char* title, const char* format, ...) noexcept
{
	char msg[BufferSize] = { 0 };
	va_list list;
	va_start(list, format);
	vsnprintf(msg, BufferSize, format, list);
	va_end(list);
#if defined(DC_PLATFORM_WIN32)
	::MessageBoxW(NULL, Encoding::Utf8ToWChar(msg).c_str(), Encoding::Utf8ToWChar(title).c_str(), MB_OK);
#else
	//TODO:其他平台实现
#endif
}
void Debuger::WriteLog(LogMsgType type, const char* msg) noexcept
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
void Debuger::WriteLogToConsole(LogMsgType type, const char* msg) noexcept
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
#elif defined(DC_PLATFORM_WASM)
	switch (type) {
	case LogMsgType::Debug:		emscripten_console_log(msg); break;		// emscripten_console_log->console.log
	case LogMsgType::Log:		emscripten_console_log(msg); break;		// emscripten_console_log->console.info
	case LogMsgType::Warning:	emscripten_console_warn(msg); break;	// emscripten_console_warn->console.warn
	case LogMsgType::Error:
	case LogMsgType::Exception: emscripten_console_error(msg); break;	// emscripten_console_error->console.error
	}
#elif defined(DC_PLATFORM_IOS)
	//NSString* ns_msg = [NSString stringWithFormat : @"[%s] %s (frame:%d)", sz_type, msg, Time::GetFrameCount()];
	//NSLog(@"%@", ns_msg);
	printf("[%s]:%s, frame:%d\n", sz_type, msg, Time::GetFrameCount());
#elif defined(DC_PLATFORM_MAC)
	// 定义ANSI颜色代码
	const char* color_code = "";
	switch (type) {
	case LogMsgType::Debug:     color_code = "\033[36m"; break; // Cyan
	case LogMsgType::Log:		color_code = "\033[37m"; break; // White
	case LogMsgType::Warning:   color_code = "\033[33m"; break; // Yellow
	case LogMsgType::Error:
	case LogMsgType::Exception: color_code = "\033[31m"; break; // Red
	}
	// 带颜色和类型前缀的终端输出
	printf("%s[%s]: %s, frame:%d\033[0m\n", color_code, sz_type, msg, Time::GetFrameCount());
#else
	printf("[%s]:%s, frame:%d\n", sz_type, msg, Time::GetFrameCount());
#endif
}
void Debuger::WriteLogToFile(LogMsgType type, const char* msg) noexcept
{
	if (_fileStream == nullptr)return;

	const char* sz_type = TypeToString[(int)type];
	char full_msg[BufferSize + 100] = {0};
#if defined(DC_PLATFORM_WIN32)
	const char* break_line = "\r\n";
#else
	const char* break_line = "\n";
#endif
	snprintf(full_msg, BufferSize + 100, "[%s]:%s,frame:%d%s", sz_type, msg, Time::GetFrameCount(), break_line);
	_fileStream->Write(full_msg, 0, strlen(full_msg));
	_fileStream->Flush();
}
void Debuger::SetLogToFileEnable(bool b) noexcept
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