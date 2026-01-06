 
/*****************************************************************************
* Author： hannibal
* Date：2009/11/13
* Description： 4×4矩阵
*****************************************************************************/
#pragma once

#include <cassert>
#include "Vector3.h"
#include "Vector4.h"

DC_BEGIN_NAMESPACE
class Quaternion;
class Matrix3;
/********************************************************************/
#pragma pack(push,4)
// 4×4矩阵
class ENGINE_DLL Matrix4 final : public object
{
	BEGIN_REFECTION_TYPE(Matrix4)
	END_FINAL_REFECTION_TYPE;

public:
	union
	{
		struct
		{
			float _11, _12, _13, _14;
			float _21, _22, _23, _24;
			float _31, _32, _33, _34;
			float _41, _42, _43, _44;
		};
		float m[4][4];
		float p[4 * 4];
	};

public:
	const static Matrix4 zero;
	const static Matrix4 identity;

public:
	Matrix4()
	{
		memset(p, 0, sizeof(p)); m[0][0] = 1; m[1][1] = 1; m[2][2] = 1; m[3][3] = 1;
	}
	Matrix4(const float* arr, int size)
	{
		AssertEx(size == 16, ""); ::memcpy(p, arr, 16 * sizeof(float));
	}
	Matrix4(const Matrix4& mat)
	{
		m[0][0] = mat[0][0]; m[0][1] = mat[0][1]; m[0][2] = mat[0][2]; m[0][3] = mat[0][3];
		m[1][0] = mat[1][0]; m[1][1] = mat[1][1]; m[1][2] = mat[1][2]; m[1][3] = mat[1][3];
		m[2][0] = mat[2][0]; m[2][1] = mat[2][1]; m[2][2] = mat[2][2]; m[2][3] = mat[2][3];
		m[3][0] = mat[3][0]; m[3][1] = mat[3][1]; m[3][2] = mat[3][2]; m[3][3] = mat[3][3];
	}
	Matrix4(const Matrix3& mat);
	constexpr Matrix4(Matrix4&& other)noexcept = default;
	Matrix4(const Vector3& position, const Quaternion& rotation, const Vector3& scale)
	{
		Recompose(position, rotation, scale);
	}
	constexpr Matrix4(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33): 
		_11(m00), _12(m01), _13(m02), _14(m03),
		_21(m10), _22(m11), _23(m12), _24(m13),
		_31(m20), _32(m21), _33(m22), _34(m23),
		_41(m30), _42(m31), _43(m32), _44(m33)
	{
	}

public:
	//赋值，改变原始值
	Matrix4& operator =(const Matrix4& other) noexcept;
	Matrix4& operator =(Matrix4&& other) noexcept;
	void operator = (const Matrix3& other) noexcept;
	
	//求负值，不改变原值
	constexpr Matrix4 operator -()const noexcept
	{
		return Matrix4(-m[0][0], -m[0][1], -m[0][2], -m[0][3],
			-m[1][0], -m[1][1], -m[1][2], -m[1][3],
			-m[2][0], -m[2][1], -m[2][2], -m[2][3],
			-m[3][0], -m[3][1], -m[3][2], -m[3][3]);
	}

	//重载括号运算符
	constexpr float *operator [](int index) noexcept { return m[index]; }
	constexpr const float *operator [](int index)const noexcept { return m[index]; }

	//重载算术运算符
	Matrix4& operator *(float f) noexcept;
	Matrix4 operator * (const Matrix4 &mat)const noexcept;
	Matrix4 operator + (const Matrix4 &mat) const noexcept;
	Matrix4 operator - (const Matrix4 &mat)const noexcept;

