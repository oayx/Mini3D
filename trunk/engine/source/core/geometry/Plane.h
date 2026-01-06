 
/*****************************************************************************
* Author： hannibal
* Date：2009/11/13
* Description：平面
*****************************************************************************/
#pragma once

#include "core/math/Vector3.h"

DC_BEGIN_NAMESPACE
class Aabb;
/********************************************************************/
#pragma pack(push,4)
class ENGINE_DLL Plane final : public object
{
	BEGIN_REFECTION_TYPE(Plane)
	END_FINAL_REFECTION_TYPE;

public:
	enum Side
	{
		NO_SIDE = 0,       // 经过平面
		POSITIVE_SIDE,     // plane normal points space
		NEGATIVE_SIDE      // negative normal points space  
	};

public:
	constexpr Plane() { }
	constexpr Plane(const Plane & plane) : normal(plane.normal), d(plane.d){}
	constexpr Plane(const Vector3& normal, float d) : normal(normal), d(-d) {}
	constexpr Plane(const Vector3& normal, const Vector3& point) : normal(normal), d(-normal.Dot(point)) {}
	Plane (const Vector3& point0, const Vector3& point1, const Vector3& point2);

public:
	void SetPlane(const Vector3& normal, float d) noexcept { this->normal = normal; this->d = -d; }
	void SetPlane(const Vector3& point0, const Vector3& point1, const Vector3& point2) noexcept;
	//点在平面那一边
	Side GetSide(const Vector3& point) const noexcept;
	Side GetSide(const Aabb &aabb)const noexcept;
	//包围盒在平面那一边，可决定平面是否可见
	bool IsBoxNegative(const Vector3* pCorners) const noexcept;
	//距离,沿(法线)正方向为正
	constexpr float GetDistance(const Vector3& point) const noexcept { return normal.Dot(point) + d; }
	//两个平面平行
	constexpr bool IsParallel(const Plane& plane) noexcept { return normal.Cross(plane.normal).IsZero(); }
	//点到平面的投影，也就是最近点:Q' = Q + (d - Q.n).n 
	constexpr Vector3 GetlatestPoint(const Vector3 &vec) noexcept { return (vec + normal * (-d - vec.Dot(normal))); }
	//点到平面的距离，有正负之分
	constexpr float GetlatestPointDistance(const Vector3 &vec) noexcept { return (d + vec.Dot(normal)); }

	String ToString()const noexcept
	{
		char arr[128] = { 0 };
		Snprintf(arr, sizeof(arr), "d:%f, normal:%f, %f, %f", d, normal.x, normal.y, normal.z);
		return String(arr);
	}

public:
	Vector3 normal = Vector3::zero;
	/*
	沿法线正方向移动到原点的距离，已经变负了，如果沿法线正方向移动的话设置时需为负。
	^Y
	|
	| (d =-10)|(创建时d = 10)
	|    <----|---->
	->X
	|         |
	|         |
	|
	*/
	float d = 0.0f;
};//Plane
#pragma pack(pop)
#if defined(_MSC_VER)
static_assert(sizeof(Plane) == sizeof(Vector3) + sizeof(float), "invalid bytes");
#endif

inline Plane::Plane (const Vector3& point0, const Vector3& point1, const Vector3& point2)
{
	Vector3 edge1 = point1 - point0;
	Vector3 edge2 = point2 - point0;
	normal = edge1.Cross(edge2);
	normal.Normalize();
	d = -normal.Dot(point0);
}
inline void Plane::SetPlane(const Vector3& point0, const Vector3& point1, const Vector3& point2) noexcept
{
	Vector3 edge1 = point1 - point0;
	Vector3 edge2 = point2 - point0;
	normal = edge1.Cross(edge2);
	normal.Normalize();
	d = -normal.Dot(point0);
}
DC_END_NAMESPACE