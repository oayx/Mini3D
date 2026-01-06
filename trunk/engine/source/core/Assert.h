
/*****************************************************************************************************/
// @author hannibal
// @date   2015/05/28
// @brief  
/*****************************************************************************************************/
#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "Debuger.h"
 
DC_BEGIN_NAMESPACE
inline void __show__(const char* szTemp)
{
	//保存日志
	//FILE* f = fopen("./assert.log", "a");
	//fwrite(szTemp, 1, strlen(szTemp), f);
	//fwrite("\r\n", 1, 2, f);
	//fclose(f);

	Debuger::Log(szTemp);

#if defined(DC_PLATFORM_WIN32)
	Debuger::Message("Error", szTemp);
#endif
	exit(1);
}
inline void __assert__(const char *format, ...)
{
	char szErrorBuffer[4096];

	va_list args;
	va_start(args, format);
	vsnprintf(szErrorBuffer, sizeof(szErrorBuffer), format, args);
	va_end(args);

	__show__(szErrorBuffer);
}

#define __ERROR_ABORT__(expr, fmt, ...)\
    {\
        if (!(expr)){\
            __assert__("%s(%d): %s: " fmt, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);\
        }\
    }


//断言
#if defined(DC_DEBUG)
#define MyAssert(expr)				__ERROR_ABORT__(expr, "")
#define AssertEx(expr,fmt, ...)		__ERROR_ABORT__(expr, fmt, ##__VA_ARGS__)
#else
#define MyAssert(expr)				UNUSED(false)
#define AssertEx(expr,fmt, ...)		UNUSED(false)
#endif



//指针校验
#define CHECK_RETURN_PTR_VOID(ptr)		do{MyAssert(ptr); if(!(ptr)){return;}}while(0);
#define CHECK_RETURN_PTR_NULL(ptr)		do{MyAssert(ptr); if(!(ptr)){return nullptr;}}while(0);
#define CHECK_RETURN_PTR_TRUE(ptr)		do{MyAssert(ptr); if(!(ptr)){return true;}}while(0);
#define CHECK_RETURN_PTR_FALSE(ptr)		do{MyAssert(ptr); if(!(ptr)){return false;}}while(0);
#define CHECK_RETURN_PTR_NUMBER(ptr)	do{MyAssert(ptr); if(!(ptr)){return 0;}}while(0);
#define CHECK_RETURN_PTR_STRING(ptr)	do{MyAssert(ptr); }while(0);

#define CHECK_BREAK_PTR(ptr)			{MyAssert(ptr); if(!(ptr)){break;}}
#define CHECK_CONTINUE_PTR(ptr)			{MyAssert(ptr); if(!(ptr)){continue;}}

DC_END_NAMESPACE
