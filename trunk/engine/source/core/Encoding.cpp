#include "Encoding.h"
#include "platform/PlatformDefine.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_REFECTION_TYPE(Encoding);
int Encoding::Convert(EncodingType from, EncodingType to, const char *src, int srclen, char* save, int savelen)
{
	return Convert(GetEncodingType(from), GetEncodingType(to), src, srclen, save, savelen);
}
String Encoding::Convert(EncodingType from, EncodingType to, const char *src, int srclen)
{
	String ret = "";
	int len = srclen * 4 + 1;
	char* data = Memory::NewArray<char>(len);
	bool result = Convert(GetEncodingType(from), GetEncodingType(to), src, srclen, data, len);
	if (result)ret = String(data);
	Memory::DeleteArray(data);
	return ret;
}
String Encoding::Convert(EncodingType from, EncodingType to, const String& src)
{
	String ret = "";
	int len = src.Size() * 4 + 1;
	char* data = Memory::NewArray<char>(len);
	bool result = Convert(GetEncodingType(from), GetEncodingType(to), src.c_str(), src.Size(), data, len);
	if (result)ret = String(data);
	Memory::DeleteArray(data);
	return ret;
}
String Encoding::Convert(const char* from_charset, const char* to_charset, const String& src)
{
	String ret = "";
	int len = src.Size() * 4 + 1;
	char* data = Memory::NewArray<char>(len);
	bool result = Convert((char*)(from_charset), (char*)to_charset, src.c_str(), src.Size(), data, len);
	if (result)ret = String(data);
	Memory::DeleteArray(data);
	return ret;
}
int Encoding::GBKToUtf8(const char *src, int srclen, char* save, int savelen)
{
	return Convert((char*)"gbk", (char*)"utf-8", src, srclen, save, savelen);
}
String Encoding::GBKToUtf8(const char *src, int srclen)
{
	String ret = "";
	int len = srclen * 4 + 1;
	char* data = Memory::NewArray<char>(len);
	bool result = Convert((char*)"gbk", (char*)"utf-8", src, srclen, data, len);
	if (result)ret = String(data);
	Memory::DeleteArray(data);
	return ret;
}
String Encoding::GBKToUtf8(const String& src)
{
	String ret = "";
	int len = src.Size() * 4 + 1;
	char* data = Memory::NewArray<char>(len);
	bool result = Convert((char*)"gbk", (char*)"utf-8", src.c_str(), src.Size(), data, len);
	if (result)ret = String(data);
	Memory::DeleteArray(data);
	return ret;
}
int Encoding::Utf8ToGBK(const char *src, int srclen, char* save, int savelen)
{
	return Convert((char*)"utf-8", (char*)"gbk", src, srclen, save, savelen);
}
String Encoding::Utf8ToGBK(const char *src, int srclen)
{
	String ret = "";
	int len = srclen * 4 + 1;
	char* data = Memory::NewArray<char>(len);
	bool result = Convert((char*)"utf-8", (char*)"gbk", src, srclen, data, len);
	if (result)ret = String(data);
	Memory::DeleteArray(data);
	return ret;
}
String Encoding::Utf8ToGBK(const String& src)
{
	String ret = "";
	int len = src.Size() * 4 + 1;
	char* data = Memory::NewArray<char>(len);
	bool result = Convert((char*)"utf-8", (char*)"gbk", src.data(), src.Size(), data, len);
	if (result)ret = data;
	Memory::DeleteArray(data);
	return ret;
}
bool Encoding::Convert(const char *from_charset, const char *to_charset, const char *inbuf, size_t inlen, char *outbuf, size_t outlen)
{
	char **pin = (char **)&inbuf;
	char **pout = &outbuf;

	iconv_t cd = iconv_open(to_charset, from_charset);
	if (cd == (iconv_t)-1) return false;
	memset(outbuf, 0, outlen);
	size_t ret = iconv(cd, pin, &inlen, pout, &outlen);//成功返回0
	if (ret == (size_t)-1) return false;
	iconv_close(cd);
	return ret == 0;
}
char* Encoding::GetEncodingType(EncodingType type)
{
	switch (type)
	{
	case EncodingType::Unicode:return (char*)"utf-8";
	case EncodingType::UTF8:return (char*)"utf-8";
	case EncodingType::UTF32:return (char*)"utf-32";
	case EncodingType::GBK:return (char*)"gbk";
	case EncodingType::BIG5:return (char*)"big5";
	case EncodingType::Default:
	default: return (char*)"utf-8";
	}
}
#if defined(DC_PLATFORM_WIN32)
std::string Encoding::WCharToUtf8(const wchar_t* src, int len)
{
	if (len < 0)len = ::wcslen(src);
	std::string str;
	int size = ::WideCharToMultiByte(CP_UTF8, 0, src, len, NULL, 0, NULL, NULL);
	if (size > 0)
	{
		str.resize(size);
		::WideCharToMultiByte(CP_UTF8, 0, src, len, &str[0], size, NULL, NULL);
	}
	return str;
}
std::string Encoding::WCharToUtf8(const std::wstring& src)
{
	return WCharToUtf8(src.c_str(), (int)src.size());
}
void Encoding::WCharToUtf8(const wchar_t* in_buffer, char* out_buffer, int len)
{
	if (in_buffer[0] == L'\0')
	{
		out_buffer[0] = '\0';
		return;
	}

	int res = ::WideCharToMultiByte(CP_UTF8, 0, in_buffer, -1, out_buffer, len, NULL, NULL);
	if (res == 0)
		out_buffer[0] = 0;
	else
		out_buffer[len - 1] = 0;
}
std::wstring Encoding::Utf8ToWChar(const char* src, int len)
{
	if (len < 0)len = ::strlen(src);
	std::wstring wstr;
	int size = ::MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)src, len, 0, 0);
	if (size > 0)
	{
		wstr.resize(size);//如果用size+1，会导致字符串以'\0'结尾，这样在后续就不能连接字符串了
		::MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)src, len, &wstr.front(), size);
	}
	return wstr;
}
std::wstring Encoding::Utf8ToWChar(const std::string& s)
{
	return Utf8ToWChar(s.c_str(), (int)s.size());
}
void Encoding::Utf8ToWChar(const char* in_buffer, wchar_t* out_buffer, int len)
{
	if (in_buffer[0] == '\0')
	{
		out_buffer[0] = L'\0';
		return;
	}

	int res = ::MultiByteToWideChar(CP_UTF8, 0, in_buffer, -1, out_buffer, len);
	if (res == 0)
		out_buffer[0] = 0;
	else
		out_buffer[len - 1] = 0;
}
#else
std::string Encoding::WCharToUtf8(const wchar_t* src, int len)
{
	if (len < 0)len = ::wcslen(src);
	std::string str;
	str.resize(len * 4 + 1);
	bool result = Convert("wchar_t", "utf-8", (const char*)src, len, &str[0], str.length());
	return str;
}
std::string Encoding::WCharToUtf8(const std::wstring& src)
{
	return WCharToUtf8(src.c_str(), (int)src.size());
}
void Encoding::WCharToUtf8(const wchar_t* in_buffer, char* out_buffer, int len)
{
	if (in_buffer[0] == L'\0')
	{
		out_buffer[0] = '\0';
		return;
	}

	bool result = Convert("wchar_t", "utf-8", (const char*)in_buffer, len, out_buffer, len);
}
std::wstring Encoding::Utf8ToWChar(const char* src, int len)
{
	if (len < 0)len = ::strlen(src);
	std::wstring wstr;
	wstr.resize(len * 4 + 1);
	bool result = Convert("utf-8", "wchar_t", (const char*)src, len, (char*)&wstr[0], wstr.length());
	return wstr;
}
std::wstring Encoding::Utf8ToWChar(const std::string& s)
{
	return Utf8ToWChar(s.c_str(), (int)s.size());
}
void Encoding::Utf8ToWChar(const char* in_buffer, wchar_t* out_buffer, int len)
{
	if (in_buffer[0] == '\0')
	{
		out_buffer[0] = L'\0';
		return;
	}

	bool result = Convert("utf-8", "wchar_t", (const char*)in_buffer, len, (char*)out_buffer, len);
}
#endif
DC_END_NAMESPACE
