
/*****************************************************************************************************/
// @author hannibal
// @date   2015/09/09
// @brief  封装数值 
/*****************************************************************************************************/
#pragma once

#include "Assert.h"
#include "String.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
/**
* 封装bool
*/
class ENGINE_DLL Bool final : public object
{
	BEGIN_REFECTION_TYPE(Bool)
	END_FINAL_REFECTION_TYPE;
	
public:
	static constexpr bool True = true;
	static constexpr bool False = false;

public:
	static bool IsTrue(const char* strValue) noexcept
	{
		if (Strncmp(strValue, "true", sizeof("true")) == 0 || Strncmp(strValue, "True", sizeof("True")) == 0
			|| Strncmp(strValue, "TRUE", sizeof("TRUE")) == 0)
		{
			return true;
		}
		return false;
	}
	static bool IsTrue(const std::string& strValue) noexcept
	{
		if (Strncmp(strValue.c_str(), "true", sizeof("true")) == 0 || Strncmp(strValue.c_str(), "True", sizeof("True")) == 0
			|| Strncmp(strValue.c_str(), "TRUE", sizeof("TRUE")) == 0)
		{
			return true;
		}
		return false;
	}
	static bool IsTrue(const String& strValue) noexcept
	{
		if (Strncmp(strValue.c_str(), "true", sizeof("true")) == 0 || Strncmp(strValue.c_str(), "True", sizeof("True")) == 0
			|| Strncmp(strValue.c_str(), "TRUE", sizeof("TRUE")) == 0)
		{
			return true;
		}
		return false;
	}
	static bool FromString(const String& strValue) noexcept
	{
		return IsTrue(strValue);
	}
	constexpr static const char* ToString(bool b) noexcept
	{
		return b ? "true" : "false";
	}
};

/********************************************************************/
/**
* 大小端转换
*/
class ENGINE_DLL ByteOrder final : public object
{
	BEGIN_REFECTION_TYPE(ByteOrder)
	END_FINAL_REFECTION_TYPE;
	
	union  U32F32
	{
		uint i;
		float f;
	};
public:
	constexpr inline ushort SwapU16(ushort value) noexcept
	{
		return ((value & 0x00ff) << 8) | ((value & 0xff00) >> 8);
	}
	constexpr inline uint SwapU32(uint value) noexcept
	{
		return ((value & 0x000000ff) << 24)
			| ((value & 0x0000ff00) << 8)
			| ((value & 0x00ff0000) >> 8)
			| ((value & 0xff000000) >> 24);
	}
	inline float SwapFloat(float value) noexcept
	{
		U32F32 u;
		u.f = value;
		u.i = SwapU32(u.i);
		return u.f;
	}
	//判断是否小端模式
	inline bool IsSmallOrder() noexcept
	{
		unsigned short v = 0x0102;
		unsigned char *p = (unsigned char *)&v;

		if (*p == 0x02)
			return true;
		return false;
	}
};
/********************************************************************/
/**
* 数值基础类
*/
template <class T>
class ENGINE_DLL Numberic : public object
{
protected:
	T _var;

public:
	T value() { return _var; }

	virtual String ToString()
	{
		std::string str = "";
		std::stringstream kStream;
		kStream << _var;
		kStream >> str;
		return str;
	}
};
/********************************************************************/
/**
* 封装int
*/
class ENGINE_DLL Int final : public Numberic<int>
{
public:
	static constexpr int MIN_VALUE = INT_MIN;
	static constexpr int MAX_VALUE = INT_MAX;

public:
	//构造函数
	Int(const int& t = 0) { this->_var = t; };
	//自动转换
	operator int() noexcept { return this->_var; };
	operator const int() const noexcept { return (const int)this->_var; };
	//取址运算
	int* operator &() noexcept { return &this->_var; };
	//二目运算
	const int& operator=(const int& t) noexcept { this->_var = t; return this->_var; };
	const int& operator=(const char* s) noexcept { this->_var = atoi(s); return this->_var; };
	const int& operator+=(const int& t) noexcept { this->_var += t; return this->_var; };
	const int& operator-=(const int& t) noexcept { this->_var -= t; return this->_var; };
	const int& operator*=(const int& t) noexcept { this->_var *= t; return this->_var; };
	const int& operator/=(const int& t) noexcept { this->_var /= t; return this->_var; };
	const int& operator%=(const int& t) noexcept { this->_var %= t; return this->_var; };
	const int& operator^=(const int& t) noexcept { this->_var ^= t; return this->_var; };
	const int& operator&=(const int& t) noexcept { this->_var &= t; return this->_var; };
	const int& operator|=(const int& t) noexcept { this->_var |= t; return this->_var; };
	const int& operator>>=(const int& t) noexcept { this->_var >>= t; return this->_var; };
	const int& operator<<=(const int& t) noexcept { this->_var <<= t; return this->_var; };
	const int operator+(const int& t) noexcept { return this->_var + t; };
	const int operator-(const int& t) noexcept { return this->_var - t; };
	const int operator*(const int& t) noexcept { return this->_var * t; };
	const int operator/(const int& t) noexcept { return this->_var / t; };
	const int operator%(const int& t) noexcept { return this->_var % t; };
	const int operator^(const int& t) noexcept { return this->_var ^ t; };
	const int operator&(const int& t) noexcept { return this->_var & t; };
	const int operator|(const int& t) noexcept { return this->_var | t; };
	const int operator>>(const int& t) noexcept { return this->_var >> t; };
	const int operator<<(const int& t) noexcept { return this->_var << t; };
	//单目运算
	void operator ++() { _var++; };
	void operator --() { _var--; };
	//比较运算
	virtual bool operator == (const int& t) const noexcept { return this->_var == t; };
	virtual bool operator != (const int& t) const noexcept { return this->_var != t; };
	virtual bool operator <  (const int& t) const noexcept { return this->_var < t; };
	virtual bool operator <= (const int& t) const noexcept { return this->_var <= t; };
	virtual bool operator >  (const int& t) const noexcept { return this->_var > t; };
	virtual bool operator >= (const int& t) const noexcept { return this->_var >= t; };
};

