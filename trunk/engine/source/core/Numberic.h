
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
class ENGINE_DLL Bool Final : public object
{
	BEGIN_REFECTION_TYPE(Bool)
	END_FINAL_REFECTION_TYPE;
	
public:
	static const bool True = true;
	static const bool False = false;

public:
	static bool isTrue(const char* strValue)
	{
		if (Strncmp(strValue, "true", sizeof("true")) == 0 || Strncmp(strValue, "True", sizeof("True")) == 0
			|| Strncmp(strValue, "TRUE", sizeof("TRUE")) == 0)
		{
			return true;
		}
		return false;
	}
	static bool isTrue(const std::string& strValue)
	{
		if (Strncmp(strValue.c_str(), "true", sizeof("true")) == 0 || Strncmp(strValue.c_str(), "True", sizeof("True")) == 0
			|| Strncmp(strValue.c_str(), "TRUE", sizeof("TRUE")) == 0)
		{
			return true;
		}
		return false;
	}
	static bool isTrue(const String& strValue)
	{
		if (Strncmp(strValue.c_str(), "true", sizeof("true")) == 0 || Strncmp(strValue.c_str(), "True", sizeof("True")) == 0
			|| Strncmp(strValue.c_str(), "TRUE", sizeof("TRUE")) == 0)
		{
			return true;
		}
		return false;
	}
	static bool FromString(const String& strValue)
	{
		return isTrue(strValue);
	}
	static const char* ToString(bool b)
	{
		return b ? "true" : "false";
	}
};

/********************************************************************/
/**
* 大小端转换
*/
class ENGINE_DLL ByteOrder Final : public object
{
	BEGIN_REFECTION_TYPE(ByteOrder)
	END_FINAL_REFECTION_TYPE;
	
	union  U32F32
	{
		uint i;
		float f;
	};
public:
	inline ushort SwapU16(ushort value)
	{
		return ((value & 0x00ff) << 8) | ((value & 0xff00) >> 8);
	}
	inline uint SwapU32(uint value)
	{
		return ((value & 0x000000ff) << 24)
			| ((value & 0x0000ff00) << 8)
			| ((value & 0x00ff0000) >> 8)
			| ((value & 0xff000000) >> 24);
	}
	inline float SwapFloat(float value)
	{
		U32F32 u;
		u.f = value;
		u.i = SwapU32(u.i);
		return u.f;
	}
	//判断是否小端模式
	inline bool IsSmallOrder()
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
	T m_var;

public:
	T value() { return m_var; }

	virtual String ToString()
	{
		std::string str = "";
		std::stringstream kStream;
		kStream << m_var;
		kStream >> str;
		return str;
	}
};
/********************************************************************/
/**
* 封装int
*/
class ENGINE_DLL Int Final : public Numberic<int>
{
public:
	static const int MIN_VALUE = INT_MIN;
	static const int MAX_VALUE = INT_MAX;

public:
	//构造函数
	Int(const int& t = 0) { this->m_var = t; };
	//自动转换
	operator int() { return this->m_var; };
	operator const int() const { return (const int)this->m_var; };
	//取址运算
	int* operator &() { return &this->m_var; };
	//二目运算
	const int& operator=(const int& t) { this->m_var = t; return this->m_var; };
	const int& operator=(const char* s) { this->m_var = atoi(s); return this->m_var; };
	const int& operator+=(const int& t) { this->m_var += t; return this->m_var; };
	const int& operator-=(const int& t) { this->m_var -= t; return this->m_var; };
	const int& operator*=(const int& t) { this->m_var *= t; return this->m_var; };
	const int& operator/=(const int& t) { this->m_var /= t; return this->m_var; };
	const int& operator%=(const int& t) { this->m_var %= t; return this->m_var; };
	const int& operator^=(const int& t) { this->m_var ^= t; return this->m_var; };
	const int& operator&=(const int& t) { this->m_var &= t; return this->m_var; };
	const int& operator|=(const int& t) { this->m_var |= t; return this->m_var; };
	const int& operator>>=(const int& t) { this->m_var >>= t; return this->m_var; };
	const int& operator<<=(const int& t) { this->m_var <<= t; return this->m_var; };
	const int operator+(const int& t) { return this->m_var + t; };
	const int operator-(const int& t) { return this->m_var - t; };
	const int operator*(const int& t) { return this->m_var * t; };
	const int operator/(const int& t) { return this->m_var / t; };
	const int operator%(const int& t) { return this->m_var % t; };
	const int operator^(const int& t) { return this->m_var ^ t; };
	const int operator&(const int& t) { return this->m_var & t; };
	const int operator|(const int& t) { return this->m_var | t; };
	const int operator>>(const int& t) { return this->m_var >> t; };
	const int operator<<(const int& t) { return this->m_var << t; };
	//单目运算
	void operator ++() { m_var++; };
	void operator --() { m_var--; };
	//比较运算
	virtual bool operator == (const int& t) const { return this->m_var == t; };
	virtual bool operator != (const int& t) const { return this->m_var != t; };
	virtual bool operator <  (const int& t) const { return this->m_var < t; };
	virtual bool operator <= (const int& t) const { return this->m_var <= t; };
	virtual bool operator >  (const int& t) const { return this->m_var > t; };
	virtual bool operator >= (const int& t) const { return this->m_var >= t; };
};

