 
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
class ENGINE_DLL Aabb final : public object
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
	Aabb(Aabb&& other)noexcept = default;

public:
	Aabb& operator = (const Aabb& other) noexcept { _minimum = other._minimum; _maximum = other._maximum; _extent = other._extent; return *this; }
	Aabb& operator = (Aabb&& other) noexcept { _minimum = std::move(other._minimum); _maximum = std::move(other._maximum); _extent = other._extent; return *this; }

	bool operator== (const Aabb& rhs) const noexcept;
	bool operator!= (const Aabb& rhs) const noexcept;
	Aabb operator *(const Matrix4& mat)const noexcept;
	Aabb operator *(const Vector3& vec)const noexcept;

public:
	constexpr const Vector3& GetMinimum() const noexcept { return _minimum; }
	constexpr const Vector3& GetMaximum() const noexcept { return _maximum; }

	void SetMinimum(const Vector3& vec) noexcept { _extent = EXTENT_FINITE; _minimum = vec; }
	void SetMinimum(float x, float y, float z) noexcept { _extent = EXTENT_FINITE; _minimum.x = x; _minimum.y = y; _minimum.z = z; }
	void SetMaximum(const Vector3& vec) noexcept { _extent = EXTENT_FINITE; _maximum = vec; }
	void SetMaximum(float x, float y, float z) noexcept { _extent = EXTENT_FINITE; _maximum.x = x; _maximum.y = y; _maximum.z = z; }

	//构造AABB
	void SetExtents(const Vector3& min, const Vector3& max) noexcept { _extent = EXTENT_FINITE; _minimum = min; _maximum = max; }

	//不区分大小
	void SetAbsExtents(const Vector3& min, const Vector3& max) noexcept;

	//获得八个点
	void GetCorners(Vector3* corners)const noexcept;
	Vector3 GetCorner(CornerEnum cornerToGet) const noexcept;

	constexpr bool IsNull() const noexcept { return (_extent == EXTENT_NULL); }
	void SetNULL() noexcept { _maximum.SetZero(); _minimum.SetZero(); _extent = EXTENT_NULL; }
	constexpr bool IsFinite() const noexcept { return (_extent == EXTENT_FINITE); }
	void SetInfinite() noexcept { _extent = EXTENT_INFINITE; }
	constexpr bool IsInfinite() const noexcept { return (_extent == EXTENT_INFINITE); }

	//缩放
	void Scale(const Vector3& s) noexcept;

	//取得中点
	constexpr Vector3 GetCenter() const noexcept { return Vector3((_maximum.x + _minimum.x) * 0.5f, (_maximum.y + _minimum.y) * 0.5f, (_maximum.z + _minimum.z) * 0.5f); }
	constexpr Vector3 GetSize() const noexcept
	{
		switch (_extent)
		{
		case EXTENT_NULL: return Vector3::zero;
		case EXTENT_FINITE: return _maximum - _minimum;
		case EXTENT_INFINITE: return Vector3::infinity;
		default: return Vector3::zero;
		}
	}
	constexpr Vector3 GetHalfSize()const noexcept
	{
		switch (_extent)
		{
		case EXTENT_NULL: return Vector3::zero;
		case EXTENT_FINITE: return (_maximum - _minimum) * 0.5f;
		case EXTENT_INFINITE: return Vector3::infinity;
		default:return Vector3::zero;
		}
	}

	//合并
	void Merge(const Vector3 &vec) noexcept;
	void Merge(const Aabb &aabb) noexcept;

	//给定的点是否包含在AABB内
	bool Contains(const Vector3& v) const noexcept;
	bool Contains(const Aabb& aabb) const noexcept;

	//两个AABB相交测试
	IntersectResult Intersects(const Aabb &aabb)const noexcept;
	//与球相交测试
	IntersectResult Intersects(const Sphere &sphere)const noexcept;

	String ToString()const 
	{ 
		char arr[256] = { 0 }; 
		Snprintf(arr, sizeof(arr), "min:%f, %f, %f, max:%f, %f, %f", _minimum.x, _minimum.y, _minimum.z, _maximum.x, _maximum.y, _maximum.z);
		return String(arr); 
	}
};
#pragma pack(pop)
DC_END_NAMESPACE