class ENGINE_DLL UInt final : public Numberic<unsigned int>
{
public:
	static constexpr unsigned int MIN_VALUE = 0;
	static constexpr unsigned int MAX_VALUE = UINT_MAX;

public:
	//构造函数
	UInt(const unsigned int& t = 0) { this->_var = t; };
	//自动转换
	operator unsigned int() noexcept { return this->_var; };
	operator const unsigned int() const noexcept { return (const unsigned int)this->_var; };
	//取址运算
	unsigned int* operator &() noexcept { return &this->_var; };
	//二目运算
	const unsigned int& operator=(const unsigned int& t) noexcept { this->_var = t; return this->_var; };
	const unsigned int& operator=(const char* s) noexcept { this->_var = atoi(s); return this->_var; };
	const unsigned int& operator+=(const unsigned int& t) noexcept { this->_var += t; return this->_var; };
	const unsigned int& operator-=(const unsigned int& t) noexcept { this->_var -= t; return this->_var; };
	const unsigned int& operator*=(const unsigned int& t) noexcept { this->_var *= t; return this->_var; };
	const unsigned int& operator/=(const unsigned int& t) noexcept { this->_var /= t; return this->_var; };
	const unsigned int& operator%=(const unsigned int& t) noexcept { this->_var %= t; return this->_var; };
	const unsigned int& operator^=(const unsigned int& t) noexcept { this->_var ^= t; return this->_var; };
	const unsigned int& operator&=(const unsigned int& t) noexcept { this->_var &= t; return this->_var; };
	const unsigned int& operator|=(const unsigned int& t) noexcept { this->_var |= t; return this->_var; };
	const unsigned int& operator>>=(const unsigned int& t) noexcept { this->_var >>= t; return this->_var; };
	const unsigned int& operator<<=(const unsigned int& t) noexcept { this->_var <<= t; return this->_var; };
	const unsigned int operator+(const unsigned int& t) noexcept { return this->_var + t; };
	const unsigned int operator-(const unsigned int& t) noexcept { return this->_var - t; };
	const unsigned int operator*(const unsigned int& t) noexcept { return this->_var * t; };
	const unsigned int operator/(const unsigned int& t) noexcept { return this->_var / t; };
	const unsigned int operator%(const unsigned int& t) noexcept { return this->_var % t; };
	const unsigned int operator^(const unsigned int& t) noexcept { return this->_var ^ t; };
	const unsigned int operator&(const unsigned int& t) noexcept { return this->_var & t; };
	const unsigned int operator|(const unsigned int& t) noexcept { return this->_var | t; };
	const unsigned int operator>>(const unsigned int& t) noexcept { return this->_var >> t; };
	const unsigned int operator<<(const unsigned int& t) noexcept { return this->_var << t; };
	//单目运算
	void operator ++() noexcept { _var++; };
	void operator --() noexcept { _var--; };
	//比较运算
	virtual bool operator == (const unsigned int& t) const noexcept { return this->_var == t; };
	virtual bool operator != (const unsigned int& t) const noexcept { return this->_var != t; };
	virtual bool operator <  (const unsigned int& t) const noexcept { return this->_var < t; };
	virtual bool operator <= (const unsigned int& t) const noexcept { return this->_var <= t; };
	virtual bool operator >  (const unsigned int& t) const noexcept { return this->_var > t; };
	virtual bool operator >= (const unsigned int& t) const noexcept { return this->_var >= t; };
};
DC_END_NAMESPACE
