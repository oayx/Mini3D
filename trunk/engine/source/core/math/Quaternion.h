 
/*****************************************************************************
* Author： hannibal
* Date：2009/11/13
* Description： 四元素
*****************************************************************************/
#pragma once

#include "Vector3.h"
#include "Math.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
#pragma pack(push,4)
class ENGINE_DLL Quaternion final : public object
{
	BEGIN_REFECTION_TYPE(Quaternion)
	END_FINAL_REFECTION_TYPE;

public:
	float x = 0.0f, y = 0.0f, z = 0.0f, w = 0.0f;
	static Quaternion identity;

public:
	constexpr Quaternion(){}
	constexpr Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
	Quaternion(float degree, const Vector3& axis) { FromRadianAxis(degree, axis); }
	Quaternion(const Vector3& euler) { FromEuler(euler); }
	constexpr Quaternion(const Quaternion& q) : x(q.x), y(q.y), z(q.z), w(q.w) {}
	constexpr Quaternion(Quaternion&& other)noexcept = default;
	Quaternion(const String& str)
	{
		Vector<float> vec = str.Split<float>(',');
		if (vec.Size() == 4)
		{
			x = vec[0]; y = vec[1]; z = vec[2]; w = vec[3];
		}
	}
public:
	Quaternion& operator = (const Quaternion& q) noexcept { w = q.w; x = q.x; y = q.y; z = q.z; return *this; }
	Quaternion& operator = (Quaternion&& other) noexcept { x = std::move(other.x); y = std::move(other.y); z = std::move(other.z); w = std::move(other.w); return *this; }

	Quaternion operator +=(const Quaternion &q) noexcept { x += q.x; y += q.y; z += q.z; w += q.w; return Normalize(); }
	Quaternion operator -= (const Quaternion &q) noexcept { x -= q.x; y -= q.y; z -= q.z; w -= q.w; return Normalize(); }
	Quaternion operator *= (const Quaternion &q) noexcept;
	Quaternion operator *= (const Vector3& v) noexcept;
	Quaternion operator *= (float f) noexcept { x *= f; y *= f; z *= f; w *= f; return *this; }
	Quaternion operator /= (float f) noexcept { x /= f; y /= f; z /= f; w /= f; return *this; }

	constexpr Quaternion operator + () const noexcept { return Quaternion(+x, +y, +z, +w); }
	constexpr Quaternion operator - () const noexcept { return Quaternion(-x, -y, -z, -w); }

	constexpr Quaternion operator + (const Quaternion& q) const noexcept { return Quaternion(x + q.x, y + q.y, z + q.z, w + q.w); }
	constexpr Quaternion operator - (const Quaternion& q) const noexcept { return Quaternion(x - q.x, y - q.y, z - q.z, w - q.w); }
	constexpr Quaternion operator * (float scale) const noexcept { return Quaternion(scale*x, scale*y, scale*z, scale*w); }
	constexpr Quaternion operator / (float scale) const noexcept { return Quaternion(x / scale, y / scale, z / scale, w / scale); }

	constexpr Quaternion operator * (const Quaternion& q) const noexcept {
		return Quaternion(
			w * q.x + x * q.w + y * q.z - z * q.y,
			w * q.y + y * q.w + z * q.x - x * q.z,
			w * q.z + z * q.w + x * q.y - y * q.x,
			w * q.w - x * q.x - y * q.y - z * q.z);
	}

	constexpr bool operator == (const Quaternion& q) const noexcept {return false;}

	Vector3 operator *(const Vector3 &vec)const noexcept;
	friend Quaternion operator* (float scale, const Quaternion& rkQ) noexcept;

public:
	//归一化
	Quaternion Normalize() noexcept;
	Quaternion Normalize()const noexcept { Quaternion q = *this; return q.Normalize(); }
	//点积
	float Dot(const Quaternion& q)const noexcept { return w * q.w + x * q.x + y * q.y + z * q.z; }
	//逆矩阵，方法是共轭除以模
	Quaternion Inverse()const noexcept;
	//共轭
	Quaternion UnitInverse()const noexcept { return Quaternion(-x, -y, -z, w); }
	Quaternion &SetIdentity() noexcept { x = y = z = 0.0f; w = 1.0f; return *this; }

	//欧拉角
	Quaternion FromEuler(const Vector3& euler) noexcept;
	Vector3 ToEuler()const noexcept;

	//旋转轴
	Quaternion FromRadianAxis(float degree, const Vector3 &axis) noexcept;
	void ToRadianAxis(float &degree, Vector3 &axis)const noexcept;

	//轴
	Quaternion FromAxis(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis) noexcept;
	void ToAxis(Vector3& xAxis, Vector3& yAxis, Vector3& zAxis)const noexcept;

	String ToString()const noexcept { char arr[128] = { 0 }; Snprintf(arr, sizeof(arr), "%f, %f, %f, %f", x, y, z, w); return String(arr); }

	// spherical linear interpolation
	static Quaternion Slerp (const Quaternion& q1, const Quaternion& q2, float dt) noexcept;
	// normalised linear interpolation - faster but less accurate (non-constant rotation velocity)
	static Quaternion Lerp(const Quaternion& q1, const Quaternion& q2, float dt) noexcept;
};
#pragma pack(pop)
static_assert(sizeof(Quaternion) == 4 * sizeof(float), "invalid bytes");
DC_END_NAMESPACE
