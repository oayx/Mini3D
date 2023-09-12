
/*****************************************************************************************************/
// @author hannibal
// @date   2015/04/29
// @brief 字符串封装
/*****************************************************************************************************/
#pragma once

#include "BaseType.h"
#include "Assert.h"
#include "stl/Vector.h"
#include <external/format/format.h>

/********************************************************************/
DC_BEGIN_NAMESPACE
class ENGINE_DLL String Final : public object
{
	BEGIN_REFECTION_TYPE(String)
	END_FINAL_REFECTION_TYPE;

public:
	String() {}
	String(const char* str) {if (str != nullptr)m_string = str;}
	String(const char* str, int size) :m_string(str, size){}
	String(const char32_t* unicode32);
	String(const char32_t* unicode32, int size);
	String(const std::string& str) :m_string(str){}

	int		Size() const { return (int)m_string.size(); }
	bool	IsEmpty() const { return m_string.empty(); }

	int		IndexOf(const String& str, int start = 0) const;
	int		LastIndexOf(const String& str, int start = -1) const;
	int		IndexOf(char ch, int start = 0) const;
	int		LastIndexOf(char ch, int start = -1) const;
	int		FirstNotOf(char ch)const { return (int)m_string.find_first_not_of(ch); }
	int		LastNotOf(char ch)const { return (int)m_string.find_last_not_of(ch); }

	bool	StartsWith(const String& str) const;
	bool	EndsWith(const String& str) const;
	String	Substring(int start, int count = -1) const;

	String	Remove(int start);
	String	Remove(int start, int count);

	String	Insert(int pos, const String& str);
	String	Insert(int pos, const String& str, int count);

	String	ToLower() const;
	String	ToUpper() const;

	String	Trim(const String& src);
	String	TrimLeft(const String& src);
	String	TrimRight(const String& src);

	String	Replace(const char old, const char to) const;
	String	Replace(const String& old, const String& to) const;
	bool	Equals(const String& str, bool ignoreCase = false)const;
	bool	Contains(const String& str) const { return this->IndexOf(str) >= 0; }

	bool	IsANSI() const;

	String& operator = (const char* str) { if (str != nullptr)m_string = str; else m_string = ""; return *this; }
	String& operator = (const std::string& str) { m_string = str; return *this; }
	bool	operator ==(const String& right) const{if (this->Size() == right.Size()) { return memcmp(this->m_string.data(), right.m_string.data(), (int)m_string.size()) == 0; }return false;}
	bool	operator !=(const String& right) const { return !(*this == right); }
	bool	operator ==(const char* right) const{if (right && this->Size() == strlen(right)) { return memcmp(this->m_string.data(), right, this->Size()) == 0; }return false;}
	bool	operator !=(const char* right) const { return !(*this == right); }
	String	operator +(const String& right) const { String result; result.m_string = m_string + right.m_string; return result; }
	String& operator +=(const String& right) { *this = *this + right; return *this; }
	bool	operator <(const String& right) const { return m_string < right.m_string; }
	bool	operator >(const String& right) const { return m_string > right.m_string; }
	String	operator /(const String& right)const { return m_string + "/" + right.m_string; }
	String	operator /(const std::string& right)const { return m_string + "/" + right; }
	char&		operator[](int index) { return m_string[index]; }
	const char& operator[](int index) const { return m_string[index]; }
	operator std::string() { return m_string; }
	std::size_t operator()() const { return std::hash<std::string>()(m_string); }

	uint64	GetHash() const { return (uint64)std::hash<std::string>()(m_string); }
	byte	ToByte()const { return (unsigned char)std::stoul(m_string); }
	short	ToShort()const { return std::stoi(m_string); }
	ushort	ToUShort()const { return (unsigned short)std::stoul(m_string); }
	int		ToInt()const { return std::stoi(m_string); }
	uint	ToUInt()const { return std::stoul(m_string); }
	int64	ToInt64()const { return std::stoll(m_string); }
	uint64	ToUInt64()const { return std::stoull(m_string); }
	float	ToFloat()const { return std::stof(m_string); }
	double	ToDouble()const { return std::stod(m_string); }
	bool	ToBool()const { if (ToLower() == "true") { return true; }return false; }

