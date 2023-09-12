#include "Vector3.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_REFECTION_TYPE(Vector3);
const Vector3 Vector3::zero(0.0f, 0.0f, 0.0f);
const Vector3 Vector3::right(1.0f,0.0f,0.0f);
const Vector3 Vector3::up(0.0f,1.0f,0.0f);
const Vector3 Vector3::forward(0.0f,0.0f,1.0f);
const Vector3 Vector3::one(1.0f,1.0f,1.0f);
const Vector3 Vector3::infinity(std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity());

Vector3 Vector3::Normalize()
{
	float fLenght = Lenght();
	if (Math::FloatEqual(fLenght, 0.0f))
	{
		x = y = z = 0.0f;
	}
	else
	{
		float fInv = 1.0f / fLenght;
		x = x * fInv;
		y = y * fInv;
		z = z * fInv;
	}
	return *this;
}
Vector3 Vector3::Normalize(float len)
{
	Normalize();
	x = x * len;
	y = y * len;
	z = z * len;
	return *this;
}
void Vector3::Projection(const Vector3 &vec, Vector3 &vecParallel, Vector3 &vecVertical) const
{
	vecParallel = vec * ((*this).Dot(vec)/vec.SquareSize());
	vecVertical = (*this) - vecParallel;
}
Vector3 Vector3::Lerp(const Vector3& from, const Vector3& to, float t, bool clamp_01)
{
	return Vector3(
		Math::Lerp(from.x, to.x, t, clamp_01),
		Math::Lerp(from.y, to.y, t, clamp_01),
		Math::Lerp(from.z, to.z, t, clamp_01));
}
DC_END_NAMESPACE
