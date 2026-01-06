 
/*****************************************************************************************************/
// @author hannibal
// @date   2020/09/24
// @brief  
/*****************************************************************************************************/
#pragma once

#include "runtime/component/GameObject.h"
#include "external/bullet/btBulletDynamicsCommon.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
inline btVector3 TobtVec3(const Vector3& vec) { return btVector3(vec.x, vec.y, vec.z); }
inline Vector3 FrombtVec3(const btVector3& vec) { return Vector3(vec.x(), vec.y(), vec.z()); }
inline btQuaternion TobtQuaternion(const Quaternion& q) { return btQuaternion(q.x, q.y, q.z, q.w); }
inline Quaternion FrombtQuaternion(const btQuaternion& q) { return Quaternion(q.x(), q.y(), q.z(), q.w()); }

#define bt_maxManifoldPoints	4

class ENGINE_DLL ContactPoint
{
public:
	Vector3 normal = Vector3::zero;
	Vector3 position = Vector3::zero;
	float normalImpulse = 0.0f;
	float tangentImpulse = 0.0f;
	float separation = 0.0f;
};
class ENGINE_DLL Collision
{
public:
	Component* collider = nullptr;
	Component* otherCollider = nullptr;
	int contactCount = 0;
	ContactPoint contacts[bt_maxManifoldPoints];
};

//射线查询信息
class ENGINE_DLL RaycastHit
{
public:
	Vector3 point = Vector3::zero;
	Vector3 normal = Vector3::zero;
	GameObject* gameobject = nullptr;
};

//刚体类型
enum class ColliderType
{
	None = 0,
	Box,
	Sphere,
	Capsule,
	Cylinder,
	Plane,
	Terrain,
	Mesh,
};

//连续碰撞检查
enum class CollisionDetection
{
	Discrete = 0,
	Continuous,
};
DECLARE_ENUM_OPERATORS(CollisionDetection);

DC_END_NAMESPACE