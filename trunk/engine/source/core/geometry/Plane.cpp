#include "Plane.h" 
#include "../math/Aabb.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_REFECTION_TYPE(Plane);
Plane::Side Plane::GetSide (const Vector3& point) const noexcept
{
	float distance = GetDistance(point);

	if (distance < 0.0)
		return Plane::NEGATIVE_SIDE;

	if (distance > 0.0)
		return Plane::POSITIVE_SIDE;

	return Plane::NO_SIDE;
}
Plane::Side Plane::GetSide(const Aabb &aabb)const noexcept
{
	Vector3 kCenterVec = aabb.GetCenter();
	Vector3 kHalfSizeVec = aabb.GetHalfSize();

	float fCenterDist = GetDistance(kCenterVec);
	float fHalfSizeDist = normal.AbsDot(kHalfSizeVec);

	if (fCenterDist < -fHalfSizeDist)
	{
		return Plane::NEGATIVE_SIDE;
	}
	if (fCenterDist > fHalfSizeDist)
	{
		return Plane::POSITIVE_SIDE;
	}
	return Plane::NO_SIDE;
}
bool Plane::IsBoxNegative(const Vector3* pCorners) const noexcept
{
	if (GetSide(pCorners[0]) == Plane::NEGATIVE_SIDE &&
		GetSide(pCorners[1]) == Plane::NEGATIVE_SIDE &&
		GetSide(pCorners[2]) == Plane::NEGATIVE_SIDE &&
		GetSide(pCorners[3]) == Plane::NEGATIVE_SIDE &&
		GetSide(pCorners[4]) == Plane::NEGATIVE_SIDE &&
		GetSide(pCorners[5]) == Plane::NEGATIVE_SIDE &&
		GetSide(pCorners[6]) == Plane::NEGATIVE_SIDE &&
		GetSide(pCorners[7]) == Plane::NEGATIVE_SIDE)
	{
		return true;
	}
	return false;
}
DC_END_NAMESPACE
