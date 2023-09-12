#include "String.h"
#include <stdarg.h>
#include <regex>
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
static Vector<char> Unicode32ToUtf8(char32_t c32)
{
	Vector<char> buffer;
	int byte_count = 0;

	if (c32 <= 0x7f)
	{
		byte_count = 1;
	}
	else if (c32 <= 0x7ff)
	{
		byte_count = 2;
	}
	else if (c32 <= 0xffff)
	{
		byte_count = 3;
	}
	else if (c32 <= 0x1fffff)
	{
		byte_count = 4;
	}
	else if (c32 <= 0x3ffffff)
	{
		byte_count = 5;
	}
	else if (c32 <= 0x7fffffff)
	{
		byte_count = 6;
	}

	Vector<char> bytes;
	bytes.Reserve(byte_count);
	for (int i = 0; i < byte_count - 1; ++i)
	{
		bytes.Add((c32 & 0x3f) | 0x80);
		c32 >>= 6;
	}

	if (byte_count > 1)
	{
		bytes.Add((char)(c32 | (0xffffff80 >> (byte_count - 1))));
	}
	else
	{
		bytes.Add((char)(c32));
	}

	for (int i = 0; i < byte_count; ++i)
	{
		buffer.Add(bytes[byte_count - 1 - i]);
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
	m_string = &str[0];
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
	m_string = &str[0];
}
int String::IndexOf(const String& str, int start) const
{
	size_t pos = m_string.find(str.m_string, start);
	if (pos != std::string::npos)
	{
		return (int)pos;
	}
	else
	{
		return -1;
	}
}
int String::LastIndexOf(const String& str, int start) const
{
	size_t pos = m_string.rfind(str.m_string, start);
	if (pos != std::string::npos)
	{
		return (int)pos;
	}
	else
	{
		return -1;
	}
}
int String::IndexOf(char ch, int start) const
{
	size_t pos = m_string.find(ch, start);
	if (pos != std::string::npos)
	{
		return (int)pos;
	}
	else
	{
		return -1;
	}
}
int String::LastIndexOf(char ch, int start) const
{
	size_t pos = m_string.rfind(ch, start);
	if (pos != std::string::npos)
	{
		return (int)pos;
	}
	else
	{
		return -1;
	}
}
bool String::StartsWith(const String& str) const
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
		return memcmp(&(*this)[0], &str[0], str.Size()) == 0;
	}
}

bool String::EndsWith(const String& str) const
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
		return memcmp(&(*this)[this->Size() - str.Size()], &str[0], str.Size()) == 0;
	}
}

String String::Substring(int start, int count) const
{
	start = Math::Clamp(start, 0, (int)m_string.size());
	String result;
	result.m_string = m_string.substr(start, count);
	return result;
}
String String::Remove(int start)
{
	return m_string.erase(start);
}
String String::Remove(int start, int count)
{
	return m_string.erase(start, count);
}
String String::Insert(int pos, const String& str)
{
	return m_string.insert(pos, str.c_str());
}
String String::Insert(int pos, const String& str, int count)
{
	return m_string.insert(pos, str.c_str(), count);
}
String String::ToLower() const
{
	Vector<char> str(m_string.size() + 1);
	for (int i = 0; i < (int)m_string.size(); ++i)
	{
		char c = m_string[i];
		if (c >= 'A' && c <= 'Z')
		{
			c -= 'A' - 'a';
		}
		str[i] = c;
	}
	str[str.Size() - 1] = '\0';

	return String(&str[0]);
}