	//重载关系运算符
	constexpr bool operator == (const Matrix4& mat) const noexcept
	{
		if (m[0][0] != mat.m[0][0] || m[0][1] != mat.m[0][1] || m[0][2] != mat.m[0][2] || m[0][3] != mat.m[0][3] ||
			m[1][0] != mat.m[1][0] || m[1][1] != mat.m[1][1] || m[1][2] != mat.m[1][2] || m[1][3] != mat.m[1][3] ||
			m[2][0] != mat.m[2][0] || m[2][1] != mat.m[2][1] || m[2][2] != mat.m[2][2] || m[2][3] != mat.m[2][3] ||
			m[3][0] != mat.m[3][0] || m[3][1] != mat.m[3][1] || m[3][2] != mat.m[3][2] || m[3][3] != mat.m[3][3])
			return false;
		return true;
	}
	constexpr bool operator != (Matrix4& mat) const noexcept { return !(*this == mat); }

	constexpr friend Vector4 operator * (const Vector4& v, const Matrix4& mat) noexcept
	{
		return Vector4(
			v.x*mat[0][0] + v.y*mat[1][0] + v.z*mat[2][0] + v.w*mat[3][0],
			v.x*mat[0][1] + v.y*mat[1][1] + v.z*mat[2][1] + v.w*mat[3][1],
			v.x*mat[0][2] + v.y*mat[1][2] + v.z*mat[2][2] + v.w*mat[3][2],
			v.x*mat[0][3] + v.y*mat[1][3] + v.z*mat[2][3] + v.w*mat[3][3]
		);
	}
public:
	//单位化
	Matrix4& Identity() noexcept { memset(p, 0, sizeof(p)); _11 = 1.0f; _22 = 1.0f; _33 = 1.0f; _44 = 1.0f; return *this; }
	//从0开始
	float	 Get(int row, int col)const noexcept { return p[row * 4 + col]; }
	void	 Set(int row, int col, float v) noexcept { p[row * 4 + col] = v; }

	//缩放
	Matrix4& Scale(float x, float y, float z) noexcept { _11 = x; _22 = y; _33 = z; return *this; }
	Matrix4& Scale(const Vector3 &vec3) noexcept { _11 = vec3.x; _22 = vec3.y; _33 = vec3.z; return *this; }

	//平移矩阵
	Matrix4& Translate(float x, float y, float z) noexcept { _41 = x; _42 = y; _43 = z; return *this; }
	Matrix4& Translate(const Vector3 &vec3) noexcept { _41 = vec3.x; _42 = vec3.y; _43 = vec3.z;return *this;  }
	Vector3  GetTranslate()const noexcept { return Vector3(_41, _42, _43); }

	//从旋转轴构建(位置不变)
	Quaternion GetRotate()const noexcept;

	//求矩阵的逆
	Matrix4 Inverse()const noexcept;
	//转置矩阵
	Matrix4 Transpose()const noexcept;

	//世界矩阵的逆的转置仅针对法向量，我们也不需要世界矩阵的平移分量
	//而且不去掉的话，后续再乘上观察矩阵之类的就会产生错误的变换结果
	Matrix4 InverseTranspose()const noexcept;

	Vector3 TransformVector(const Vector3& v) const noexcept;
	Vector3 TransformPoint(const Vector3& v) const noexcept;

	//矩阵分解
	void Decompose(Vector3& position, Quaternion& rotation, Vector3& scale) noexcept;
	void Recompose(const Vector3& position, const Quaternion& rotation, const Vector3& scale) noexcept;

	static Matrix4 FromTranslate(const Vector3& position) noexcept;
	static Matrix4 FromRotate(const Quaternion& rotation) noexcept;
	static Matrix4 FromRotate(const Vector3 &axis, float degree) noexcept;
	static Matrix4 FromScale(const Vector3& scale) noexcept;

	static Matrix4 LookAt(const Vector3 &pos, const Vector3 &at, const Vector3 &up) noexcept;
	static Matrix4 LookTo(const Vector3 &pos, const Vector3 &dir, const Vector3 &up) noexcept;
	static Matrix4 Perspective(float fov, float aspect, float zNear, float zFar) noexcept;
	static Matrix4 Ortho(float w, float h, float zNear, float zFar) noexcept;
};
#pragma pack(pop)
static_assert(sizeof(Matrix4) == 16 * sizeof(float), "invalid bytes");
DC_END_NAMESPACE
