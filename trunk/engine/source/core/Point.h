 
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
class ENGINE_DLL tPoint Final : public object
{
	BEGIN_REFECTION_TYPE(tPoint)
	END_FINAL_REFECTION_TYPE;
	
public:
	tPoint() { Set(T(0), T(0)); }
	tPoint(T _x, T _y) { Set(_x, _y); }
	tPoint(const tPoint& other) { Set(other.x, other.y); }

public:
	tPoint& operator =(const tPoint& other) { Set(other.x, other.y); return *this; }
	bool operator ==(const tPoint &other)const { return this->Equals(other); }
	bool operator !=(const tPoint &other)const { return !this->Equals(other); }

public:
	void Set(T x, T y) { this->x = x; this->y = y; }
	bool Equals(const tPoint& target) const { return Math::FloatEqual((float)x, (float)target.x) && Math::FloatEqual((float)y, (float)target.y); }

	String ToString()const { char arr[128] = { 0 }; Sprintf(arr, "%f, %f", (float)x, (float)y); return String(arr); }

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

template<typename T>
const tPoint<T> tPoint<T>::zero((T)0, (T)0);

DC_END_NAMESPACE