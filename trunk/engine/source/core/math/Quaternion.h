 
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
class ENGINE_DLL Quaternion Final : public object
{
	BEGIN_REFECTION_TYPE(Quaternion)
	END_FINAL_REFECTION_TYPE;

public:
	float x = 0.0f, y = 0.0f, z = 0.0f, w = 0.0f;
	static Quaternion identity;

public:
	Quaternion(){}
	Quaternion(float x, float y, float z, float w) { this->x = x; this->y = y; this->z = z; this->w = w; }
	Quaternion(float degree, const Vector3& axis) { FromRadianAxis(degree, axis); }
	Quaternion(const Vector3& euler) { FromEuler(euler); }
	Quaternion(const Quaternion &q) { x = q.x; y = q.y; z = q.z; w = q.w; }
	Quaternion(const String& str)
	{
		Vector<float> vec = str.Split<float>(',');
		if (vec.Size() == 4)
		{
			x = vec[0]; y = vec[1]; z = vec[2]; w = vec[3];
		}
	}
public:
	Quaternion operator +=(const Quaternion &q) { x += q.x; y += q.y; z += q.z; w += q.w; return Normalize(); }
	Quaternion operator -= (const Quaternion &q) { x -= q.x; y -= q.y; z -= q.z; w -= q.w; return Normalize(); }
	Quaternion operator *= (const Quaternion &q);
	Quaternion operator *= (const Vector3& v);
	Quaternion operator *= (float f) { x *= f; y *= f; z *= f; w *= f; return *this; }
	Quaternion operator /= (float f) { x /= f; y /= f; z /= f; w /= f; return *this; }

	Quaternion operator + () const { return Quaternion(+x, +y, +z, +w); }
	Quaternion operator - () const { return Quaternion(-x, -y, -z, -w); }

	Quaternion &operator = (const Quaternion& q) { w = q.w; x = q.x; y = q.y; z = q.z; return *this; }

	Quaternion	operator + (const Quaternion& q) const { return Quaternion(x + q.x, y + q.y, z + q.z, w + q.w); }
	Quaternion	operator - (const Quaternion& q) const { return Quaternion(x - q.x, y - q.y, z - q.z, w - q.w); }
	Quaternion	operator * (const Quaternion& q) const;//非标准的定义
	Quaternion	operator * (float scale) const { return Quaternion(scale*x, scale*y, scale*z, scale*w); }
	Quaternion  operator / (float scale) const { return Quaternion(x / scale, y / scale, z / scale, w / scale); }
	bool operator == (const Quaternion& q) const{return false;}

	Vector3 operator *(const Vector3 &vec)const;
	friend Quaternion operator* (float scale, const Quaternion& rkQ);

public:
	//归一化
	Quaternion Normalize();
	Quaternion Normalize()const { Quaternion q = *this; return q.Normalize(); }
	//点积
	float Dot(const Quaternion& q)const { return w * q.w + x * q.x + y * q.y + z * q.z; }
	//逆矩阵，方法是共轭除以模
	Quaternion Inverse()const;
	//共轭
	Quaternion UnitInverse()const { return Quaternion(-x, -y, -z, w); }
	Quaternion &SetIdentity() { x = y = z = 0.0f; w = 1.0f; return *this; }

	//欧拉角
	Quaternion FromEuler(const Vector3& euler);
	Vector3 ToEuler()const;

	//旋转轴
	Quaternion FromRadianAxis(float degree, const Vector3 &axis);
	void ToRadianAxis(float &degree, Vector3 &axis)const;

	//轴
	Quaternion FromAxis(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis);
	void ToAxis(Vector3& xAxis, Vector3& yAxis, Vector3& zAxis)const;

	String ToString()const { char arr[128] = { 0 }; Sprintf(arr, "%f, %f, %f, %f", x, y, z, w); return String(arr); }

	// spherical linear interpolation
	static Quaternion Slerp (const Quaternion& q1, const Quaternion& q2, float dt);
	// normalised linear interpolation - faster but less accurate (non-constant rotation velocity)
	static Quaternion Lerp(const Quaternion& q1, const Quaternion& q2, float dt);
};
#pragma pack(pop)
static_assert(sizeof(Quaternion) == 4 * sizeof(float), "invalid bytes");
DC_END_NAMESPACE
