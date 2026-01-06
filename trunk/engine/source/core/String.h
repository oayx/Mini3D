
/*****************************************************************************************************/
// @author hannibal
// @date   2015/04/29
// @brief 字符串封装
/*****************************************************************************************************/
#pragma once

#include "BaseType.h"
#include "Assert.h"
#include "stl/Vector.h"
#include <type_traits>
#include <external/format/format.h>

/********************************************************************/
DC_BEGIN_NAMESPACE
class ENGINE_DLL String final : public object
{
	BEGIN_REFECTION_TYPE(String)
	END_FINAL_REFECTION_TYPE;

public:
	String() {}
	String(const char* str) {if (str != nullptr)_string = str;}
	String(const char* str, int size) :_string(str, size){}
	String(const char32_t* unicode32);
	String(const char32_t* unicode32, int size);
	String(const std::string& str) :_string(str){}
	String(const String& other) :_string(other._string) {}
	String(String&& other)noexcept : _string(std::move(other._string)) {}

	int		Size() const noexcept { return (int)_string.size(); }
	bool	IsEmpty() const noexcept { return _string.empty(); }

	int		IndexOf(const String& str, int start = 0, bool ignoreCase = false) const noexcept;
	int		LastIndexOf(const String& str, int start = -1, bool ignoreCase = false) const noexcept;
	int		IndexOf(char ch, int start = 0, bool ignoreCase = false) const noexcept;
	int		LastIndexOf(char ch, int start = -1, bool ignoreCase = false) const noexcept;
	int		FirstNotOf(char ch)const noexcept { return (int)_string.find_first_not_of(ch); }
	int		LastNotOf(char ch)const noexcept { return (int)_string.find_last_not_of(ch); }

	bool	StartsWith(const String& str, bool ignoreCase = false) const noexcept;
	bool	EndsWith(const String& str, bool ignoreCase = false) const noexcept;
	String	Substring(int start, int count = -1) const noexcept;

	String	Remove(int start) noexcept;
	String	Remove(int start, int count) noexcept;

	String	Insert(int pos, const String& str) noexcept;
	String	Insert(int pos, const String& str, int count) noexcept;

	String	ToLower() const noexcept;
	String	ToUpper() const noexcept;

	String	Trim(const String& src) noexcept;
	String	TrimLeft(const String& src) noexcept;
	String	TrimRight(const String& src) noexcept;

	String	Replace(const char old, const char to) const noexcept;
	String	Replace(const String& old, const String& to) const noexcept;
	bool	Equals(const String& str, bool ignoreCase = false)const noexcept;
	bool	Contains(const String& str, bool ignoreCase = false) const noexcept { return this->IndexOf(str, 0, ignoreCase) >= 0; }

	bool	IsANSI() const noexcept;

	String& operator=(const String& other)noexcept { _string = other._string; return *this; }
	String& operator=(String&& other)noexcept { _string = std::move(other._string); return *this;}
	String& operator = (const char* str) noexcept { if (str != nullptr)_string = str; else _string = ""; return *this; }
	String& operator = (const std::string& str) noexcept { _string = str; return *this; }
	bool	operator ==(const String& right) const noexcept {if (this->Size() == right.Size()) { return memcmp(this->_string.data(), right._string.data(), (int)_string.size()) == 0; }return false;}
	bool	operator !=(const String& right) const noexcept { return !(*this == right); }
	bool	operator ==(const char* right) const noexcept {if (right && this->Size() == strlen(right)) { return memcmp(this->_string.data(), right, this->Size()) == 0; }return false;}
	bool	operator !=(const char* right) const noexcept { return !(*this == right); }
	String	operator +(const String& right) const noexcept { String result; result._string = _string + right._string; return result; }
	String& operator +=(const String& right) noexcept { *this = *this + right; return *this; }
	bool	operator <(const String& right) const noexcept { return _string < right._string; }
	bool	operator >(const String& right) const noexcept { return _string > right._string; }
	String	operator /(const String& right)const noexcept { return _string + "/" + right._string; }
	String	operator /(const std::string& right)const noexcept { return _string + "/" + right; }
	char&		operator[](int index) noexcept { return _string[index]; }
	const char& operator[](int index) const noexcept { return _string[index]; }
	operator std::string() noexcept { return _string; }
	std::size_t operator()() const noexcept { return std::hash<std::string>()(_string); }

