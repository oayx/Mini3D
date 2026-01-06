 
/*****************************************************************************
* Author： hannibal
* Date：2009/11/13
* Description：射线，定义：P(t) = P0 + td，P0为起点，向量d为单位向量，t为长度（0～L）也可以不为单位向量，t（0～1）
*****************************************************************************/
#pragma once

#include "core/math/Vector3.h"

DC_BEGIN_NAMESPACE
class Aabb;
class Plane;
class Triangle;
/********************************************************************/
#pragma pack(push,4)
class ENGINE_DLL Ray final : public object
{
	BEGIN_REFECTION_TYPE(Ray)
	END_FINAL_REFECTION_TYPE;
	
public:
	constexpr Ray(){}//默认为从原点指向Z轴正方向的单位向量
	constexpr Ray(const Vector3 &origin, const Vector3 &dir) : origin(origin), dir(dir){}

public:
	constexpr Vector3 GetPoint(float t)const noexcept { return Vector3(origin + (dir * t)); }
	constexpr Vector3 GetOrigin()const noexcept { return origin; }
	constexpr Vector3 GetDir()const noexcept { return dir; }
	//点到射线的投影，也就是最近点:Q' = Porg + t.d,其中t = d.(Q - Porg)
	constexpr Vector3 GetlatestPoint(const Vector3 &vec)const noexcept { return GetPoint((dir.Dot(vec - origin)) / dir.SquareSize()); }

	String ToString()const noexcept
	{
		char arr[256] = { 0 };
		Snprintf(arr, sizeof(arr), "origin:%f, %f, %f,dir:%f, %f, %f", origin.x, origin.y, origin.z, dir.x, dir.y, dir.z);
		return String(arr);
	}

public:
	//射线与aabb相交
	std::pair<bool, float> Intersects(const Aabb& box)const noexcept;

	//射线与三角形相交
	std::pair<bool, float> Intersects(const Triangle& triangle, bool positiveSide = true, bool negativeSide = true)const noexcept;

	//射线与平面相交
	std::pair<bool, float> Intersects(const Plane& plane)const noexcept;

public:
	Vector3 origin = Vector3::zero;	//起点
	Vector3 dir = Vector3::forward;	//终点->长度和方向
};//Ray
#pragma pack(pop)
#if defined(_MSC_VER)
static_assert(sizeof(Ray) == 2*sizeof(Vector3), "invalid bytes");
#endif
DC_END_NAMESPACE