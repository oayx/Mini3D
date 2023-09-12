 
/*****************************************************************************
* Author： hannibal
* Date：2009/11/13
* Description： 4×4矩阵
*****************************************************************************/
#pragma once

#include "Vector3.h"
#include "assert.h"
#include "Vector4.h"

DC_BEGIN_NAMESPACE
class Quaternion;
class Matrix3;
/********************************************************************/
#pragma pack(push,4)
// 4×4矩阵
class ENGINE_DLL Matrix4 Final : public object
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
	Matrix4();
	Matrix4(const float* arr, int size);
	Matrix4(const Matrix4 &mat);
	Matrix4(const Matrix3 &mat);
	Matrix4(const Vector3& position, const Quaternion& rotation, const Vector3& scale);
	Matrix4(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33);

public:
	//赋值，改变原始值
	Matrix4 &operator =(const Matrix4 &mat);
	void operator = ( const Matrix3& mat3 );
	
	//求负值，不改变原值
	Matrix4 operator -()const;

	//重载括号运算符
	float *operator [](int index) { return m[index]; }
	const float *operator [](int index)const { return m[index]; }

	//重载算术运算符
	Matrix4& operator *(float f);
	Matrix4 operator * (const Matrix4 &mat)const;
	Matrix4 operator + (const Matrix4 &mat) const;
	Matrix4 operator - (const Matrix4 &mat)const;

	//重载关系运算符
	bool operator == (const Matrix4& mat) const;
	bool operator != (Matrix4& mat) const;

	inline friend Vector4 operator * (const Vector4& v, const Matrix4& mat)
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
	Matrix4& Identity() { memset(p, 0, sizeof(p)); _11 = 1.0f; _22 = 1.0f; _33 = 1.0f; _44 = 1.0f; return *this; }
	//从0开始
	float	 Get(int row, int col)const { return p[row * 4 + col]; }
	void	 Set(int row, int col, float v) { p[row * 4 + col] = v; }

	//缩放
	Matrix4& Scale(float x, float y, float z) { _11 = x; _22 = y; _33 = z; return *this; }
	Matrix4& Scale(const Vector3 &vec3) { _11 = vec3.x; _22 = vec3.y; _33 = vec3.z; return *this; }

	//平移矩阵
	Matrix4& Translate(float x, float y, float z) { _41 = x; _42 = y; _43 = z; return *this; }
	Matrix4& Translate(const Vector3 &vec3) { _41 = vec3.x; _42 = vec3.y; _43 = vec3.z;return *this;  }
	Vector3  GetTranslate()const { return Vector3(_41, _42, _43); }

	//从旋转轴构建(位置不变)
	Quaternion GetRotate()const;

	//求矩阵的逆
	Matrix4 Inverse()const;
	//转置矩阵
	Matrix4 Transpose()const;

	//世界矩阵的逆的转置仅针对法向量，我们也不需要世界矩阵的平移分量
	//而且不去掉的话，后续再乘上观察矩阵之类的就会产生错误的变换结果
	Matrix4 InverseTranspose()const;

	Vector3 TransformVector(const Vector3& v) const;
	Vector3 TransformPoint(const Vector3& v) const;

	//矩阵分解
	void Decompose(Vector3& position, Quaternion& rotation, Vector3& scale);
	void Recompose(const Vector3& position, const Quaternion& rotation, const Vector3& scale);

	static Matrix4 FromTranslate(const Vector3& position);
	static Matrix4 FromRotate(const Quaternion& rotation);
	static Matrix4 FromRotate(const Vector3 &axis, float degree);
	static Matrix4 FromScale(const Vector3& scale);

	static Matrix4 LookAt(const Vector3 &pos, const Vector3 &at, const Vector3 &up);
	static Matrix4 LookTo(const Vector3 &pos, const Vector3 &dir, const Vector3 &up);
	static Matrix4 Perspective(float fov, float aspect, float zNear, float zFar);
	static Matrix4 Ortho(float w, float h, float zNear, float zFar);
};
#pragma pack(pop)
static_assert(sizeof(Matrix4) == 16 * sizeof(float), "invalid bytes");
DC_END_NAMESPACE
