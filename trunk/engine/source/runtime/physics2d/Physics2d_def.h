 
/*****************************************************************************************************/
// @author hannibal
// @date   2020/09/11
// @brief  2d物理世界
/*****************************************************************************************************/
#pragma once

#include "runtime/component/GameObject.h"
#include "Box2D/Box2D.h"

DC_BEGIN_NAMESPACE
class Collider2d;
/********************************************************************/
//Box2D 已被调谐使用米，千克和秒来作单位。这个值用于设置游戏世界单位与Box2D单位转换
inline static float PTM_RATIO = 1.0f;

inline b2Vec2 Tob2Vec2(const Vector2& vec) { return b2Vec2(vec.x, vec.y); }
inline Vector2 Fromb2Vec2(const b2Vec2& vec) { return Vector2(vec.x, vec.y); }

class ENGINE_DLL ContactPoint2D
{
public:
	Vector2 normal = Vector2::zero;
	Vector2 position = Vector2::zero;
	float normalImpulse = 0.0f;
	float tangentImpulse = 0.0f;
	float separation = 0.0f;
};
class ENGINE_DLL Collision2D
{
public:
	Collider2d* collider = nullptr;
	Collider2d* otherCollider = nullptr;
	int contactCount = 0;
	ContactPoint2D contacts[b2_maxManifoldPoints];
};

//射线查询信息
class ENGINE_DLL RaycastHit2D
{
public:
	Vector2 point = Vector2::zero;
	Vector2 normal = Vector2::zero;
	GameObject* gameobject = nullptr;
};

//连续碰撞检查
enum class CollisionDetection2d
{
	Discrete,
	Continuous,
};
DECLARE_ENUM_OPERATORS(CollisionDetection2d);

DC_END_NAMESPACE