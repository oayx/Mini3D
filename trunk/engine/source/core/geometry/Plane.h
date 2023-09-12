 
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
class ENGINE_DLL Plane Final : public object
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
	Plane() { }
	Plane(const Plane & plane) { normal = plane.normal; d = plane.d; }
	Plane(const Vector3& normal, float d) { this->normal = normal; this->d = -d; }
	Plane(const Vector3& normal, const Vector3& point) { this->normal = normal; this->d = -normal.Dot(point); }
	Plane (const Vector3& point0, const Vector3& point1, const Vector3& point2);

public:
	void SetPlane(const Vector3& normal, float d) { this->normal = normal; this->d = -d; }
	void SetPlane(const Vector3& point0, const Vector3& point1, const Vector3& point2);
	//点在平面那一边
	Side GetSide(const Vector3& point) const;
	Side GetSide(const Aabb &aabb)const;
	//包围盒在平面那一边，可决定平面是否可见
	bool IsBoxNegative(const Vector3* pCorners) const;
	//距离,沿(法线)正方向为正
	float GetDistance(const Vector3& point) const { return normal.Dot(point) + d; }
	//两个平面平行
	bool IsParallel(const Plane& plane) { return normal.Cross(plane.normal).IsZero(); }
	//点到平面的投影，也就是最近点:Q' = Q + (d - Q.n).n 
	Vector3 GetlatestPoint(const Vector3 &vec) { return (vec + normal * (-d - vec.Dot(normal))); }
	//点到平面的距离，有正负之分
	float GetlatestPointDistance(const Vector3 &vec) { return (d + vec.Dot(normal)); }

	String ToString()const
	{
		char arr[128] = { 0 };
		Sprintf(arr, "d:%f, normal:%f, %f, %f", d, normal.x, normal.y, normal.z);
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

inline Plane::Plane (const Vector3& point0, const Vector3& point1, const Vector3& point2)
{
	Vector3 edge1 = point1 - point0;
	Vector3 edge2 = point2 - point0;
	normal = edge1.Cross(edge2);
	normal.Normalize();
	d = -normal.Dot(point0);
}
inline void Plane::SetPlane(const Vector3& point0, const Vector3& point1, const Vector3& point2)
{
	Vector3 edge1 = point1 - point0;
	Vector3 edge2 = point2 - point0;
	normal = edge1.Cross(edge2);
	normal.Normalize();
	d = -normal.Dot(point0);
}
//static_assert(sizeof(Plane) == sizeof(Vector3) + sizeof(float), "invalid bytes");
DC_END_NAMESPACE