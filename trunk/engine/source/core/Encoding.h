
/*****************************************************************************************************/
// @author hannibal
// @date   2015/04/21
// @brief  编码转换
/*****************************************************************************************************/
#pragma once

#if DC_PLATFORM_WIN32
#	define ICONV_CONST
#	include "external/iconv/iconv.h"
#else
#	include <iconv.h>
#endif
#include "Debuger.h"
#include "String.h"

DC_BEGIN_NAMESPACE
enum class EncodingType
{
	Default,
	Unicode,
	UTF8,
	UTF32,
	GBK,		//东亚字体，包括中文、韩文、日文
	BIG5,		//繁体中文
};
DECLARE_ENUM_OPERATORS(EncodingType);

/********************************************************************/
class ENGINE_DLL Encoding final : public object
{
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(Encoding);
	BEGIN_REFECTION_TYPE(Encoding)
	END_FINAL_REFECTION_TYPE;
	
public:
	//任意编码类型转换
	static int Convert(EncodingType from, EncodingType to, const char *src, int srclen, char* save, int savelen);
	static String Convert(EncodingType from, EncodingType to, const char *src, int srclen);
	static String Convert(EncodingType from, EncodingType to, const String& src);
	static String Convert(const char* from_charset, const char* to_charset, const String& src);

public:
	//返回0表示成功
	static int GBKToUtf8(const char *src, int srclen, char* save, int savelen);
	static String GBKToUtf8(const char *src, int srclen);
	static String GBKToUtf8(const String& src);
	//返回0表示成功
	static int Utf8ToGBK(const char *src, int srclen, char* save, int savelen);
	static String Utf8ToGBK(const char *src, int srclen);
	static String Utf8ToGBK(const String& src);

private:
	/*!
		对字符串进行语言编码转换
		param from		原始编码，比如GB2312, GBK, GB18030, UTF-8, UTF-16, BIG5等
		param to		转换的目的编码
		param save		转换后的数据保存到这个指针里，需要在外部分配内存
		param savelen	存储转换后数据的内存大小
		param src		原始需要转换的字符串
		param srclen	原始字符串长度
	*/
	static bool Convert(const char *from_charset, const char *to_charset, const char *inbuf, size_t inlen, char *outbuf, size_t outlen);
	static char* GetEncodingType(EncodingType type);

public:
	static std::string	WCharToUtf8(const wchar_t* src, int len = -1);
	static std::string	WCharToUtf8(const std::wstring& src);
	static void			WCharToUtf8(const wchar_t* in_buffer, char* out_buffer, int len);
	static std::wstring Utf8ToWChar(const char* src, int len = -1);
	static std::wstring Utf8ToWChar(const std::string& s);
	static void			Utf8ToWChar(const char* in_buffer, wchar_t* out_buffer, int len);
};//Encoding
DC_END_NAMESPACE 
