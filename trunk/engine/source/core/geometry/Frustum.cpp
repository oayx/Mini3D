#include "Frustum.h"
#include "Sphere.h"
#include "../math/Aabb.h"
#include "../math/Matrix4.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_REFECTION_TYPE(Frustum);
Plane Frustum::GetPlane(int index)const
{
	if (index >= 0 && index < 6)
		return _planes[index];
	else
		return Plane();
}
FrustumVisible Frustum::GetVisibility(const Aabb &bound)const
{
	if (bound.IsNull())
	{
		return FrustumVisible::None;
	}

	bool all_in = true;
	for (byte iPlaneIndex = 0; iPlaneIndex < (byte)FrustumPlane::Max; iPlaneIndex++)
	{
		Plane::Side side = GetPlane(iPlaneIndex).GetSide(bound);
		if (side == Plane::NEGATIVE_SIDE) return FrustumVisible::None;
		if (side == Plane::NO_SIDE)
			all_in = false;
	}

	if (all_in)
		return FrustumVisible::Full;
	else
		return FrustumVisible::Partial;
}
FrustumVisible Frustum::GetVisibility(const Sphere &sphere)const
{
	bool all_in = true;
	for (int i = 0; i < 6; ++i)
	{
		float dis = _planes[i].GetDistance(sphere.center);
		if (dis < -sphere.radius)
			return FrustumVisible::None;
		if (fabs(dis) < sphere.radius)
			all_in = false;
	}

	if (all_in)
		return FrustumVisible::Full;
	else
		return FrustumVisible::Partial;
}
void Frustum::Calculate(const Matrix4& mat_view, const Matrix4& mat_proj)
{
	Matrix4 mat_out = mat_view * mat_proj;
	_planes[0].normal.x = mat_out._14 + mat_out._13; // Near plane
	_planes[0].normal.y = mat_out._24 + mat_out._23;
	_planes[0].normal.z = mat_out._34 + mat_out._33;
	_planes[0].d = mat_out._44 + mat_out._43;

	_planes[1].normal.x = mat_out._14 - mat_out._13; // Far plane
	_planes[1].normal.y = mat_out._24 - mat_out._23;
	_planes[1].normal.z = mat_out._34 - mat_out._33;
	_planes[1].d = mat_out._44 - mat_out._43;

	_planes[2].normal.x = mat_out._14 + mat_out._11; // Left plane
	_planes[2].normal.y = mat_out._24 + mat_out._21;
	_planes[2].normal.z = mat_out._34 + mat_out._31;
	_planes[2].d = mat_out._44 + mat_out._41;

	_planes[3].normal.x = mat_out._14 - mat_out._11; // Right plane
	_planes[3].normal.y = mat_out._24 - mat_out._21;
	_planes[3].normal.z = mat_out._34 - mat_out._31;
	_planes[3].d = mat_out._44 - mat_out._41;

	_planes[4].normal.x = mat_out._14 - mat_out._12; // Top plane
	_planes[4].normal.y = mat_out._24 - mat_out._22;
	_planes[4].normal.z = mat_out._34 - mat_out._32;
	_planes[4].d = mat_out._44 - mat_out._42;

	_planes[5].normal.x = mat_out._14 + mat_out._12; // Bottom plane
	_planes[5].normal.y = mat_out._24 + mat_out._22;
	_planes[5].normal.z = mat_out._34 + mat_out._32;
	_planes[5].d = mat_out._44 + mat_out._42;

	for (int i = 0; i < 6; i++)
	{
		float length = _planes[i].normal.Lenght();
		_planes[i].normal.Normalize();
		_planes[i].d /= length;
	}
}
DC_END_NAMESPACE
