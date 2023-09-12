 
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
class ENGINE_DLL tRect Final : public object
{
	BEGIN_REFECTION_TYPE(tRect)
	END_FINAL_REFECTION_TYPE;
	
public:
	tRect() { Set(T(0), T(0), T(0), T(0)); }
	tRect(T _x, T _y, T _width, T _height) { Set(_x, _y, _width, _height); }
	tRect(const tRect& other) { Set(other.x, other.y, other.width, other.height); }
	tRect(const String& str)
	{
		Vector<float> vec = str.Split<float>(',');
		if (vec.Size() == 4)
		{
			x = (T)vec[0]; y = (T)vec[1]; width = (T)vec[2]; height = (T)vec[3];
		}
	}

public:
	tRect& operator =(const tRect& other) { Set(other.x, other.y, other.width, other.height); return *this; }
	bool operator ==(const tRect &other)const { return this->Equals(other); }
	bool operator !=(const tRect &other)const { return !this->Equals(other); }

public:
	void Set(T x, T y, T width, T height) { this->x = x; this->y = y; this->width = width; this->height = height; }
	T GetMinX() const { return x; }
	T GetMidX() const { return (T)(x + width / 2.0); }
	T GetMaxX() const { return (T)(x + width); }
	T GetMinY() const { return y; }
	T GetMidY() const { return (T)(y + height / 2.0); }
	T GetMaxY() const { return y + height; }
	bool Equals(const tRect& other) const { return (Math::FloatEqual((float)x, (float)other.x) && Math::FloatEqual((float)y, (float)other.y) && Math::FloatEqual((float)width, (float)other.width) && Math::FloatEqual((float)height, (float)other.height)); }
	bool Contains(const Point& point) const { return (T)point.x >= GetMinX() && (T)point.x <= GetMaxX() && (T)point.y >= GetMinY() && (T)point.y <= GetMaxY(); }
	bool Contains(const tRect& rect) const { return ((GetMinX() < rect.GetMinX() && GetMaxX() > rect.GetMaxX() && GetMinY() < rect.GetMinY() && GetMaxY() > rect.GetMaxY())); }
	bool Intersects(const tRect& rect) const { return (!(GetMaxX() < rect.GetMinX() || rect.GetMaxX() < GetMinX() || GetMaxY() < rect.GetMinY() || rect.GetMaxY() < GetMinY())); }

	String ToString()const { char arr[128] = { 0 }; Sprintf(arr, "%f, %f, %f, %f", (float)x, (float)y, (float)width, (float)height); return String(arr); }

public:
	T x = (T)0, y = (T)0, width = (T)0, height = (T)0;//xy是左上角

	static const tRect zero;
};
typedef tRect<float> Rect;
typedef tRect<int> iRect;
typedef tRect<uint> uRect;

template<typename T>
const tRect<T> tRect<T>::zero((T)0, (T)0, (T)0, (T)0);

DC_END_NAMESPACE