	uint64	GetHash() const noexcept { return (uint64)std::hash<std::string>()(_string); }
	byte	ToByte()const noexcept { return (unsigned char)std::stoul(_string); }
	short	ToShort()const noexcept { return std::stoi(_string); }
	ushort	ToUShort()const noexcept { return (unsigned short)std::stoul(_string); }
	int		ToInt()const noexcept { return std::stoi(_string); }
	uint	ToUInt()const noexcept { return std::stoul(_string); }
	int64	ToInt64()const noexcept { return std::stoll(_string); }
	uint64	ToUInt64()const noexcept { return std::stoull(_string); }
	float	ToFloat()const noexcept { return std::stof(_string); }
	double	ToDouble()const noexcept { return std::stod(_string); }
	bool	ToBool()const noexcept { if (ToLower() == "true") { return true; }return false; }

	Vector<char32_t> ToUnicode32() const noexcept;
	static String	 FromWString(const wchar_t* pw, int len) noexcept;
	static String	 FromWString(const std::wstring& w_str) noexcept;
	std::wstring	 ToWString()const noexcept;

	template<class T> Vector<T> Split(char split)const noexcept;
	Vector<String>	  Split(const String& split)const noexcept;

	//查找子串出现位置
	Vector<int>		 KMP(const String& child) noexcept;

	const char*		 c_str() const noexcept { return _string.c_str(); }
	const char*		 data() const noexcept { return _string.data(); }

public:
	//C#风格
	template <typename... Args>
	static String Format(const char* format, const Args & ... args) noexcept { return fmt::format(format, args...); }
	template <typename... Args>
	static String Format(const std::string& format, const Args & ... args) noexcept { return fmt::format(format, args...); }

	static String Base64(const char* bytes, int size) noexcept;
	static String UrlDecode(const String& str) noexcept;
	static String Md5(const String& src) noexcept;
	static String Md5(const char* src, int size) noexcept;
	static String TeaEncrypt(const String& src) noexcept;
	static String TeaDecrypt(const String& src) noexcept;
	static String TeaEncrypt(unsigned char* src, int size) noexcept;
	static String TeaDecrypt(unsigned char* src, int size) noexcept;
	static uint   Hash(const String& src) noexcept;
	static uint   Hash(const char* src, int size) noexcept;

	//表达式求值
	static float Expression(const String& expr) noexcept;
	//正则表达式
	static bool  Match(const String& match, const String& str) noexcept;

	//查找子字符串
	static const char* Stristr(const char* haystack, const char* needle) noexcept;

	template<class V>
	static String ToString(const V& v) noexcept { return std::to_string(v); }

	static String Empty;

private:
	std::string _string = "";
};
typedef Vector<String> VecString;
inline bool operator ==(const char* left, const String& right) noexcept
{
	return right == left;
}
inline bool operator !=(const char* left, const String& right) noexcept
{
	return right != left;
}
inline String operator +(const char* left, const String& right) noexcept
{
	return String(left) + right;
}
template<class T>
Vector<T> String::Split(char split)const noexcept
{
	Vector<T> tokens;
	if (_string.empty())return tokens;

	std::string::size_type lastPos = _string.find_first_not_of(split, 0);
	std::string::size_type pos = _string.find_first_of(split, lastPos);
	while (std::string::npos != pos || std::string::npos != lastPos)
	{
		std::string s = _string.substr(lastPos, pos - lastPos);
		lastPos = _string.find_first_not_of(split, pos);
		pos = _string.find_first_of(split, lastPos);
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
			Debuger::Error("Convert string error:%s", _string.c_str());
			break;
		}
	}

	return tokens;
}
DC_END_NAMESPACE
