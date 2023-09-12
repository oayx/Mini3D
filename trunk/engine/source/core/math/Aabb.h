 
/*****************************************************************************
* Author： hannibal
* Date：2009/11/13
* Description： 轴对齐包围盒
*****************************************************************************/
#pragma once

#include "Vector3.h"

DC_BEGIN_NAMESPACE
class Matrix4;
class Sphere;
/********************************************************************/
#pragma pack(push,4)
class ENGINE_DLL Aabb Final : public object
{
	BEGIN_REFECTION_TYPE(Aabb)
	END_FINAL_REFECTION_TYPE;
	
protected:
	enum Extent
	{
		EXTENT_NULL,
		EXTENT_FINITE,
		EXTENT_INFINITE
	};

public:
	Vector3   _minimum = Vector3::zero;
	Vector3   _maximum = Vector3::zero;
	Extent    _extent = Extent::EXTENT_NULL;

public:
	/*
	1-----2
	/|    /|
	/ |   / |
	5-----4  |
	|  0--|--3
	| /   | /
	|/    |/
	6-----7
	*/
	typedef enum
	{
		FAR_LEFT_BOTTOM    = 0,
		FAR_LEFT_TOP       = 1,
		FAR_RIGHT_TOP      = 2,
		FAR_RIGHT_BOTTOM   = 3,
		NEAR_RIGHT_BOTTOM  = 7,
		NEAR_LEFT_BOTTOM   = 6,
		NEAR_LEFT_TOP      = 5,
		NEAR_RIGHT_TOP     = 4
	}CornerEnum;

public:
	Aabb() { SetMinimum(0.0f, 0.0f, 0.0f); SetMaximum(0.0f, 0.0f, 0.0f); _extent = EXTENT_FINITE; }
	Aabb(const Aabb &aabb);
	Aabb(const Vector3& min, const Vector3& max) { SetExtents(min, max); }

public:
	bool operator== (const Aabb& rhs) const;
	bool operator!= (const Aabb& rhs) const;
	Aabb operator *(const Matrix4& mat)const;
	Aabb operator *(const Vector3& vec)const;

public:
	const Vector3& GetMinimum() const { return _minimum; }
	const Vector3& GetMaximum() const { return _maximum; }

	void SetMinimum(const Vector3& vec) { _extent = EXTENT_FINITE; _minimum = vec; }
	void SetMinimum(float x, float y, float z) { _extent = EXTENT_FINITE; _minimum.x = x; _minimum.y = y; _minimum.z = z; }
	void SetMaximum(const Vector3& vec) { _extent = EXTENT_FINITE; _maximum = vec; }
	void SetMaximum(float x, float y, float z) { _extent = EXTENT_FINITE; _maximum.x = x; _maximum.y = y; _maximum.z = z; }

	//构造AABB
	void SetExtents(const Vector3& min, const Vector3& max) { _extent = EXTENT_FINITE; _minimum = min; _maximum = max; }

	//不区分大小
	void SetAbsExtents(const Vector3& min, const Vector3& max);

	//获得八个点
	void GetCorners(Vector3* corners)const;
	Vector3 GetCorner(CornerEnum cornerToGet) const;

	bool IsNull() const { return (_extent == EXTENT_NULL); }
	void SetNULL() { _maximum.SetZero(); _minimum.SetZero(); _extent = EXTENT_NULL; }
	bool IsFinite() const { return (_extent == EXTENT_FINITE); }
	void SetInfinite() { _extent = EXTENT_INFINITE; }
	bool IsInfinite() const { return (_extent == EXTENT_INFINITE); }

	//缩放
	void Scale(const Vector3& s);

	//取得中点
	Vector3 GetCenter() const { return Vector3((_maximum.x + _minimum.x) * 0.5f, (_maximum.y + _minimum.y) * 0.5f, (_maximum.z + _minimum.z) * 0.5f); }
	Vector3 GetSize() const;
	Vector3 GetHalfSize()const;

	//合并
	void Merge(const Vector3 &vec);
	void Merge(const Aabb &aabb);

	//给定的点是否包含在AABB内
	bool Contains(const Vector3& v) const;
	bool Contains(const Aabb& aabb) const;

	//两个AABB相交测试
	IntersectResult Intersects(const Aabb &aabb)const;
	//与球相交测试
	IntersectResult Intersects(const Sphere &sphere)const;

	String ToString()const 
	{ 
		char arr[256] = { 0 }; 
		Sprintf(arr, "min:%f, %f, %f, max:%f, %f, %f", _minimum.x, _minimum.y, _minimum.z, _maximum.x, _maximum.y, _maximum.z);
		return String(arr); 
	}
};
#pragma pack(pop)
DC_END_NAMESPACE
