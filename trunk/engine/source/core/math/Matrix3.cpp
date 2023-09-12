#include "Matrix3.h"
#include "Matrix4.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_REFECTION_TYPE(Matrix3);
Matrix3 Matrix3::zero
	(
	0,0,0,
	0,0,0,
	0,0,0
	);
Matrix3 Matrix3::identity
	(
	1,0,0,
	0,1,0,
	0,0,1
	);

Matrix3::Matrix3(float m00, float m01, float m02, float m10, float m11, float m12, float m20, float m21, float m22)
{
	m[0][0] = m00;
	m[0][1] = m01;
	m[0][2] = m02;
	m[1][0] = m10;
	m[1][1] = m11;
	m[1][2] = m12;
	m[2][0] = m20;
	m[2][1] = m21;
	m[2][2] = m22;
}
Matrix3::Matrix3(const Matrix3 &mat)
{
	m[0][0] = mat[0][0];
	m[0][1] = mat[0][1];
	m[0][2] = mat[0][2];
	m[1][0] = mat[1][0];
	m[1][1] = mat[1][1];
	m[1][2] = mat[1][2];
	m[2][0] = mat[2][0];
	m[2][1] = mat[2][1];
	m[2][2] = mat[2][2];
}
Matrix3::Matrix3(const Matrix4 &mat)
{
	m[0][0] = mat[0][0];
	m[0][1] = mat[0][1];
	m[0][2] = mat[0][2];
	m[1][0] = mat[1][0];
	m[1][1] = mat[1][1];
	m[1][2] = mat[1][2];
	m[2][0] = mat[2][0];
	m[2][1] = mat[2][1];
	m[2][2] = mat[2][2];
}
Matrix3 &Matrix3::operator = (const Matrix3& mat)
{
	if (this != &mat)
	{
		m[0][0] = mat.m[0][0]; m[0][1] = mat.m[0][1]; m[0][2] = mat.m[0][2];
		m[1][0] = mat.m[1][0]; m[1][1] = mat.m[1][1]; m[1][2] = mat.m[1][2];
		m[2][0] = mat.m[2][0]; m[2][1] = mat.m[2][1]; m[2][2] = mat.m[2][2];
	}
	return *this;
}
Matrix3 &Matrix3::operator = (const Matrix4& mat)
{
	m[0][0] = mat.m[0][0]; m[0][1] = mat.m[0][1]; m[0][2] = mat.m[0][2];
	m[1][0] = mat.m[1][0]; m[1][1] = mat.m[1][1]; m[1][2] = mat.m[1][2];
	m[2][0] = mat.m[2][0]; m[2][1] = mat.m[2][1]; m[2][2] = mat.m[2][2];
	return *this;
}
Matrix3 Matrix3::operator -()const
{
	return Matrix3(
		-m[0][0],
		-m[0][1],
		-m[0][2],

		-m[1][0],
		-m[1][1],
		-m[1][2],

		-m[2][0],
		-m[2][1],
		-m[2][2]
	);
}
Matrix3& Matrix3::operator *(float f)
{
	m[0][0] *= f;
	m[0][1] *= f;
	m[0][2] *= f;

	m[1][0] *= f;
	m[1][1] *= f;
	m[1][2] *= f;

	m[2][0] *= f;
	m[2][1] *= f;
	m[2][2] *= f;
	return *this;
}
Matrix3 Matrix3::operator + (const Matrix3 &mat) const
{
	return Matrix3(
		m[0][0] + mat.m[0][0],
		m[0][1] + mat.m[0][1],
		m[0][2] + mat.m[0][2],

		m[1][0] + mat.m[1][0],
		m[1][1] + mat.m[1][1],
		m[1][2] + mat.m[1][2],

		m[2][0] + mat.m[2][0],
		m[2][1] + mat.m[2][1],
		m[2][2] + mat.m[2][2]
	);
}
Matrix3 Matrix3::operator - (const Matrix3 &mat)const
{
	return Matrix3(
		m[0][0] - mat.m[0][0],
		m[0][1] - mat.m[0][1],
		m[0][2] - mat.m[0][2],

		m[1][0] - mat.m[1][0],
		m[1][1] - mat.m[1][1],
		m[1][2] - mat.m[1][2],

		m[2][0] - mat.m[2][0],
		m[2][1] - mat.m[2][1],
		m[2][2] - mat.m[2][2]
	);
}
Matrix3 Matrix3::operator / (float f)const
{
	return Matrix3(
		m[0][0] / f,
		m[0][1] / f,
		m[0][2] / f,

		m[1][0] / f,
		m[1][1] / f,
		m[1][2] / f,

		m[2][0] / f,
		m[2][1] / f,
		m[2][2] / f
	);
}
Matrix3 Matrix3::operator *(const Matrix3 &mat)const
{
	Matrix3 mat_out;
	mat_out.m[0][0] = m[0][0] * mat.m[0][0] + m[0][1] * mat.m[1][0] + m[0][2] * mat.m[2][0];
	mat_out.m[0][1] = m[0][0] * mat.m[0][1] + m[0][1] * mat.m[1][1] + m[0][2] * mat.m[2][1];
	mat_out.m[0][2] = m[0][0] * mat.m[0][2] + m[0][1] * mat.m[1][2] + m[0][2] * mat.m[2][2];

	mat_out.m[1][0] = m[1][0] * mat.m[0][0] + m[1][1] * mat.m[1][0] + m[1][2] * mat.m[2][0];
	mat_out.m[1][1] = m[1][0] * mat.m[0][1] + m[1][1] * mat.m[1][1] + m[1][2] * mat.m[2][1];
	mat_out.m[1][2] = m[1][0] * mat.m[0][2] + m[1][1] * mat.m[1][2] + m[1][2] * mat.m[2][2];

	mat_out.m[2][0] = m[2][0] * mat.m[0][0] + m[2][1] * mat.m[1][0] + m[2][2] * mat.m[2][0];
	mat_out.m[2][1] = m[2][0] * mat.m[0][1] + m[2][1] * mat.m[1][1] + m[2][2] * mat.m[2][1];
	mat_out.m[2][2] = m[2][0] * mat.m[0][2] + m[2][1] * mat.m[1][2] + m[2][2] * mat.m[2][2];
	return mat_out;
}
bool Matrix3::operator == (const Matrix3& mat) const
{
	if (
		m[0][0] != mat.m[0][0] || m[0][1] != mat.m[0][1] || m[0][2] != mat.m[0][2] ||
		m[1][0] != mat.m[1][0] || m[1][1] != mat.m[1][1] || m[1][2] != mat.m[1][2] ||
		m[2][0] != mat.m[2][0] || m[2][1] != mat.m[2][1] || m[2][2] != mat.m[2][2])
	{
		return false;
	}

	return true;
}
bool Matrix3::operator != (Matrix3& mat) const
{
	if (
		m[0][0] != mat.m[0][0] || m[0][1] != mat.m[0][1] || m[0][2] != mat.m[0][2] ||
		m[1][0] != mat.m[1][0] || m[1][1] != mat.m[1][1] || m[1][2] != mat.m[1][2] ||
		m[2][0] != mat.m[2][0] || m[2][1] != mat.m[2][1] || m[2][2] != mat.m[2][2])
	{
		return true;
	}

	return false;
}
Matrix3 Matrix3::Inverse()const
{
	Matrix4 mat4(*this);
	mat4 = mat4.Inverse();
	return mat4;
}
Matrix3 Matrix3::Transpose()const
{
	Matrix3 mat_out;
	mat_out._11 = _11;
	mat_out._12 = _21;
	mat_out._13 = _31;
	mat_out._21 = _12;
	mat_out._22 = _22;
	mat_out._23 = _32;
	mat_out._31 = _13;
	mat_out._32 = _23;
	mat_out._33 = _33;
	return mat_out;
}
Matrix3 Matrix3::Reflect(const Vector3& normal)const
{
	Vector3 vecNor = normal.Normalize();
	Matrix3 mat_out;
	mat_out._11 = 1 - 2 * vecNor.x*vecNor.x;
	mat_out._12 = -2 * vecNor.x * vecNor.y;
	mat_out._13 = -2 * vecNor.x * vecNor.z;

	mat_out._21 = -2 * vecNor.x * vecNor.y;
	mat_out._22 = 1 - 2 * vecNor.y*vecNor.y;
	mat_out._23 = -2 * vecNor.y * vecNor.z;

	mat_out._31 = -2 * vecNor.x * vecNor.z;
	mat_out._32 = -2 * vecNor.z * vecNor.y;
	mat_out._33 = 1 - 2 * vecNor.z*vecNor.z;
	return mat_out;
}
Matrix3& Matrix3::Rotate(const Quaternion& q)
{
	float x = q.x * 2.0F;
	float y = q.y * 2.0F;
	float z = q.z * 2.0F;
	float xx = q.x * x;
	float yy = q.y * y;
	float zz = q.z * z;
	float xy = q.x * y;
	float xz = q.x * z;
	float yz = q.y * z;
	float wx = q.w * x;
	float wy = q.w * y;
	float wz = q.w * z;

	_11 = 1.0f - (yy + zz);
	_12 = xy + wz;
	_13 = xz - wy;
	_21 = xy - wz;
	_22 = 1.0f - (xx + zz);
	_23 = yz + wx;
	_31 = xz + wy;
	_32 = yz - wx;
	_33 = 1.0f - (xx + yy);
	return *this;
}
Matrix3& Matrix3::Rotate(const Vector3 &axis, float degree)
{
	float angle = Math::Deg2Rad * degree;
	float cost = ::cos(angle), sint = ::sin(angle), one_sub_cost = 1 - cost;
	Vector3 v = axis.Normalize();
	m[0][0] = v.x * v.x * one_sub_cost + cost;			m[0][1] = v.x * v.y * one_sub_cost + v.z * sint;	m[0][2] = v.x * v.z * one_sub_cost - v.y * sint;
	m[1][0] = v.x * v.y * one_sub_cost - v.z * sint;	m[1][1] = v.y * v.y * one_sub_cost + cost;			m[1][2] = v.y * v.z * one_sub_cost + v.x * sint;
	m[2][0] = v.x * v.z * one_sub_cost + v.y * sint;	m[2][1] = v.y * v.z * one_sub_cost - v.x * sint;	m[2][2] = v.z * v.z * one_sub_cost + cost;
	return *this;
}
Quaternion Matrix3::ToRotate()const
{
	float tr = Get(0, 0) + Get(1, 1) + Get(2, 2);
	float s;

	Quaternion q;
	if (tr > 0.0f)
	{
		s = sqrt(tr + 1.0f);
		q.w = 0.5f*s;
		s = 0.5f / s;
		q.x = (Get(1, 2) - Get(2, 1))*s;
		q.y = (Get(2, 0) - Get(0, 2))*s;
		q.z = (Get(0, 1) - Get(1, 0))*s;
	}
	else
	{
		// |w| <= 1/2
		int s_iNext[3] = { 1, 2, 0 };
		int i = 0;
		if (Get(1, 1) > Get(0, 0))
			i = 1;
		if (Get(2, 2) > Get(i, i))
			i = 2;
		int j = s_iNext[i];
		int k = s_iNext[j];

		s = sqrt(Get(i, i) - Get(j, j) - Get(k, k) + 1.0f);
		float* t[3] = { &q.x, &q.y, &q.z };
		*t[i] = 0.5f*s;
		s = 0.5f / s;
		q.w = (Get(j, k) - Get(k, j)) * s;
		*t[j] = (Get(i, j) + Get(j, i))*s;
		*t[k] = (Get(i, k) + Get(k, i))*s;
	}
	return q.Normalize();
}

