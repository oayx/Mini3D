 
/*****************************************************************************
* Author： hannibal
* Date：2009/11/13
* Description： 3×3矩阵
*****************************************************************************/
#pragma once

#include "Vector3.h"
#include "Quaternion.h"

DC_BEGIN_NAMESPACE
class Matrix4;
/********************************************************************/
#pragma pack(push,4)
// 3×3矩阵
class ENGINE_DLL Matrix3 final : public object
{
	BEGIN_REFECTION_TYPE(Matrix3)
	END_FINAL_REFECTION_TYPE;

public:
	union
	{
		struct
		{
			float _11, _12, _13;
			float _21, _22, _23;
			float _31, _32, _33;
		};
		float m[3][3] = { 0.0f };
		float p[3 * 3];
	};

public:
	static Matrix3 zero;
	static Matrix3 identity;

public:
	Matrix3() { memset(p, 0, sizeof(p)); }
	Matrix3(const Vector3& euler) { Euler(euler); }
	Matrix3(const Vector3& axis, float degree) { Rotate(axis, degree); }
	Matrix3(const Quaternion& q) { Rotate(q); }
	Matrix3(const Matrix3& mat)
	{
		m[0][0] = mat[0][0]; m[0][1] = mat[0][1]; m[0][2] = mat[0][2];
		m[1][0] = mat[1][0]; m[1][1] = mat[1][1]; m[1][2] = mat[1][2];
		m[2][0] = mat[2][0]; m[2][1] = mat[2][1]; m[2][2] = mat[2][2];
	}
	Matrix3(const Matrix4& mat);
	constexpr Matrix3(Matrix3&& other)noexcept = default;
	constexpr Matrix3(
		float m00, float m01, float m02,
		float m10, float m11, float m12,
		float m20, float m21, float m22): 
		_11(m00), _12(m01), _13(m02),
		_21(m10), _22(m11), _23(m12),
		_31(m20), _32(m21), _33(m22){}

public:
	//赋值，改变原始值
	Matrix3& operator =(const Matrix3& mat) noexcept;
	Matrix3& operator =(Matrix3&& mat) noexcept;
	Matrix3& operator =(const Matrix4 &mat) noexcept;
 
	//求负值，不改变原值
	constexpr Matrix3 operator -()const noexcept
	{
		return Matrix3(
			-m[0][0], -m[0][1], -m[0][2],
			-m[1][0], -m[1][1], -m[1][2],
			-m[2][0], -m[2][1], -m[2][2]
		);
	}

	//重载括号运算符
	constexpr float *operator [](int index) noexcept { return m[index]; }
	constexpr const float *operator [](int index)const noexcept { return m[index]; }

	//重载算术运算符
	Matrix3& operator *(float f) noexcept;
	Matrix3 operator * (const Matrix3 &mat)const noexcept;
	Matrix3 operator + (const Matrix3 &mat) const noexcept;
	Matrix3 operator - (const Matrix3 &mat)const noexcept;
	Matrix3 operator / (float f)const noexcept;
 
	//重载关系运算符
	constexpr bool operator == (const Matrix3& mat) const noexcept
	{
		if (m[0][0] != mat.m[0][0] || m[0][1] != mat.m[0][1] || m[0][2] != mat.m[0][2] ||
			m[1][0] != mat.m[1][0] || m[1][1] != mat.m[1][1] || m[1][2] != mat.m[1][2] ||
			m[2][0] != mat.m[2][0] || m[2][1] != mat.m[2][1] || m[2][2] != mat.m[2][2])
			return false;
		return true;
	}
	constexpr bool operator != (Matrix3& mat) const noexcept { return !(*this == mat); }

	//向量乘矩阵
	constexpr friend Vector3 operator * (const Vector3& v, const Matrix3& mat) noexcept
	{
		return Vector3(
			v.x*mat[0][0] + v.y*mat[1][0] + v.z*mat[2][0],
			v.x*mat[0][1] + v.y*mat[1][1] + v.z*mat[2][1],
			v.x*mat[0][2] + v.y*mat[1][2] + v.z*mat[2][2]
		);
	}
public:
	//- Description：单位化
	Matrix3&	Identity() noexcept { memset(p, 0, sizeof(p)); _11 = 1.0f; _22 = 1.0f; _33 = 1.0f; return *this; }

	//从0开始
	float		Get(int row, int col)const noexcept { return p[row * 3 + col]; }
	void		Set(int row, int col, float v) noexcept { p[row * 3 + col] = v; }

	//- Description：缩放
	Matrix3&	Scale(float x, float y, float z) noexcept { _11 = x; _22 = y; _33 = z; return *this; }
	Matrix3&	Scale(const Vector3 &vec) noexcept { _11 = vec.x; _22 = vec.y; _33 = vec.z; return *this;}
	Vector3		GetScale()const noexcept {return Vector3(_11,_22,_33);}

	//求矩阵的逆: = 伴随矩阵除以行列式 
	Matrix3		Inverse()const noexcept;

	//转置矩阵
	Matrix3		Transpose()const noexcept;

	//矩阵将沿通过原点且垂直于n的平面进行镜像变换
	Matrix3		Reflect(const Vector3& normal)const noexcept;

	Matrix3&	Rotate(const Quaternion& q) noexcept;
	Matrix3&	Rotate(const Vector3 &axis, float degree) noexcept;
	Quaternion	ToRotate()const noexcept;

	Matrix3&	Euler(const Vector3& euler) noexcept;
	Vector3		ToEuler()const noexcept;

	static Matrix3 FromRotate(const Quaternion& q) noexcept;
	static Matrix3 FromScale(const Vector3& scale) noexcept;

	static Matrix3 LookTo(const Vector3 &dir, const Vector3 &up) noexcept;
	static Matrix3 LookAt(const Vector3 &eye, const Vector3 &at, const Vector3 &up) noexcept;
};
#pragma pack(pop)
static_assert(sizeof(Matrix3) == 9 * sizeof(float), "invalid bytes");
DC_END_NAMESPACE
