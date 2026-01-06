 
/*****************************************************************************************************/
// @author hannibal
// @date   2020/09/11
// @brief  2d物理世界
/*****************************************************************************************************/
#pragma once

#include "Physics2d_def.h"
#include "ContactListener2d.h"
#include "RayCastCallback2d.h"

DC_BEGIN_NAMESPACE
class Camera;
class ContactListener2d;
class PhysicsDebugDraw2d;
class PhysicsComponent2d;
/********************************************************************/
class ENGINE_DLL Physics2d final : public Object
{
	friend class Application;
	friend class RigidBody2d;
	friend class Collider2d;
	friend class CircleCollider2d;
	friend class PhysicsComponent2d;
	friend class Project;
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(Physics2d);
	BEGIN_DERIVED_REFECTION_TYPE(Physics2d, Object)
	END_REFECTION_TYPE;

private:
	static void		Initialize();
	static void		Destroy();
	/**
	 *  Box2D 中有一些数学代码构成的积分器(integrator)，积分器在离散的时间点上模拟物理方程，它将
	 *	游戏动画循环一同运行。所以我们需要为 Box2D 选取一个时间步，通常来说游戏物理引擎需要至少60Hz
	 *	的速度，也就是 1/60 的时间步。
	 */
	static void		Update();
	static void		Simulate(float dt);

public:
	static bool		Raycast(const Vector2& origin, const Vector2& direction, float distance, RaycastHit2D& hitInfo);
	static bool		Raycast(const Vector2& origin, const Vector2& direction, float distance, uint layerMask, RaycastHit2D& hitInfo);
	static bool		RaycastAll(const Vector2& origin, const Vector2& direction, float distance, Vector<RaycastHit2D>& list);
	static bool		RaycastAll(const Vector2& origin, const Vector2& direction, float distance, uint layerMask, Vector<RaycastHit2D>& list);

	static void		EnableDebug(bool b);

	static void		SetGravity(const Vector2& gravity);
	static Vector2	GetGravity();
	static void		SetAllowSleep(bool sleep);
	static void		SleepAll(bool sleep);

	/**
	 * 除了积分器之外，Box2D 中还有约束求解器(constraint solver)。约束求解器用于解决模拟中的所有
	 * 约束，一次一个。单个的约束会被完美的求解，然而当我们求解一个约束的时候，我们就会稍微耽误另
	 * 一个。要得到良好的解，我们需要迭代所有约束多次。建议的 Box2D 迭代次数是 10 次
	 */
	static void		SetIterations(int velocity, int position) { _velocityIterations = velocity; _positionIterations = position; }

	static void		SetDefaultPhysxAttribute(float friction, float restitution) { _defaultFriction = Math::Clamp01(friction); _defaultRestitution = Math::Clamp01(restitution); }
	static float	GetDefaultFriction() { return _defaultFriction; }
	static float	GetDefaultRestitution() { return _defaultRestitution; }

	static bool		SetUnitRatio(float ratio) { if (ratio < 0)return false; PTM_RATIO = ratio; return true; }

	static void		SetAutoSimulate(bool b) { _autoSimulate = b; }
	static void		UseUnscaledTime(bool b) { _useUnscaledTime = b; }

private:
	inline static b2World* _physxWorld = nullptr;
	inline static ContactListener2d* _contactListener2d = nullptr;
	inline static PhysicsDebugDraw2d* _debugDraw = nullptr;

	inline static int	_velocityIterations = 10;
	inline static int	_positionIterations = 10;

	inline static bool	_autoSimulate = true;		//是否自动模拟
	inline static bool	_useUnscaledTime = false;	//是否受DeltaTime影响，默认true

	inline static float	_defaultFriction = 0.6f;
	inline static float	_defaultRestitution = 0.0f;
};

class PhysicsComponent2d : public Component
{
	friend class GameObject;
	friend class Physics2d;
	BEGIN_DERIVED_REFECTION_TYPE(PhysicsComponent2d, Component)
	END_REFECTION_TYPE;

protected:
	virtual void OnEnable()override;
	virtual void OnDisable()override;
	virtual void OnDrawGizmos(Camera* camera)override;
};
DC_END_NAMESPACE
