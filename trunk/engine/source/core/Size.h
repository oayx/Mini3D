 
/*****************************************************************************************************/
// @author hannibal
// @date   2015/04/21
// @brief   
/*****************************************************************************************************/
#pragma once

#include "math/Math.h"
#include "String.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
template<typename T>
class ENGINE_DLL tSize Final : public object
{
	BEGIN_REFECTION_TYPE(tSize)
	END_FINAL_REFECTION_TYPE;
	
public:
	tSize() { Set(T(0), T(0)); }
	tSize(T _width, T _height) { Set(_width, _height); }
	tSize(const tSize& other) { Set(other.width, other.height); }
	tSize(const String& str)
	{
		Vector<float> vec = str.Split<float>(',');
		if (vec.Size() == 2) { width = (T)vec[0]; height = (T)vec[1]; }
	}

public:
	tSize& operator =(const tSize& other) { Set(other.width, other.height); return *this; }
	bool operator <(const tSize &other)const { return (width < other.width); }
	bool operator ==(const tSize &other)const { return this->Equals(other); }
	bool operator !=(const tSize &other)const { return !this->Equals(other); }

	tSize operator +(const tSize &other)const { return tSize(width + other.width, height + other.height); }
	tSize operator -(const tSize &other)const { return tSize(width - other.width, height - other.height); }
	tSize operator *(float scale)const { return tSize(T(width * scale), T(height * scale)); }
	tSize operator /(float scale)const { return tSize(T(width / scale), T(height / scale)); }

public:
	void Set(T width, T height) { this->width = width; this->height = height; }
	bool Equals(const tSize& target) const { return Math::FloatEqual((float)width, (float)target.width) && Math::FloatEqual((float)height, (float)target.height); }

	String ToString()const { char arr[128] = { 0 }; Sprintf(arr, "%f, %f", (float)width, (float)height); return String(arr); }

public:
	union
	{
		struct { T width, height; };
		T p[2] = { T(0) };
	};

	static const tSize zero;
};
typedef tSize<float> Size;
typedef tSize<int> iSize;
typedef tSize<uint> uSize;

template<typename T>
const tSize<T> tSize<T>::zero((T)0, (T)0);

DC_END_NAMESPACE