	Vector<char32_t> ToUnicode32() const;
	static String	 FromWString(const wchar_t* pw, int len);
	static String	 FromWString(const std::wstring& w_str);
	std::wstring	 ToWString()const;

	template<class T> Vector<T> Split(char split)const;
	Vector<String>	  Split(const String& split)const;

	//查找子串出现位置
	Vector<int>		 KMP(const String& child);

	const char*		 c_str() const { return m_string.c_str(); }
	const char*		 data() const { return m_string.data(); }

public:
	//C#风格
	template <typename... Args>
	static String Format(const char* format, const Args & ... args) { return fmt::format(format, args...); }

	static String Base64(const char* bytes, int size);
	static String UrlDecode(const String& str);
	static String Md5(const String& src);
	static String Md5(const char* src, int size);
	static String TeaEncrypt(const String& src);
	static String TeaDecrypt(const String& src);
	static String TeaEncrypt(unsigned char* src, int size);
	static String TeaDecrypt(unsigned char* src, int size);
	static uint   Hash(const String& src);
	static uint   Hash(const char* src, int size);

	//表达式求值
	static float Expression(const String& expr);
	//正则表达式
	static bool  Match(const String& match, const String& str);

	//查找子字符串
	static const char* Stristr(const char* haystack, const char* needle);

	template<class V>
	static String ToString(const V& v) { return std::to_string(v); }

	static String Empty;

private:
	std::string m_string = "";
};
typedef Vector<String> VecString;
inline bool operator ==(const char* left, const String& right)
{
	return right == left;
}
inline bool operator !=(const char* left, const String& right)
{
	return right != left;
}
inline String operator +(const char* left, const String& right)
{
	return String(left) + right;
}
template<class T>
Vector<T> String::Split(char split)const
{
	Vector<T> tokens;
	if (m_string.empty())return tokens;

	std::string::size_type lastPos = m_string.find_first_not_of(split, 0);
	std::string::size_type pos = m_string.find_first_of(split, lastPos);
	while (std::string::npos != pos || std::string::npos != lastPos)
	{
		std::string s = m_string.substr(lastPos, pos - lastPos);
		lastPos = m_string.find_first_not_of(split, pos);
		pos = m_string.find_first_of(split, lastPos);
		try
		{
			if (std::is_same<int, T>::value)
				tokens.Add((T)std::stoi(s));
			else if (std::is_same<uint, T>::value)
				tokens.Add((T)std::stoul(s));
			else if (std::is_same<int64, T>::value)
				tokens.Add((T)std::stoll(s));
			else if (std::is_same<uint64, T>::value)
				tokens.Add((T)std::stoull(s));
			else if (std::is_same<short, T>::value)
				tokens.Add((T)std::stoi(s));
			else if (std::is_same<ushort, T>::value)
				tokens.Add((T)std::stoul(s));
			else if (std::is_same<char, T>::value)
				tokens.Add((T)s[0]);
			else if (std::is_same<byte, T>::value)
				tokens.Add((T)std::stoul(s));
			else if (std::is_same<float, T>::value)
				tokens.Add((T)std::stof(s));
			else if (std::is_same<double, T>::value)
				tokens.Add((T)std::stod(s));
			else if (std::is_same<bool, T>::value)
				tokens.Add((T)(Stricmp(s.c_str(), "true") == 0 ? true : false));
			else
				AssertEx(false, "error type");
		}
		catch (...)
		{
			Debuger::Error("Convert string error:%s", m_string.c_str());
			break;
		}
	}

	return tokens;
}
DC_END_NAMESPACE