Matrix3& Matrix3::Euler(const Vector3& euler)
{
	float cx = ::cos(Math::Deg2Rad * euler.x);
	float sx = ::sin(Math::Deg2Rad * euler.x);
	float cy = ::cos(Math::Deg2Rad * euler.y);
	float sy = ::sin(Math::Deg2Rad * euler.y);
	float cz = ::cos(Math::Deg2Rad * euler.z);
	float sz = ::sin(Math::Deg2Rad * euler.z);

	_11 = cy * cz + sx * sy*sz;
	_12 = cz * sx*sy - cy * sz;
	_13 = cx * sy;

	_21 = cx * sz;
	_22 = cx * cz;
	_23 = -sx;

	_31 = -cz * sy + cy * sx*sz;
	_32 = cy * cz*sx + sy * sz;
	_33 = cx * cy;
	return *this;
}
Vector3 Matrix3::ToEuler()const
{
	float x, y, z;
	// from http://www.geometrictools.com/Documentation/EulerAngles.pdf
	// YXZ order
	if (_23 < 0.999F) // some fudge for imprecision
	{
		if (_23 > -0.999F) // some fudge for imprecision
		{
			x = ::asin(-_23);
			y = ::atan2(_13, _33);
			z = ::atan2(_21, _22);
		}
		else
		{
			// WARNING.  Not unique.  YA - ZA = atan2(r01,r00)
			x = Math::PI / 4.0f;
			y = ::atan2(_12, _11);
			z = 0.0F;
		}
	}
	else
	{
		// WARNING.  Not unique.  YA + ZA = atan2(-r01,r00)
		x = -Math::PI / 4.0f;
		y = ::atan2(-_12, _11);
		z = 0.0F;
	}
	return Vector3(Math::Rad2Deg * x, Math::Rad2Deg * y, Math::Rad2Deg * z);
}
Matrix3 Matrix3::FromRotate(const Quaternion& q)
{
	Matrix3 mat(q);
	return mat;
}
Matrix3 Matrix3::FromScale(const Vector3& scale)
{
	Matrix3 mat;
	mat.Scale(scale);
	return mat;
}
Matrix3 Matrix3::LookTo(const Vector3 &dir, const Vector3 &up)
{
	Vector3 xAxis;
	Vector3 zAxis = dir.Normalize();
	Vector3 yAxis = up.Normalize();

	xAxis = yAxis.Cross(zAxis);
	xAxis.Normalize();

	yAxis = zAxis.Cross(xAxis);
	yAxis.Normalize();

	Matrix3 mat;
	mat._11 = xAxis.x; mat._12 = yAxis.x; mat._13 = zAxis.x;
	mat._21 = xAxis.y; mat._22 = yAxis.y; mat._23 = zAxis.y;
	mat._31 = xAxis.z; mat._32 = yAxis.z; mat._33 = zAxis.z;
	return mat;
}
Matrix3 Matrix3::LookAt(const Vector3 &eye, const Vector3 &at, const Vector3 &up)
{
	Vector3 dir(at - eye);
	return LookTo(dir, up);
}
DC_END_NAMESPACE