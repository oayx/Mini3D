 
/*****************************************************************************************************/
// @author hannibal
// @date   2015/04/21
// @brief  矩形
/*****************************************************************************************************/
#pragma once

#include "Size.h"
#include "Point.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
template<typename T>
class ENGINE_DLL tRect final : public object
{
	BEGIN_REFECTION_TYPE(tRect)
	END_FINAL_REFECTION_TYPE;
	
public:
	constexpr tRect() : x(T(0)), y(T(0)), width(T(0)), height(T(0)) {}
	constexpr tRect(T _x, T _y, T _width, T _height) : x(_x), y(_y), width(_width), height(_height) {}
	constexpr tRect(const tRect& other) : x(other.x), y(other.y), width(other.width), height(other.height) {}
	tRect(const String& str)
	{
		Vector<float> vec = str.Split<float>(',');
		if (vec.Size() == 4)
		{
			x = (T)vec[0]; y = (T)vec[1]; width = (T)vec[2]; height = (T)vec[3];
		}
	}

public:
	tRect& operator =(const tRect& other) noexcept { Set(other.x, other.y, other.width, other.height); return *this; }
	constexpr bool operator ==(const tRect &other)const noexcept { return this->Equals(other); }
	constexpr bool operator !=(const tRect &other)const noexcept { return !this->Equals(other); }

public:
	void Set(T x, T y, T width, T height) noexcept { this->x = x; this->y = y; this->width = width; this->height = height; }
	constexpr T GetMinX() const noexcept { return x; }
	constexpr T GetMidX() const noexcept { return (T)(x + width / 2.0); }
	constexpr T GetMaxX() const noexcept { return (T)(x + width); }
	constexpr T GetMinY() const noexcept { return y; }
	constexpr T GetMidY() const noexcept { return (T)(y + height / 2.0); }
	constexpr T GetMaxY() const noexcept { return y + height; }
	constexpr bool Equals(const tRect& other) const noexcept { return (Math::FloatEqual((float)x, (float)other.x) && Math::FloatEqual((float)y, (float)other.y) && Math::FloatEqual((float)width, (float)other.width) && Math::FloatEqual((float)height, (float)other.height)); }
	constexpr bool Contains(const Point& point) const noexcept { return (T)point.x >= GetMinX() && (T)point.x <= GetMaxX() && (T)point.y >= GetMinY() && (T)point.y <= GetMaxY(); }
	constexpr bool Contains(const tRect& rect) const noexcept { return ((GetMinX() < rect.GetMinX() && GetMaxX() > rect.GetMaxX() && GetMinY() < rect.GetMinY() && GetMaxY() > rect.GetMaxY())); }
	constexpr bool Intersects(const tRect& rect) const noexcept { return (!(GetMaxX() < rect.GetMinX() || rect.GetMaxX() < GetMinX() || GetMaxY() < rect.GetMinY() || rect.GetMaxY() < GetMinY())); }

	String ToString()const noexcept { char arr[128] = { 0 }; Snprintf(arr, sizeof(arr), "%f, %f, %f, %f", (float)x, (float)y, (float)width, (float)height); return String(arr); }

public:
	T x = (T)0, y = (T)0, width = (T)0, height = (T)0;//xy是左上角

	static const tRect zero;
};
typedef tRect<float> Rect;
typedef tRect<int> iRect;
typedef tRect<uint> uRect;
static_assert(sizeof(Rect) == 4 * sizeof(float), "invalid bytes");
static_assert(sizeof(iRect) == 4 * sizeof(int), "invalid bytes");
static_assert(sizeof(uRect) == 4 * sizeof(uint), "invalid bytes");

template<typename T>
const tRect<T> tRect<T>::zero((T)0, (T)0, (T)0, (T)0);

DC_END_NAMESPACE
