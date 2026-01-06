#include "Quaternion.h"
#include "Matrix3.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_REFECTION_TYPE(Quaternion);
Quaternion Quaternion::identity(0.0f, 0.0f, 0.0f, 1.0f);
Quaternion Quaternion::operator *= (const Quaternion &q) noexcept
{
	float qx, qy, qz, qw;
	qx = x;
	qy = y;
	qz = z;
	qw = w;

	x = qw * q.x + qx * q.w + qy * q.z - qz * q.y;
	y = qw * q.y - qx * q.z + qy * q.w + qz * q.x;
	z = qw * q.z + qx * q.y - qy * q.x + qz * q.w;
	w = qw * q.w - qx * q.x - qy * q.y - qz * q.z;

	return *this;
}
Quaternion Quaternion::operator *= (const Vector3& v) noexcept
{
	float qx, qy, qz, qw;
	qx = x;
	qy = y;
	qz = z;
	qw = w;

	x = qw * v.x + qy * v.z - qz * v.y;
	y = qw * v.y - qx * v.z + qz * v.x;
	z = qw * v.z + qx * v.y - qy * v.x;
	w = -qx * v.x - qy * v.y - qz * v.z;

	return *this;
}
Vector3 Quaternion::operator *(const Vector3 &vec) const noexcept
{
	Vector3 uv, uuv;
	Vector3 qvec(x, y, z);
	uv = qvec.Cross(vec);
	uuv = qvec.Cross(uv);
	uv = uv * (2.0f * w);
	uuv = uuv * 2.0f;

	return vec + uv + uuv;
}
Quaternion operator* (float scale, const Quaternion& q2) noexcept
{
	return Quaternion(scale*q2.x, scale*q2.y, scale*q2.z, scale*q2.w);
}
Quaternion Quaternion::Normalize() noexcept
{
	float dist, square;
	square = x * x + y * y + z * z + w * w;
	if (square > 0.0)
		dist = (float)(1.0f / Math::Sqrt(square));
	else
		dist = 1;

	x *= dist;
	y *= dist;
	z *= dist;
	w *= dist;
	return *this;
}
Quaternion Quaternion::Inverse() const noexcept
{
	float fNorm = w * w + x * x + y * y + z * z;
	if (fNorm > 0.0f)
	{
		float fInvNorm = 1.0f / fNorm;
		return Quaternion(-x * fInvNorm, -y * fInvNorm, -z * fInvNorm, w * fInvNorm);
	}
	else
	{
		return identity;
	}
}
Quaternion Quaternion::FromEuler(const Vector3& euler) noexcept
{
	float eulerX = Math::Deg2Rad * euler.x * 0.5f;
	float eulerY = Math::Deg2Rad * euler.y * 0.5f;
	float eulerZ = Math::Deg2Rad * euler.z * 0.5f;

	const float sinX = sinf(eulerX);
	const float cosX = cosf(eulerX);
	const float sinY = sinf(eulerY);
	const float cosY = cosf(eulerY);
	const float sinZ = sinf(eulerZ);
	const float cosZ = cosf(eulerZ);

	x = cosZ * cosY*sinX - sinZ * sinY*cosX;
	y = cosZ * sinY*cosX + sinZ * cosY*sinX;
	z = sinZ * cosY*cosX - cosZ * sinY*sinX;
	w = cosZ * cosY*cosX + sinZ * sinY*sinX;

	return*this;
}
Vector3 Quaternion::ToEuler()const noexcept
{
	const float sqw = w * w;
	const float sqx = x * x;
	const float sqy = y * y;
	const float sqz = z * z;

	const float unit = sqx + sqy + sqz + sqw;
	const float test = x * y + z * w;

	float yaw = 0.0f;
	float pitch = 0.0f;
	float roll = 0.0f;

	//奇异姿态, 俯仰角为±90°
	if (test > 0.499f * unit)
	{
		yaw = 2.0f * Math::ATan2(x, w);
		pitch = Math::PI * 0.5f;
		roll = 0.0f;
	}
	else if (test < -0.499f * unit)
	{
		yaw = -2.0f * Math::ATan2(x, w);
		pitch = -Math::PI * 0.5f;
		roll = 0.0f;
	}
	else
	{
		yaw = Math::ATan2(2.0f * y * w - 2.0f * x * z, sqx - sqy - sqz + sqw);
		pitch = Math::ASin(2.0f * test / unit);
		roll = Math::ATan2(2.0f * x * w - 2.0f * y * z, -sqx + sqy - sqz + sqw);
	}

	//[0..360].
	if (Math::Sign(yaw) < 0.f)
		yaw = Math::Deg2Rad * (360.f) + yaw;
	if (Math::Sign(pitch) < 0.f)
		pitch = Math::Deg2Rad * (360.f) + pitch;
	if (Math::Sign(roll) < 0.f)
		roll = Math::Deg2Rad * (360.f) + roll;

	return Vector3(roll, yaw, pitch) * Math::Rad2Deg;
}
Quaternion Quaternion::FromRadianAxis(float degree, const Vector3 &axis) noexcept
{
	Vector3 v = axis.Normalize();
	float cosv, sinv;
	cosv = Math::Cos(Math::Deg2Rad * degree * 0.5f);
	sinv = Math::Sin(Math::Deg2Rad * degree * 0.5f);
	x = v.x * sinv;
	y = v.y * sinv;
	z = v.z * sinv;
	w = cosv;
	return  *this;
}
void Quaternion::ToRadianAxis(float &degree, Vector3 &axis)const noexcept
{
	float angle = 2.0f * acos(w);
	degree = Math::Rad2Deg * angle;
	if (Math::FloatEqual(angle, 0.0F))
	{
		axis = Vector3::right;
		return;
	}
	float div = 1.0f / sqrt(1.0f - w*w);
	axis.Set(x*div, y*div, z*div);
}
Quaternion Quaternion::FromAxis(const Vector3 &xAxis, const Vector3 &yAxis, const Vector3 &zAxis) noexcept
{
	Matrix3 mat;
	mat._11 = xAxis.x;
	mat._12 = xAxis.y;
	mat._13 = xAxis.z;

	mat._21 = yAxis.x;
	mat._22 = yAxis.y;
	mat._23 = yAxis.z;

	mat._31 = zAxis.x;
	mat._32 = zAxis.y;
	mat._33 = zAxis.z;

	*this = mat.ToRotate();
	return *this;
}
void Quaternion::ToAxis(Vector3 &xAxis, Vector3 &yAxis, Vector3 &zAxis)const noexcept
{
	Matrix3 mat(*this);
	xAxis.x = mat._11;
	xAxis.y = mat._12;
	xAxis.z = mat._13;

	yAxis.x = mat._21;
	yAxis.y = mat._22;
	yAxis.z = mat._23;

	zAxis.x = mat._31;
	zAxis.y = mat._32;
	zAxis.z = mat._33;
}
Quaternion Quaternion::Slerp(const Quaternion& q1,const Quaternion& q2, float dt) noexcept
{
	float dot = q1.Dot(q2);
	Quaternion temp_q;

	// Do we need to invert rotation?
	if (dot < 0.0f)
	{
		dot = -dot;
		temp_q = -q2;
	}
	else
	{
		temp_q = q2;
	}

	if (Math::Abs(dot) < 0.95f)
	{
		// Standard case (slerp)
		float fSin = Math::Sqrt(1 - Math::Sqr(dot));
		float fAngle = Math::ATan2(fSin, dot);
		float fInvSin = 1.0f / fSin;
		float fCoeff0 = Math::Sin((1.0f - dt) * fAngle) * fInvSin;
		float fCoeff1 = Math::Sin(dt * fAngle) * fInvSin;
		return fCoeff0 * q1 + fCoeff1 * temp_q;
	}
	else
	{
		return Lerp(q1, temp_q, dt);
	}
}
Quaternion Quaternion::Lerp(const Quaternion& q1, const Quaternion& q2, float dt) noexcept
{
	Quaternion result;
	float dot = q1.Dot(q2);
	if (dot < 0.0f)
	{
		result = q1 + dt * ((-q2) - q1);
	}
	else
	{
		result = q1 + dt * (q2 - q1);
	}
	return result.Normalize();
}
DC_END_NAMESPACE