class ENGINE_DLL UInt Final : public Numberic<unsigned int>
{
public:
	static const unsigned int MIN_VALUE = 0;
	static const unsigned int MAX_VALUE = UINT_MAX;

public:
	//构造函数
	UInt(const unsigned int& t = 0) { this->m_var = t; };
	//自动转换
	operator unsigned int() { return this->m_var; };
	operator const unsigned int() const { return (const unsigned int)this->m_var; };
	//取址运算
	unsigned int* operator &() { return &this->m_var; };
	//二目运算
	const unsigned int& operator=(const unsigned int& t) { this->m_var = t; return this->m_var; };
	const unsigned int& operator=(const char* s) { this->m_var = atoi(s); return this->m_var; };
	const unsigned int& operator+=(const unsigned int& t) { this->m_var += t; return this->m_var; };
	const unsigned int& operator-=(const unsigned int& t) { this->m_var -= t; return this->m_var; };
	const unsigned int& operator*=(const unsigned int& t) { this->m_var *= t; return this->m_var; };
	const unsigned int& operator/=(const unsigned int& t) { this->m_var /= t; return this->m_var; };
	const unsigned int& operator%=(const unsigned int& t) { this->m_var %= t; return this->m_var; };
	const unsigned int& operator^=(const unsigned int& t) { this->m_var ^= t; return this->m_var; };
	const unsigned int& operator&=(const unsigned int& t) { this->m_var &= t; return this->m_var; };
	const unsigned int& operator|=(const unsigned int& t) { this->m_var |= t; return this->m_var; };
	const unsigned int& operator>>=(const unsigned int& t) { this->m_var >>= t; return this->m_var; };
	const unsigned int& operator<<=(const unsigned int& t) { this->m_var <<= t; return this->m_var; };
	const unsigned int operator+(const unsigned int& t) { return this->m_var + t; };
	const unsigned int operator-(const unsigned int& t) { return this->m_var - t; };
	const unsigned int operator*(const unsigned int& t) { return this->m_var * t; };
	const unsigned int operator/(const unsigned int& t) { return this->m_var / t; };
	const unsigned int operator%(const unsigned int& t) { return this->m_var % t; };
	const unsigned int operator^(const unsigned int& t) { return this->m_var ^ t; };
	const unsigned int operator&(const unsigned int& t) { return this->m_var & t; };
	const unsigned int operator|(const unsigned int& t) { return this->m_var | t; };
	const unsigned int operator>>(const unsigned int& t) { return this->m_var >> t; };
	const unsigned int operator<<(const unsigned int& t) { return this->m_var << t; };
	//单目运算
	void operator ++() { m_var++; };
	void operator --() { m_var--; };
	//比较运算
	virtual bool operator == (const unsigned int& t) const { return this->m_var == t; };
	virtual bool operator != (const unsigned int& t) const { return this->m_var != t; };
	virtual bool operator <  (const unsigned int& t) const { return this->m_var < t; };
	virtual bool operator <= (const unsigned int& t) const { return this->m_var <= t; };
	virtual bool operator >  (const unsigned int& t) const { return this->m_var > t; };
	virtual bool operator >= (const unsigned int& t) const { return this->m_var >= t; };
};
DC_END_NAMESPACE
