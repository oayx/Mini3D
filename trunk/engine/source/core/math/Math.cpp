#include "Math.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix3.h"
#include "Matrix4.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_REFECTION_TYPE(Math);
const float Math::PI = 3.1415926f;
const float Math::PI2 = 6.2831853f;
const float Math::INVPI = 0.3183098f;
const float Math::Epsilon = 0.00001f;

const float Math::Deg2Rad = 0.0174533f;
const float Math::Rad2Deg = 57.2958f;

/********************************************************************/
static float Vector2_RotationAngle(const Vector2& vec1, const Vector2& vec2)
{
	return vec1.RotationAngle(vec2);
}
/********************************************************************/
static void Quaternion_FromEuler(Quaternion& q, const Vector3& vec)
{
	q.FromEuler(vec);
}
static void Quaternion_ToEuler(const Quaternion& q, Vector3& vec)
{
	vec = q.ToEuler();
}
static void Quaternion_FromRadianAxis(Quaternion& q, float degree, const Vector3& vec)
{
	q.FromRadianAxis(degree, vec);
}
static void Quaternion_ToRadianAxis(const Quaternion& q, float& degree, Vector3& vec)
{
	q.ToRadianAxis(degree, vec);
}
static void Quaternion_Slerp(Quaternion& q, const Quaternion& q1, const Quaternion& q2, float dt)
{
	q = Quaternion::Slerp(q1, q2, dt);
}
static void Quaternion_Lerp(Quaternion& q, const Quaternion& q1, const Quaternion& q2, float dt)
{
	q = Quaternion::Lerp(q1, q2, dt);
}
/********************************************************************/
static void Matrix4_ToQuaternion(const Matrix4& m, Quaternion& q)
{
	q = m.GetRotate();
}
static void Matrix4_Inverse(const Matrix4& m, Matrix4& o)
{
	o = m.Inverse();
}
static void Matrix4_Transpose(const Matrix4& m, Matrix4& o)
{
	o = m.Transpose();
}
static void Matrix4_InverseTranspose(const Matrix4& m, Matrix4& o)
{
	o = m.InverseTranspose();
}
static void Matrix4_TransformVector(const Matrix4& m, const Vector3& i, Vector3& o)
{
	o = m.TransformVector(i);
}
static void Matrix4_TransformPoint(const Matrix4& m, const Vector3& i, Vector3& o)
{
	o = m.TransformPoint(i);
}
static void Matrix4_FromTranslate(Matrix4& m, const Vector3& position)
{
	m = Matrix4::FromTranslate(position);
}
static void Matrix4_FromScale(Matrix4& m, const Vector3& scale)
{
	m = Matrix4::FromScale(scale);
}
static void Matrix4_FromQuaternion(Matrix4& m, const Quaternion& q)
{
	m = Matrix4::FromRotate(q);
}
static void Matrix4_FromRadianAxis(Matrix4& m, const float& degree, const Vector3& axis)
{
	m = Matrix4::FromRotate(axis, degree);
}
static void Matrix4_FromTransform(Matrix4& m, const Vector3& position, const Vector3& scale, const Quaternion& orientation)
{
	m.Recompose(position, orientation, scale);
}
static void Matrix4_LookAt(Matrix4& m, const Vector3 &pos, const Vector3 &at, const Vector3 &up)
{
	m = Matrix4::LookAt(pos, at, up);
}
static void Matrix4_Perspective(Matrix4& m, float fov, float aspect, float zNear, float zFar)
{
	m = Matrix4::Perspective(fov, aspect, zNear, zFar);
}
static void Matrix4_Ortho(Matrix4& m, float w, float h, float zNear, float zFar)
{
	m = Matrix4::Ortho(w, h, zNear, zFar);
}
DC_END_NAMESPACE