#include "String.h"
#include <stdarg.h>
#include <regex>
#include <locale>
#include "Encoding.h"
#include "core/math/Math.h"
#include "external/crypto/md5/md5.h"
#include "external/crypto/xxtea/xxtea.h"
#include "external/crypto/xxhash/xxhash.h"
#include "external/tinyexpr/tinyexpr.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_REFECTION_TYPE(String)
String String::Empty = "";
static Vector<char> Unicode32ToUtf8(char32_t c32) noexcept
{
	Vector<char> buffer;
	int byteCount = 0;

	if (c32 <= 0x7f)
	{
		byteCount = 1;
	}
	else if (c32 <= 0x7ff)
	{
		byteCount = 2;
	}
	else if (c32 <= 0xffff)
	{
		byteCount = 3;
	}
	else if (c32 <= 0x1fffff)
	{
		byteCount = 4;
	}
	else if (c32 <= 0x3ffffff)
	{
		byteCount = 5;
	}
	else if (c32 <= 0x7fffffff)
	{
		byteCount = 6;
	}

	Vector<char> bytes;
	bytes.Reserve(byteCount);
	for (int i = 0; i < byteCount - 1; ++i)
	{
		bytes.Add((c32 & 0x3f) | 0x80);
		c32 >>= 6;
	}

	if (byteCount > 1)
	{
		bytes.Add((char)(c32 | (0xffffff80 >> (byteCount - 1))));
	}
	else
	{
		bytes.Add((char)(c32));
	}

	for (int i = 0; i < byteCount; ++i)
	{
		buffer.Add(bytes[byteCount - 1 - i]);
	}

	return buffer;
}
String::String(const char32_t* unicode32)
{
	Vector<char> str;
	for (int i = 0; unicode32[i] != 0; ++i)
	{
		char32_t c32 = unicode32[i];
		auto bytes = Unicode32ToUtf8(c32);
		for (const auto& b : bytes)
			str.Add(b);
	}
	str.Add(0);
	_string = &str[0];
}
String::String(const char32_t* unicode32, int size)
{
	Vector<char> str;
	str.Reserve(size * 4);
	for (int i = 0; i < size; ++i)
	{
		char32_t c32 = unicode32[i];
		Vector<char> bytes = Unicode32ToUtf8(c32);
		for(int j = 0; j < bytes.Size(); ++j)
			str.Add(bytes[j]);
	}
	str.Add(0);
	_string = &str[0];
}
int String::IndexOf(const String& str, int start, bool ignoreCase) const noexcept
{
	if (ignoreCase)
	{
		std::string str1 = ToLower();
		std::string str2 = str.ToLower();
		size_t pos = str1.find(str2, start);
		if (pos != std::string::npos)
			return (int)pos;
		else
			return -1;
	}
	else
	{
		size_t pos = _string.find(str._string, start);
		if (pos != std::string::npos)
			return (int)pos;
		else
			return -1;
	}
}
int String::LastIndexOf(const String& str, int start, bool ignoreCase) const noexcept
{
	if (ignoreCase)
	{
		std::string str1 = ToLower();
		std::string str2 = str.ToLower();
		size_t pos = str1.rfind(str2, start);
		if (pos != std::string::npos)
			return (int)pos;
		else
			return -1;
	}
	else
	{
		size_t pos = _string.rfind(str._string, start);
		if (pos != std::string::npos)
			return (int)pos;
		else
			return -1;
	}
}
int String::IndexOf(char ch, int start, bool ignoreCase) const noexcept
{
	if (ignoreCase)
	{
		std::string str = ToLower();
		size_t pos = str.find(std::tolower(ch, std::locale()), start);
		if (pos != std::string::npos)
			return (int)pos;
		else
			return -1;
	}
	else
	{
		size_t pos = _string.find(ch, start);
		if (pos != std::string::npos)
			return (int)pos;
		else
			return -1;
	}
}
int String::LastIndexOf(char ch, int start, bool ignoreCase) const noexcept
{
	if (ignoreCase)
	{
		std::string str = ToLower();
		size_t pos = str.rfind(std::tolower(ch, std::locale()), start);
		if (pos != std::string::npos)
			return (int)pos;
		else
			return -1;
	}
	else
	{
		size_t pos = _string.rfind(ch, start);
		if (pos != std::string::npos)
			return (int)pos;
		else
			return -1;
	}
}
bool String::StartsWith(const String& str, bool ignoreCase) const noexcept
{
	if (str.Size() == 0)
	{
		return true;
	}
	else if (this->Size() < str.Size())
	{
		return false;
	}
	else
	{
		if(ignoreCase)
			return Strnicmp(&(*this)[0], &str[0], str.Size()) == 0;
		else
			return Strncmp(&(*this)[0], &str[0], str.Size()) == 0;
	}
}

