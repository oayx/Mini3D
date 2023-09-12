 
/*****************************************************************************
* Author： hannibal
* Date：2009/11/13
* Description：三角形
*****************************************************************************/
#pragma once

#include "core/math/Vector3.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL Triangle Final : public object
{
	BEGIN_REFECTION_TYPE(Triangle)
	END_FINAL_REFECTION_TYPE;
	
public:
	Triangle() {}
	Triangle(const Vector3 &fir, const Vector3 &sec, const Vector3 &thr)
		: first(fir), secord(sec), third(thr) {}

public:
	Vector3 GetNormal();
	String ToString()const 
	{
		char arr[256] = { 0 };
		Sprintf(arr, "first:%f, %f, %f, secord:%f, %f, %f, third:%f, %f, %f", 
			first.x, first.y, first.z, secord.x, secord.y, secord.z, third.x, third.y, third.z);
		return String(arr);
	}

public:
	Vector3 first = Vector3::zero, secord = Vector3::zero, third = Vector3::zero;//三个点
};
//static_assert(sizeof(Triangle) == 3 * sizeof(Vector3), "invalid bytes");
DC_END_NAMESPACE
