 
/*****************************************************************************
* Author： hannibal
* Date：2009/11/13
* Description：球
*****************************************************************************/
#pragma once

#include "core/math/Vector3.h"
#include "core/math/Aabb.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
#pragma pack(push,4)
class ENGINE_DLL Sphere final : public object
{
	BEGIN_REFECTION_TYPE(Sphere)
	END_FINAL_REFECTION_TYPE;
	
public:
	constexpr Sphere(){}
	constexpr Sphere(const Vector3 &c, float r) : radius(r), center(c){}
	~Sphere(){}

public:
	void SetCenter(const Vector3 &center) noexcept {this->center = center;}
	void SetRadius(float radius) noexcept { this->radius = radius;}

	//点到球的投影，也就是最近点:Q' = Q + b,其中b = (d.len() - r)/d.len() * d,d = center - Q 注意点在球外
	Vector3 GetlatestPoint(const Vector3 &vec)const noexcept
	{
		Vector3 vecD = center - vec;
		Vector3 vecB = vecD * ((vecD.Lenght() - radius) / vecD.Lenght());
		Vector3 vec_out;
		vec_out = vec + vecB;

		return vec_out;
	}
	//点是否在里面
	bool Contain(const Vector3 &pt)const noexcept
	{
		return center.Distance(pt) <= radius;
	}
	//转aabb
	Aabb ToAabb()const noexcept
	{
		Aabb aabb;
		aabb.SetMinimum(center.x - radius, center.y - radius, center.z - radius);
		aabb.SetMaximum(center.x + radius, center.y + radius, center.z + radius);
		return aabb;
	}
	String ToString()const noexcept
	{
		char arr[128] = { 0 };
		Snprintf(arr, sizeof(arr), "radius:%f, center:%f, %f, %f", radius, center.x, center.y, center.z);
		return String(arr);
	}
public:
	float    radius = 1.0f;//半径
	Vector3  center = Vector3::zero;//圆心
};//Sphere
#pragma pack(pop)
static_assert(sizeof(Sphere) == sizeof(Vector3) + sizeof(float), "invalid bytes");
DC_END_NAMESPACE