bool String::EndsWith(const String& str, bool ignoreCase) const noexcept
{
	if (str.Size() == 0)
	{
		return true;
	}
	else if (this->Size() < str.Size())
	{
		return false;
	}
	else
	{
		if (ignoreCase)
			return Strnicmp(&(*this)[this->Size() - str.Size()], &str[0], str.Size()) == 0;
		else
			return Strncmp(&(*this)[this->Size() - str.Size()], &str[0], str.Size()) == 0;
	}
}

String String::Substring(int start, int count) const noexcept
{
	start = Math::Clamp(start, 0, (int)_string.size());
	String result;
	result._string = _string.substr(start, count);
	return result;
}
String String::Remove(int start) noexcept
{
	return _string.erase(start);
}
String String::Remove(int start, int count) noexcept
{
	return _string.erase(start, count);
}
String String::Insert(int pos, const String& str) noexcept
{
	return _string.insert(pos, str.c_str());
}
String String::Insert(int pos, const String& str, int count) noexcept
{
	return _string.insert(pos, str.c_str(), count);
}
String String::ToLower() const noexcept
{
	std::string str = _string;
	std::transform(str.begin(), str.end(), str.begin(),
		[](char c) { return std::tolower(c, std::locale()); });
	return str;
}

String String::ToUpper() const noexcept
{
	std::string str = _string;
	std::transform(str.begin(), str.end(), str.begin(),
		[](char c) { return std::toupper(c, std::locale()); });
	return str;
}
String String::Trim(const String& src) noexcept
{
	if (src.IsEmpty())
		return src;
	
	char ch = ' ';
	int startIndex = src.FirstNotOf(ch);
	int endIndex = src.LastNotOf(ch);
	return src.Substring(startIndex, endIndex + 1);
}
String String::TrimLeft(const String& src) noexcept
{
	if (src.IsEmpty())
		return src;

	char ch = ' ';
	int startIndex = src.FirstNotOf(ch);
	return src.Substring(startIndex, src.Size());
}
String String::TrimRight(const String& src) noexcept
{
	if (src.IsEmpty())
		return src;
	
	char ch = ' ';
	int endIndex = src.LastNotOf(ch);
	return src.Substring(0, endIndex + 1);
}
String String::Replace(const char old, const char to) const noexcept
{
	String result(*this);
	std::replace(result._string.begin(), result._string.end(), old, to);
	return result;
}
String String::Replace(const String& old, const String& to) const noexcept
{
	String result(*this);

	int start = 0;
	while (true)
	{
		int index = result.IndexOf(old, start);
		if (index >= 0)
		{
			result._string.replace(index, old._string.size(), to._string);
			start = index + (int)to._string.size();
		}
		else
		{
			break;
		}
	}
	
	return result;
}
bool String::Equals(const String& str, bool ignoreCase)const noexcept
{
	if (ignoreCase)
		return Stricmp(_string.c_str(), str.c_str()) == 0 ? true : false;
	else
		return *this == str;
}
bool String::IsANSI() const noexcept
{
	AssertEx(false, "");
	return false;
}
Vector<String> String::Split(const String& split) const noexcept
{
	Vector<String> tokens;
	if (_string.empty())return tokens;

	std::string::size_type lastPos = _string.find_first_not_of(split._string, 0);
	std::string::size_type pos = _string.find_first_of(split._string, lastPos);
	while (std::string::npos != pos || std::string::npos != lastPos) 
	{
		tokens.Add(_string.substr(lastPos, pos - lastPos));
		lastPos = _string.find_first_not_of(split._string, pos);
		pos = _string.find_first_of(split._string, lastPos);
	}
	return tokens;
}
//部分匹配表
static void cal_kmp_next(const String& str, Vector<int>& next) noexcept
{
	const int len = str.Size();
	next[0] = -1;
	int k = -1;
	int j = 0;
	while (j < len - 1)
	{
		if (k == -1 || str[j] == str[k])
		{
			++k;
			++j;
			next[j] = k;//表示第j个字符有k个匹配（“最大长度值” 整体向右移动一位，然后初始值赋为-1）
		}
		else
		{
			k = next[k];//往前回溯
		}
	}
}
Vector<int> String::KMP(const String& child) noexcept
{
	Vector<int> vec;
	Vector<int> next;
	next.Resize(_string.size(), 0);
	cal_kmp_next(child, next);
	int i = 0;//i是str1的下标
	int j = 0;//j是str2的下标
	int str1Size = _string.size();
	int str2Size = child.Size();
	while (i < str1Size && j < str2Size)
	{
		//如果j = -1，或者当前字符匹配成功（即S[i] == P[j]），
		//都令i++，j++. 注意：这里判断顺序不能调换！
		if (j == -1 || _string[i] == child[j])
		{
			++i;
			++j;
		}
		else
		{
			j = next[j];//当前字符匹配失败，直接从str[j]开始比较，i的位置不变
		}
		if (j == str2Size)//匹配成功
		{
			vec.Add(i - j);//记录下完全匹配最开始的位置
			j = -1;//重置
		}
	}
	return vec;
}
static const char BASE64_TABLE[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
String String::Base64(const char* bytes, int size) noexcept
{
	int sizePad = size;
	if (sizePad % 3 != 0)
	{
		sizePad += 3 - (sizePad % 3);
	}
	int round = sizePad / 3;

	std::string str(round * 4, '\0');

	int index;
	char a, b, c;
	for (int i = 0; i < round; ++i)
	{
		a = 0; b = 0; c = 0;

		index = i * 3 + 0;
		if (index < size) a = bytes[index];
		index = i * 3 + 1;
		if (index < size) b = bytes[index];
		index = i * 3 + 2;
		if (index < size) c = bytes[index];

		str[i * 4 + 0] = BASE64_TABLE[(a & 0xfc) >> 2];
		str[i * 4 + 1] = BASE64_TABLE[((a & 0x3) << 4) | ((b & 0xf0) >> 4)];
		str[i * 4 + 2] = BASE64_TABLE[((b & 0xf) << 2) | ((c & 0xc0) >> 6)];
		str[i * 4 + 3] = BASE64_TABLE[c & 0x3f];
	}

	for (int i = sizePad - size, j = 0; i > 0; --i, ++j)
	{
		str[(round - 1) * 4 + 3 - j] = '=';
	}

	return String(str.c_str());
}

String String::UrlDecode(const String& str) noexcept
{
	std::string dest = str.c_str();

	int i = 0;
	int j = 0;
	char c;
	int size = (int)str.Size();
	while (i < size)
	{
		c = str[i];
		switch (c)
		{
		case '+':
			dest[j++] = ' ';
			i++;
			break;
		case '%':
		{
			while (i + 2 < size && c == '%')
			{
				auto sub = str.Substring(i + 1, 2);
				char v = (char)strtol(sub.c_str(), nullptr, 16);
				dest[j++] = v;
				i += 3;
				if (i < size)
				{
					c = str[i];
				}
			}
		}
		break;
		default:
			dest[j++] = c;
			i++;
			break;
		}
	}

	return String(dest.c_str(), j);
}
String String::Md5(const String& src) noexcept
{
	return String::Md5(src.data(), src.Size());
}
String String::Md5(const char* src, int size) noexcept
{
	MD5_CTX ctx;

	unsigned char md[16] = { 0 };
	char tmp[33] = { 0 };

	MD5_Init(&ctx);
	MD5_Update(&ctx, src, size);
	MD5_Final(md, &ctx);

	std::string md5_string;
	for (int i = 0; i < 16; ++i)
	{
		memset(tmp, 0x00, sizeof(tmp));
		sprintf(tmp, "%02X", md[i]);
		md5_string += tmp;
	}
	return String(md5_string);
}
String String::TeaEncrypt(const String& src) noexcept
{
	return String::TeaEncrypt((unsigned char*)src.data(), src.Size());
}
String String::TeaDecrypt(const String& src) noexcept
{
	return String::TeaDecrypt((unsigned char*)src.data(), src.Size());
}
String String::TeaEncrypt(unsigned char* src, int size) noexcept
{
	xxtea_long outLength = 0;
	unsigned char* ret = xxtea_encrypt(src, size, nullptr, 0, &outLength);
	if (ret == nullptr)return "";
	String str((const char*)ret, outLength);
	delete[](ret);
	return str;
}
String String::TeaDecrypt(unsigned char* src, int size) noexcept
{
	xxtea_long outLength = 0;
	unsigned char* ret = xxtea_decrypt(src, size, nullptr, 0, &outLength);
	if (ret == nullptr)return "";
	String str((const char*)ret, outLength);
	delete[](ret);
	return str;
}
uint String::Hash(const String& src) noexcept
{
	return String::Hash(src.data(), src.Size());
}
uint String::Hash(const char* src, int size) noexcept
{
	void* state  = XXH32_init(0);
	XXH_errorcode err = XXH32_update(state, src, size);
	if (err != 0)return 0;
	uint ret = XXH32_digest(state);
	return ret;
}
static int Utf8ToUnicode32(const char* utf8, char32_t& c32) noexcept
{
	int byteCount = 0;

	for (int i = 0; i < 8; ++i)
	{
		unsigned char c = utf8[0];

		if (((c << i) & 0x80) == 0)
		{
			if (i == 0)
			{
				byteCount = 1;
			}
			else
			{
				byteCount = i;
			}
			break;
		}
	}

	if (byteCount >= 1 && byteCount <= 6)
	{
		char32_t code = 0;

		for (int i = 0; i < byteCount; ++i)
		{
			unsigned int c = utf8[i];
			unsigned char part;

			if (i == 0)
			{
				part = (c << (byteCount + 24)) >> (byteCount + 24);
			}
			else
			{
				part = c & 0x3f;
			}

			code = (code << 6) | part;
		}

		c32 = code;

		return byteCount;
	}
	else
	{
		return 0;
	}
}
Vector<char32_t> String::ToUnicode32() const noexcept
{
	Vector<char32_t> unicode;
	int size = (int)_string.size();
	for (int i = 0; i < size; ++i)
	{
		char32_t unicode32 = 0;
		int byteCount = Utf8ToUnicode32(&_string[i], unicode32);
		if (byteCount > 0)
		{
			unicode.Add(unicode32);
			i += byteCount - 1;
		}
		else
		{
			break;
		}
	}

	return unicode;
}
String String::FromWString(const wchar_t* pw, int len) noexcept
{
	return Encoding::WCharToUtf8(pw, len);
}
String String::FromWString(const std::wstring& w_str) noexcept
{
	return Encoding::WCharToUtf8(w_str);
}
std::wstring String::ToWString()const noexcept
{
	return Encoding::Utf8ToWChar(_string);
}
float String::Expression(const String& expr) noexcept
{
	if (expr.IsEmpty())return 0;
	float v = (float)te_interp(expr.c_str(), 0);
	return v;
}
bool String::Match(const String& match, const String& str) noexcept
{
	std::regex reg(match.c_str());
	bool result = std::regex_match(str.c_str(), reg);
	return result;
}
const char* String::Stristr(const char* haystack, const char* needle) noexcept
{
	const char* c = haystack;
	while (*c)
	{
		if (toupper(*c) == toupper(needle[0]))
		{
			const char* n = needle + 1;
			const char* c2 = c + 1;
			while (*n && *c2)
			{
				if (toupper(*n) != toupper(*c2)) break;
				++n;
				++c2;
			}
			if (*n == 0) return c;
		}
		++c;
	}
	return nullptr;
}
DC_END_NAMESPACE