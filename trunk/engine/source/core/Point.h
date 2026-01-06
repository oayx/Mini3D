 
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
class ENGINE_DLL tPoint final : public object
{
	BEGIN_REFECTION_TYPE(tPoint)
	END_FINAL_REFECTION_TYPE;
	
public:
	constexpr tPoint() : x(T(0)),y(T(0)) {}
	constexpr tPoint(T _x, T _y) : x(_x), y(_y) {}
	constexpr tPoint(const tPoint& other) : x(other.x), y(other.y) {}

public:
	tPoint& operator =(const tPoint& other) noexcept { Set(other.x, other.y); return *this; }
	constexpr bool operator ==(const tPoint &other)const noexcept { return this->Equals(other); }
	constexpr bool operator !=(const tPoint &other)const noexcept { return !this->Equals(other); }

public:
	void Set(T x, T y) noexcept { this->x = x; this->y = y; }
	constexpr bool Equals(const tPoint& target) const noexcept { return Math::FloatEqual((float)x, (float)target.x) && Math::FloatEqual((float)y, (float)target.y); }

	String ToString()const noexcept { char arr[128] = { 0 }; Snprintf(arr, sizeof(arr), "%f, %f", (float)x, (float)y); return String(arr); }

public:
	union
	{
		struct { T x, y; };
		T p[2] = { T(0) };
	};

	static const tPoint zero;
};
typedef tPoint<float> Point;
typedef tPoint<int> iPoint;
typedef tPoint<uint> uPoint;
static_assert(sizeof(Point) == 2 * sizeof(float), "invalid bytes");
static_assert(sizeof(iPoint) == 2 * sizeof(int), "invalid bytes");
static_assert(sizeof(uPoint) == 2 * sizeof(uint), "invalid bytes");

template<typename T>
const tPoint<T> tPoint<T>::zero((T)0, (T)0);

DC_END_NAMESPACE