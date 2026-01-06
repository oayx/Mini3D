#include "Matrix4.h"
#include "Matrix3.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_REFECTION_TYPE(Matrix4);
const Matrix4 Matrix4::zero
	(
	0,0,0,0,
	0,0,0,0,
	0,0,0,0,
	0,0,0,0
	);
const Matrix4 Matrix4::identity
	(
	1,0,0,0,
	0,1,0,0,
	0,0,1,0,
	0,0,0,1
	);

Matrix4::Matrix4(const Matrix3& mat)
{
	m[0][0] = mat[0][0]; m[0][1] = mat[0][1]; m[0][2] = mat[0][2]; m[0][3] = 0;
	m[1][0] = mat[1][0]; m[1][1] = mat[1][1]; m[1][2] = mat[1][2]; m[1][3] = 0;
	m[2][0] = mat[2][0]; m[2][1] = mat[2][1]; m[2][2] = mat[2][2]; m[2][3] = 0;
	m[3][0] = 0; m[3][1] = 0; m[3][2] = 0; m[3][3] = 1;
}
Matrix4 &Matrix4::operator = (const Matrix4& other) noexcept
{
	if (this != &other)
	{
		m[0][0] = other.m[0][0]; m[0][1] = other.m[0][1]; m[0][2] = other.m[0][2]; m[0][3] = other.m[0][3];
		m[1][0] = other.m[1][0]; m[1][1] = other.m[1][1]; m[1][2] = other.m[1][2]; m[1][3] = other.m[1][3];
		m[2][0] = other.m[2][0]; m[2][1] = other.m[2][1]; m[2][2] = other.m[2][2]; m[2][3] = other.m[2][3];
		m[3][0] = other.m[3][0]; m[3][1] = other.m[3][1]; m[3][2] = other.m[3][2]; m[3][3] = other.m[3][3];
	}
	return *this;
}
Matrix4& Matrix4::operator = (Matrix4&& other) noexcept
{
	if (this != &other)
	{
		m[0][0] = other.m[0][0]; m[0][1] = other.m[0][1]; m[0][2] = other.m[0][2]; m[0][3] = other.m[0][3];
		m[1][0] = other.m[1][0]; m[1][1] = other.m[1][1]; m[1][2] = other.m[1][2]; m[1][3] = other.m[1][3];
		m[2][0] = other.m[2][0]; m[2][1] = other.m[2][1]; m[2][2] = other.m[2][2]; m[2][3] = other.m[2][3];
		m[3][0] = other.m[3][0]; m[3][1] = other.m[3][1]; m[3][2] = other.m[3][2]; m[3][3] = other.m[3][3];
	}
	return *this;
}
void Matrix4::operator = (const Matrix3& other) noexcept
{
	m[0][0] = other.m[0][0]; m[0][1] = other.m[0][1]; m[0][2] = other.m[0][2];
	m[1][0] = other.m[1][0]; m[1][1] = other.m[1][1]; m[1][2] = other.m[1][2];
	m[2][0] = other.m[2][0]; m[2][1] = other.m[2][1]; m[2][2] = other.m[2][2];
}
Matrix4& Matrix4::operator *(float f) noexcept
{
	m[0][0] *= f;
	m[0][1] *= f;
	m[0][2] *= f;
	m[0][3] *= f;

	m[1][0] *= f;
	m[1][1] *= f;
	m[1][2] *= f;
	m[1][3] *= f;

	m[2][0] *= f;
	m[2][1] *= f;
	m[2][2] *= f;
	m[2][3] *= f;

	m[3][0] *= f;
	m[3][1] *= f;
	m[3][2] *= f;
	m[3][3] *= f;
	return *this;
}
Matrix4 Matrix4::operator *(const Matrix4 &mat)const noexcept
{
	Matrix4 mat_out;
	mat_out.m[0][0] = m[0][0] * mat.m[0][0] + m[0][1] * mat.m[1][0] + m[0][2] * mat.m[2][0] + m[0][3] * mat.m[3][0];
	mat_out.m[0][1] = m[0][0] * mat.m[0][1] + m[0][1] * mat.m[1][1] + m[0][2] * mat.m[2][1] + m[0][3] * mat.m[3][1];
	mat_out.m[0][2] = m[0][0] * mat.m[0][2] + m[0][1] * mat.m[1][2] + m[0][2] * mat.m[2][2] + m[0][3] * mat.m[3][2];
	mat_out.m[0][3] = m[0][0] * mat.m[0][3] + m[0][1] * mat.m[1][3] + m[0][2] * mat.m[2][3] + m[0][3] * mat.m[3][3];

	mat_out.m[1][0] = m[1][0] * mat.m[0][0] + m[1][1] * mat.m[1][0] + m[1][2] * mat.m[2][0] + m[1][3] * mat.m[3][0];
	mat_out.m[1][1] = m[1][0] * mat.m[0][1] + m[1][1] * mat.m[1][1] + m[1][2] * mat.m[2][1] + m[1][3] * mat.m[3][1];
	mat_out.m[1][2] = m[1][0] * mat.m[0][2] + m[1][1] * mat.m[1][2] + m[1][2] * mat.m[2][2] + m[1][3] * mat.m[3][2];
	mat_out.m[1][3] = m[1][0] * mat.m[0][3] + m[1][1] * mat.m[1][3] + m[1][2] * mat.m[2][3] + m[1][3] * mat.m[3][3];

	mat_out.m[2][0] = m[2][0] * mat.m[0][0] + m[2][1] * mat.m[1][0] + m[2][2] * mat.m[2][0] + m[2][3] * mat.m[3][0];
	mat_out.m[2][1] = m[2][0] * mat.m[0][1] + m[2][1] * mat.m[1][1] + m[2][2] * mat.m[2][1] + m[2][3] * mat.m[3][1];
	mat_out.m[2][2] = m[2][0] * mat.m[0][2] + m[2][1] * mat.m[1][2] + m[2][2] * mat.m[2][2] + m[2][3] * mat.m[3][2];
	mat_out.m[2][3] = m[2][0] * mat.m[0][3] + m[2][1] * mat.m[1][3] + m[2][2] * mat.m[2][3] + m[2][3] * mat.m[3][3];

	mat_out.m[3][0] = m[3][0] * mat.m[0][0] + m[3][1] * mat.m[1][0] + m[3][2] * mat.m[2][0] + m[3][3] * mat.m[3][0];
	mat_out.m[3][1] = m[3][0] * mat.m[0][1] + m[3][1] * mat.m[1][1] + m[3][2] * mat.m[2][1] + m[3][3] * mat.m[3][1];
	mat_out.m[3][2] = m[3][0] * mat.m[0][2] + m[3][1] * mat.m[1][2] + m[3][2] * mat.m[2][2] + m[3][3] * mat.m[3][2];
	mat_out.m[3][3] = m[3][0] * mat.m[0][3] + m[3][1] * mat.m[1][3] + m[3][2] * mat.m[2][3] + m[3][3] * mat.m[3][3];
	return mat_out;
}
Matrix4 Matrix4::operator + (const Matrix4 &mat) const noexcept
{
	return Matrix4(
		m[0][0] + mat.m[0][0],
		m[0][1] + mat.m[0][1],
		m[0][2] + mat.m[0][2],
		m[0][2] + mat.m[0][3],

		m[1][0] + mat.m[1][0],
		m[1][1] + mat.m[1][1],
		m[1][2] + mat.m[1][2],
		m[1][3] + mat.m[1][3],

		m[2][0] + mat.m[2][0],
		m[2][1] + mat.m[2][1],
		m[2][2] + mat.m[2][2],
		m[2][3] + mat.m[2][3],

		m[3][0] + mat.m[3][0],
		m[3][1] + mat.m[3][1],
		m[3][2] + mat.m[3][2],
		m[3][3] + mat.m[3][3]
	);
}
Matrix4 Matrix4::operator - (const Matrix4 &mat)const noexcept
{
	return Matrix4(
		m[0][0] - mat.m[0][0],
		m[0][1] - mat.m[0][1],
		m[0][2] - mat.m[0][2],
		m[0][2] - mat.m[0][3],

		m[1][0] - mat.m[1][0],
		m[1][1] - mat.m[1][1],
		m[1][2] - mat.m[1][2],
		m[1][3] - mat.m[1][3],

		m[2][0] - mat.m[2][0],
		m[2][1] - mat.m[2][1],
		m[2][2] - mat.m[2][2],
		m[2][3] - mat.m[2][3],

		m[3][0] - mat.m[3][0],
		m[3][1] - mat.m[3][1],
		m[3][2] - mat.m[3][2],
		m[3][3] - mat.m[3][3]
	);
}
Matrix4 Matrix4::Inverse()const noexcept
{
	float m00 = m[0][0], m01 = m[0][1], m02 = m[0][2], m03 = m[0][3];
	float m10 = m[1][0], m11 = m[1][1], m12 = m[1][2], m13 = m[1][3];
	float m20 = m[2][0], m21 = m[2][1], m22 = m[2][2], m23 = m[2][3];
	float m30 = m[3][0], m31 = m[3][1], m32 = m[3][2], m33 = m[3][3];

	float v0 = m20 * m31 - m21 * m30;
	float v1 = m20 * m32 - m22 * m30;
	float v2 = m20 * m33 - m23 * m30;
	float v3 = m21 * m32 - m22 * m31;
	float v4 = m21 * m33 - m23 * m31;
	float v5 = m22 * m33 - m23 * m32;

	float t00 = + (v5 * m11 - v4 * m12 + v3 * m13);
	float t10 = - (v5 * m10 - v2 * m12 + v1 * m13);
	float t20 = + (v4 * m10 - v2 * m11 + v0 * m13);
	float t30 = - (v3 * m10 - v1 * m11 + v0 * m12);

	float invDet = 1 / (t00 * m00 + t10 * m01 + t20 * m02 + t30 * m03);

	float d00 = t00 * invDet;
	float d10 = t10 * invDet;
	float d20 = t20 * invDet;
	float d30 = t30 * invDet;

	float d01 = - (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
	float d11 = + (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
	float d21 = - (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
	float d31 = + (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

	v0 = m10 * m31 - m11 * m30;
	v1 = m10 * m32 - m12 * m30;
	v2 = m10 * m33 - m13 * m30;
	v3 = m11 * m32 - m12 * m31;
	v4 = m11 * m33 - m13 * m31;
	v5 = m12 * m33 - m13 * m32;

	float d02 = + (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
	float d12 = - (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
	float d22 = + (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
	float d32 = - (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

	v0 = m21 * m10 - m20 * m11;
	v1 = m22 * m10 - m20 * m12;
	v2 = m23 * m10 - m20 * m13;
	v3 = m22 * m11 - m21 * m12;
	v4 = m23 * m11 - m21 * m13;
	v5 = m23 * m12 - m22 * m13;

	float d03 = - (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
	float d13 = + (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
	float d23 = - (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
	float d33 = + (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

	return Matrix4(
		d00, d01, d02, d03,
		d10, d11, d12, d13,
		d20, d21, d22, d23,
		d30, d31, d32, d33
		);
}
Matrix4 Matrix4::Transpose()const noexcept
{
	Matrix4 mat_out;
	mat_out._11 = _11;
	mat_out._12 = _21;
	mat_out._13 = _31;
	mat_out._14 = _41;
	mat_out._21 = _12;
	mat_out._22 = _22;
	mat_out._23 = _32;
	mat_out._24 = _42;
	mat_out._31 = _13;
	mat_out._32 = _23;
	mat_out._33 = _33;
	mat_out._34 = _43;
	mat_out._41 = _14;
	mat_out._42 = _24;
	mat_out._43 = _34;
	mat_out._44 = _44;
	return mat_out;
}
Matrix4 Matrix4::InverseTranspose()const noexcept
{
	Matrix4 mat = *this;
	mat._41 = mat._42 = mat._43 = 0.0f;
	mat._44 = 1.0f;
	mat = mat.Inverse();
	mat = mat.Transpose();
	return mat;
}
Vector3 Matrix4::TransformVector(const Vector3& vec) const noexcept
{
	return Vector3(
		(vec.x * _11 + vec.y * _21 + vec.z * _31),
		(vec.x * _12 + vec.y * _22 + vec.z * _32),
		(vec.x * _13 + vec.y * _23 + vec.z * _33)
	);
}
Vector3 Matrix4::TransformPoint(const Vector3& vec) const noexcept
{
	float w = 1.0f / (vec.x * _14 + vec.y * _24 + vec.z * _34 + _44);
	return Vector3(
		(vec.x * _11 + vec.y * _21 + vec.z * _31 + _41) * w,
		(vec.x * _12 + vec.y * _22 + vec.z * _32 + _42) * w,
		(vec.x * _13 + vec.y * _23 + vec.z * _33 + _43) * w
	);
}
void Matrix4::Decompose(Vector3& translation, Quaternion& rotation, Vector3& scale) noexcept
{
	Vector3 xaxis(m[0][0], m[0][1], m[0][2]);
	scale[0] = xaxis.Lenght();
	Vector3 yaxis(m[1][0], m[1][1], m[1][2]);
	scale[1] = yaxis.Lenght();
	Vector3 zaxis(m[2][0], m[2][1], m[2][2]);
	scale[2] = zaxis.Lenght();

	xaxis.Normalize();
	yaxis.Normalize();
	zaxis.Normalize();
	m[0][0] = xaxis[0]; m[0][1] = xaxis[1]; m[0][2] = xaxis[2];
	m[1][0] = yaxis[0]; m[1][1] = yaxis[1]; m[1][2] = yaxis[2];
	m[2][0] = zaxis[0]; m[2][1] = zaxis[1]; m[2][2] = zaxis[2];

	rotation = GetRotate();
	translation = GetTranslate();
}

void Matrix4::Recompose(const Vector3& position, const Quaternion& rotation, const Vector3& scale) noexcept
{
	//执行顺序：缩放->旋转->平移
	Matrix4 mat_s = Matrix4::FromScale(scale);
	Matrix4 mat_r = Matrix4::FromRotate(rotation);
	Matrix4 mat_t = Matrix4::FromTranslate(position);
	*this = mat_s * mat_r * mat_t;
}
Quaternion Matrix4::GetRotate()const noexcept
{
	Matrix3 mat3(*this);
	return mat3.ToRotate();
}
Matrix4 Matrix4::FromTranslate(const Vector3& position) noexcept
{
	Matrix4 mat;
	mat.Translate(position);
	return mat;
}
Matrix4 Matrix4::FromRotate(const Quaternion& rotation) noexcept
{
	Matrix3 mat3(rotation);
	Matrix4 mat4 = mat3;
	return mat4;
}
/*
描叙：创建绕任意轴顺时针旋转的矩阵，有缺陷，如vec(20,0,0)，将丢失轴
方法：
步骤：
1 把旋转轴变为单位向量
2 把旋转轴平移到经过原点
3 把旋转轴旋转到yoz平面上(绕z轴旋转)
4 把旋转轴旋转到z轴上(绕x轴旋转)
5 绕Z轴旋转
6 执行逆操作

矩阵变换：
[y0/lr  x0/lr 0 0]   [1 0   0  0]   [cosA  sinA 0 0]   [1 0  0   0]   [y0/lr  -x0/lr 0 0]
[-x0/lr y0/lr 0 0]   [0 z0  lr 0]   [-sinA cosA 0 0]   [0 z0 -lr 0]   [x0/lr  y0/lr  0 0]
M = [0      0     1 0] x [0 -lr z0 0] x [0     0    1 0] x [0 lr z0  0] x [0      0      1 0]
[0      0     0 1]   [0 0   0  1]   [0     0    0 1]   [0 0  0   1]   [0      0      0 1]
其中lr为轴向量在xoy面的投影长	，lr = sqrt(x0^2 + y0^2)。
*/
Matrix4 Matrix4::FromRotate(const Vector3 &axis, float degree) noexcept
{
	Matrix4 m;
	float angle = Math::Deg2Rad * degree;
	float cost = ::cos(angle), sint = ::sin(angle), one_sub_cost = 1 - cost;
	Vector3 v = axis.Normalize();
	m[0][0] = v.x * v.x * one_sub_cost + cost;			m[0][1] = v.x * v.y * one_sub_cost + v.z * sint;	m[0][2] = v.x * v.z * one_sub_cost - v.y * sint;
	m[1][0] = v.x * v.y * one_sub_cost - v.z * sint;	m[1][1] = v.y * v.y * one_sub_cost + cost;			m[1][2] = v.y * v.z * one_sub_cost + v.x * sint;
	m[2][0] = v.x * v.z * one_sub_cost + v.y * sint;	m[2][1] = v.y * v.z * one_sub_cost - v.x * sint;	m[2][2] = v.z * v.z * one_sub_cost + cost;
	return m;
}
Matrix4 Matrix4::FromScale(const Vector3& scale) noexcept
{
	Matrix4 mat;
	mat.Scale(scale);
	return mat;
}
Matrix4 Matrix4::LookAt(const Vector3 &pos, const Vector3 &at, const Vector3 &up) noexcept
{
	Vector3 dir(at - pos);
	return LookTo(pos, dir, up);
}
Matrix4 Matrix4::LookTo(const Vector3 &pos, const Vector3 &dir, const Vector3 &up) noexcept
{
	Vector3 xAxis;
	Vector3 zAxis = dir.Normalize();
	Vector3 yAxis = up.Normalize();
	Vector3 eye_at = pos;

	xAxis = yAxis.Cross(zAxis);
	xAxis.Normalize();

	yAxis = zAxis.Cross(xAxis);
	yAxis.Normalize();

	Matrix4 mat;
	mat._11 = xAxis.x; mat._12 = yAxis.x; mat._13 = zAxis.x; mat._14 = 0;
	mat._21 = xAxis.y; mat._22 = yAxis.y; mat._23 = zAxis.y; mat._24 = 0;
	mat._31 = xAxis.z; mat._32 = yAxis.z; mat._33 = zAxis.z; mat._34 = 0;
	mat._41 = -xAxis.Dot(eye_at);
	mat._42 = -yAxis.Dot(eye_at);
	mat._43 = -zAxis.Dot(eye_at);
	mat._44 = 1;
	return mat;
}
//统一NDC空间下 z值到[-1,1]
Matrix4 Matrix4::Perspective(float fov, float aspect, float zNear, float zFar) noexcept
{
	float h = (1.0f / tanf(fov * 0.5f));
	float w = h / aspect;
	float a = (zFar + zNear) / (zFar - zNear);
	float b = -2.0f * zFar * zNear / (zFar - zNear);

	Matrix4 mat;
	mat._11 = w;
	mat._22 = h;
	mat._33 = a;
	mat._34 = 1.0f;
	mat._43 = b;
	mat._44 = 0.0f;
#if defined(DC_GRAPHICS_API_VULKAN)
	//Vulkan Y轴朝下，这里修改成Y轴朝上
	mat.Set(3, 1, -mat.Get(3, 1));
	mat.Set(1, 1, -mat.Get(1, 1));
#endif
	return mat;
}
Matrix4 Matrix4::Ortho(float w, float h, float zNear, float zFar) noexcept
{
	Matrix4 mat;
	mat.Identity();
	mat._11 = 2.0f / w;
	mat._22 = 2.0f / h;
	mat._33 = 1.0f / (zFar - zNear);
	mat._34 = 0.0f;
	mat._43 = zNear / (zNear - zFar);
	mat._44 = 1.0f;
#if defined(DC_GRAPHICS_API_VULKAN)
	//Vulkan Y轴朝下，这里修改成Y轴朝上
	mat.Set(3, 1, -mat.Get(3, 1));
	mat.Set(1, 1, -mat.Get(1, 1));
#endif
	return mat;
}
DC_END_NAMESPACE