String String::ToUpper() const
{
	Vector<char> str(m_string.size() + 1);
	for (int i = 0; i < (int)m_string.size(); ++i)
	{
		char c = m_string[i];
		if (c >= 'a' && c <= 'z')
		{
			c += 'A' - 'a';
		}
		str[i] = c;
	}
	str[str.Size() - 1] = '\0';

	return String(&str[0]);
}
String String::Trim(const String& src)
{
	if (src.IsEmpty())
	{
		return src;
	}
	char ch = ' ';
	int startIndex = src.FirstNotOf(ch);
	int endIndex = src.LastNotOf(ch);
	return src.Substring(startIndex, endIndex + 1);
}
String String::TrimLeft(const String& src)
{
	if (src.IsEmpty())
	{
		return src;
	}
	char ch = ' ';
	int startIndex = src.FirstNotOf(ch);
	return src.Substring(startIndex, src.Size());
}
String String::TrimRight(const String& src)
{
	if (src.IsEmpty())
	{
		return src;
	}
	char ch = ' ';
	int endIndex = src.LastNotOf(ch);
	return src.Substring(0, endIndex + 1);
}
String String::Replace(const char old, const char to) const
{
	String result(*this);
	std::replace(result.m_string.begin(), result.m_string.end(), old, to);
	return result;
}
String String::Replace(const String& old, const String& to) const
{
	String result(*this);

	int start = 0;
	while (true)
	{
		int index = result.IndexOf(old, start);
		if (index >= 0)
		{
			result.m_string.replace(index, old.m_string.size(), to.m_string);
			start = index + (int)to.m_string.size();
		}
		else
		{
			break;
		}
	}
	
	return result;
}
bool String::Equals(const String& str, bool ignoreCase)const
{
	if (ignoreCase)
		return Stricmp(m_string.c_str(), str.c_str()) == 0 ? true : false;
	else
		return *this == str;
}
bool String::IsANSI() const
{
	AssertEx(false, "");
	return false;
}
Vector<String> String::Split(const String& split) const
{
	Vector<String> tokens;
	if (m_string.empty())return tokens;

	std::string::size_type lastPos = m_string.find_first_not_of(split.m_string, 0);
	std::string::size_type pos = m_string.find_first_of(split.m_string, lastPos);
	while (std::string::npos != pos || std::string::npos != lastPos) 
	{
		tokens.Add(m_string.substr(lastPos, pos - lastPos));
		lastPos = m_string.find_first_not_of(split.m_string, pos);
		pos = m_string.find_first_of(split.m_string, lastPos);
	}
	return tokens;
}
//部分匹配表
static void cal_kmp_next(const String& str, Vector<int>& next)
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
Vector<int> String::KMP(const String& child)
{
	Vector<int> vec;
	Vector<int> next;
	next.Resize(m_string.size(), 0);
	cal_kmp_next(child, next);
	int i = 0;//i是str1的下标
	int j = 0;//j是str2的下标
	int str1_size = m_string.size();
	int str2_size = child.Size();
	while (i < str1_size && j < str2_size)
	{
		//如果j = -1，或者当前字符匹配成功（即S[i] == P[j]），
		//都令i++，j++. 注意：这里判断顺序不能调换！
		if (j == -1 || m_string[i] == child[j])
		{
			++i;
			++j;
		}
		else
		{
			j = next[j];//当前字符匹配失败，直接从str[j]开始比较，i的位置不变
		}
		if (j == str2_size)//匹配成功
		{
			vec.Add(i - j);//记录下完全匹配最开始的位置
			j = -1;//重置
		}
	}
	return vec;
}
static const char BASE64_TABLE[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
String String::Base64(const char* bytes, int size)
{
	int size_pad = size;
	if (size_pad % 3 != 0)
	{
		size_pad += 3 - (size_pad % 3);
	}
	int round = size_pad / 3;

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

	for (int i = size_pad - size, j = 0; i > 0; --i, ++j)
	{
		str[(round - 1) * 4 + 3 - j] = '=';
	}

	return String(str.c_str());
}

String String::UrlDecode(const String& str)
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
String String::Md5(const String& src)
{
	return String::Md5(src.data(), src.Size());
}
String String::Md5(const char* src, int size)
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
String String::TeaEncrypt(const String& src)
{
	return String::TeaEncrypt((unsigned char*)src.data(), src.Size());
}
String String::TeaDecrypt(const String& src)
{
	return String::TeaDecrypt((unsigned char*)src.data(), src.Size());
}
String String::TeaEncrypt(unsigned char* src, int size)
{
	xxtea_long out_length = 0;
	unsigned char* ret = xxtea_encrypt(src, size, nullptr, 0, &out_length);
	if (ret == nullptr)return "";
	String str((const char*)ret, out_length);
	delete[](ret);
	return str;
}
String String::TeaDecrypt(unsigned char* src, int size)
{
	xxtea_long out_length = 0;
	unsigned char* ret = xxtea_decrypt(src, size, nullptr, 0, &out_length);
	if (ret == nullptr)return "";
	String str((const char*)ret, out_length);
	delete[](ret);
	return str;
}
uint String::Hash(const String& src)
{
	return String::Hash(src.data(), src.Size());
}
uint String::Hash(const char* src, int size)
{
	void* state  = XXH32_init(0);
	XXH_errorcode err = XXH32_update(state, src, size);
	if (err != 0)return 0;
	uint ret = XXH32_digest(state);
	return ret;
}
static int Utf8ToUnicode32(const char* utf8, char32_t& c32)
{
	int byte_count = 0;

	for (int i = 0; i < 8; ++i)
	{
		unsigned char c = utf8[0];

		if (((c << i) & 0x80) == 0)
		{
			if (i == 0)
			{
				byte_count = 1;
			}
			else
			{
				byte_count = i;
			}
			break;
		}
	}

	if (byte_count >= 1 && byte_count <= 6)
	{
		char32_t code = 0;

		for (int i = 0; i < byte_count; ++i)
		{
			unsigned int c = utf8[i];
			unsigned char part;

			if (i == 0)
			{
				part = (c << (byte_count + 24)) >> (byte_count + 24);
			}
			else
			{
				part = c & 0x3f;
			}

			code = (code << 6) | part;
		}

		c32 = code;

		return byte_count;
	}
	else
	{
		return 0;
	}
}
Vector<char32_t> String::ToUnicode32() const
{
	Vector<char32_t> unicode;
	int size = (int)m_string.size();
	for (int i = 0; i < size; ++i)
	{
		char32_t unicode32 = 0;
		int byte_count = Utf8ToUnicode32(&m_string[i], unicode32);
		if (byte_count > 0)
		{
			unicode.Add(unicode32);

			i += byte_count - 1;
		}
		else
		{
			break;
		}
	}

	return unicode;
}
String String::FromWString(const wchar_t* pw, int len)
{
	return Encoding::WCharToUtf8(pw, len);
}
String String::FromWString(const std::wstring& w_str)
{
	return Encoding::WCharToUtf8(w_str);
}
std::wstring String::ToWString()const
{
	return Encoding::Utf8ToWChar(m_string);
}
float String::Expression(const String& expr)
{
	if (expr.IsEmpty())return 0;
	float v = (float)te_interp(expr.c_str(), 0);
	return v;
}
bool String::Match(const String& match, const String& str)
{
	std::regex reg(match.c_str());
	bool result = std::regex_match(str.c_str(), reg);
	return result;
}
const char* String::Stristr(const char* haystack, const char* needle)
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