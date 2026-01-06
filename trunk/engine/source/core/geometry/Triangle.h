 
/*****************************************************************************
* Author： hannibal
* Date：2009/11/13
* Description：三角形
*****************************************************************************/
#pragma once

#include "core/math/Vector3.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
#pragma pack(push,4)
class ENGINE_DLL Triangle final : public object
{
	BEGIN_REFECTION_TYPE(Triangle)
	END_FINAL_REFECTION_TYPE;
	
public:
	constexpr Triangle() {}
	constexpr Triangle(const Vector3 &fir, const Vector3 &sec, const Vector3 &thr)
		: first(fir), secord(sec), third(thr) {}
	~Triangle(){}

public:
	Vector3 GetNormal()const noexcept
    {
        Vector3 n = Vector3(secord - first).Cross(Vector3(third - first));
        n.Normalize();
        return n;
    }
	String ToString()const  noexcept
	{
		char arr[256] = { 0 };
		Snprintf(arr, sizeof(arr), "first:%f, %f, %f, secord:%f, %f, %f, third:%f, %f, %f",
			first.x, first.y, first.z, secord.x, secord.y, secord.z, third.x, third.y, third.z);
		return String(arr);
	}

public:
	Vector3 first = Vector3::zero;
	Vector3 secord = Vector3::zero;
	Vector3 third = Vector3::zero;//三个点
};
#pragma pack(pop)
#if defined(_MSC_VER)
static_assert(sizeof(Triangle) == 3 * sizeof(Vector3), "invalid bytes");
#endif
DC_END_NAMESPACE
