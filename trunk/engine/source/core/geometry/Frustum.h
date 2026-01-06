 
/*****************************************************************************
* Author： hannibal
* Date：2009/11/13
* Description：视锥体
*****************************************************************************/
#pragma once

#include "Plane.h"

DC_BEGIN_NAMESPACE
class Aabb;
class Sphere;
class Vector3;
class Matrix4;
/********************************************************************/
//裁剪平面
enum class FrustumPlane
{
	Near = 0,
	Far = 1,
	Left = 2,
	Right = 3,
	Top = 4,
	Bottom = 5,
	Max = 6
};
DECLARE_ENUM_OPERATORS(FrustumPlane);

//可见性
enum class FrustumVisible
{
	None = 0,
	Partial,
	Full
};
DECLARE_ENUM_OPERATORS(FrustumVisible);

//视锥体
class ENGINE_DLL Frustum final : public object
{
	DEFAULT_CREATE(Frustum);
	FRIEND_CONSTRUCT_DESTRUCT(Frustum);
	BEGIN_REFECTION_TYPE(Frustum)
	END_FINAL_REFECTION_TYPE;
	
public:
	Plane GetPlane(int index)const noexcept;

	FrustumVisible GetVisibility(const Aabb &bound)const noexcept;
	FrustumVisible GetVisibility(const Sphere &sphere)const noexcept;

	void Calculate(const Matrix4& matView, const Matrix4& matProj) noexcept;

private:
	Plane _planes[6];
};
DC_END_NAMESPACE
