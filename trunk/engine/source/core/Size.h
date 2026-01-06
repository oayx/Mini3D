 
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
class ENGINE_DLL tSize final : public object
{
	BEGIN_REFECTION_TYPE(tSize)
	END_FINAL_REFECTION_TYPE;
	
public:
	constexpr tSize() : width(T(0)), height(T(0)) {}
	constexpr tSize(T _width, T _height) : width(_width), height(_height) {}
	constexpr tSize(const tSize& other) : width(other.width), height(other.height) {}
	tSize(const String& str)
	{
		Vector<float> vec = str.Split<float>(',');
		if (vec.Size() == 2) { width = (T)vec[0]; height = (T)vec[1]; }
	}

public:
	tSize& operator =(const tSize& other) noexcept { Set(other.width, other.height); return *this; }
	constexpr bool operator <(const tSize &other)const noexcept { return (width < other.width); }
	constexpr bool operator ==(const tSize &other)const noexcept { return this->Equals(other); }
	constexpr bool operator !=(const tSize &other)const noexcept { return !this->Equals(other); }

	constexpr tSize operator +(const tSize &other)const noexcept { return tSize(width + other.width, height + other.height); }
	constexpr tSize operator -(const tSize &other)const noexcept { return tSize(width - other.width, height - other.height); }
	constexpr tSize operator *(float scale)const noexcept { return tSize(T(width * scale), T(height * scale)); }
	constexpr tSize operator /(float scale)const noexcept { return tSize(T(width / scale), T(height / scale)); }

public:
	void Set(T width, T height) noexcept { this->width = width; this->height = height; }
	constexpr bool Equals(const tSize& target) const noexcept { return Math::FloatEqual((float)width, (float)target.width) && Math::FloatEqual((float)height, (float)target.height); }

	String ToString()const noexcept { char arr[128] = { 0 }; Snprintf(arr, sizeof(arr), "%f, %f", (float)width, (float)height); return String(arr); }

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
static_assert(sizeof(Size) == 2 * sizeof(float), "invalid bytes");
static_assert(sizeof(iSize) == 2 * sizeof(int), "invalid bytes");
static_assert(sizeof(uSize) == 2 * sizeof(uint), "invalid bytes");

template<typename T>
const tSize<T> tSize<T>::zero((T)0, (T)0);

DC_END_NAMESPACE