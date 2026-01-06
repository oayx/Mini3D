 
/*****************************************************************************************************/
// @author hannibal
// @date   2020/09/24
// @brief  
/*****************************************************************************************************/
#pragma once

#include "Physics_def.h"
#include "ContactListener.h"

DC_BEGIN_NAMESPACE
class Camera;
class ContactListener;
class PhysicsDebugDraw;
class PhysicsComponent;
/********************************************************************/
class ENGINE_DLL Physics final : public Object
{
	friend class Application;
	friend class RigidBody;
	friend class Collider;
	friend class ContactListener;
	friend class Project;
	friend class PhysicsComponent;
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(Physics);
	BEGIN_DERIVED_REFECTION_TYPE(Physics, Object)
	END_REFECTION_TYPE;

private:
	static void		Initialize();
	static void		Destroy();
	static void		Update();
	static void		Simulate(float dt);

public:
	static bool		Raycast(const Vector3& origin, const Vector3& direction, float distance, RaycastHit& hitInfo);
	static bool		Raycast(const Vector3& origin, const Vector3& direction, float distance, uint layerMask, RaycastHit& hitInfo);
	static bool		RaycastAll(const Vector3& origin, const Vector3& direction, float distance, Vector<RaycastHit>& list);
	static bool		RaycastAll(const Vector3& origin, const Vector3& direction, float distance, uint layerMask, Vector<RaycastHit>& list);

	static void		EnableDebug(bool b);
	static bool		IsEnableDebug() { return _btPhysxWorld->getDebugDrawer(); }

	static void		SetGravity(const Vector3& gravity);
	static const	Vector3& GetGravity() { return _gravity; }
	static void		SetAllowSleep(bool sleep);
	static void		SleepAll(bool sleep);

	static void		SetSubSteps(int steps) { _subSteps = Math::Clamp(steps, 1, 255); }
	static int		GetSubSteps() { return _subSteps; }
	static void		SetAutoSimulate(bool b) { _autoSimulate = b; }
	static bool		IsAutoSimulate() { return _autoSimulate; }
	static void		UseUnscaledTime(bool b) { _useUnscaledTime = b; }
	static void		SetAutoSyncTransform(bool b) { _autoSyncTransform = b; }
	static bool		IsAutoSyncTransform() { return _autoSyncTransform; }

	static void		SetDefaultFriction(float friction) { _defaultFriction = Math::Clamp01(friction); }
	static void		SetDefaultRestitution(float restitution) { _defaultRestitution = Math::Clamp01(restitution); }
	static float	GetDefaultFriction() { return _defaultFriction; }
	static float	GetDefaultRestitution() { return _defaultRestitution; }

	static const	Vector3& GetWorldCenter() { return _worldCenter; }
	static const	Vector3& GetWorldExtent() { return _worldExtent; }
	static void		SetWorldCenter(const Vector3& center) { _worldCenter = center; }
	static void		SetWorldExtent(const Vector3& extent) { _worldExtent = extent; }

	static void		Load();
	static void		Save();

public:
	static			btDynamicsWorld* GetWorld() { return _btPhysxWorld; }

private:
	inline static	btDynamicsWorld* _btPhysxWorld = nullptr;
	inline static	btAxisSweep3* _btBroadphase = nullptr;
	inline static	btCollisionDispatcher* _btDispatcher = nullptr;
	inline static	btSequentialImpulseConstraintSolver* _btSolver = nullptr;
	inline static	btDefaultCollisionConfiguration* _btCollisionConfiguration = nullptr;

	inline static	ContactListener* _contactListener = nullptr;
	inline static	PhysicsDebugDraw* _debugDraw = nullptr;

	inline static Vector3	_gravity = Vector3(0, -9.8f, 0);
	inline static int		_subSteps = 10;
	inline static bool		_autoSimulate = true;		//是否自动模拟
	inline static bool		_useUnscaledTime = false;	//是否受DeltaTime影响，默认true
	inline static bool		_autoSyncTransform = true;	//是否自动同步变换

	inline static float		_defaultFriction = 0.6f;
	inline static float		_defaultRestitution = 0.0f;

	inline static Vector3	_worldCenter = Vector3::zero;
	inline static Vector3	_worldExtent = Vector3(500.0f, 500.0f, 500.0f);
};

class PhysicsComponent : public Component
{
	friend class GameObject;
	BEGIN_DERIVED_REFECTION_TYPE(PhysicsComponent, Component)
	END_REFECTION_TYPE;

protected:
	virtual void OnEnable()override;
	virtual void OnDisable()override;
	virtual void OnDrawGizmos(Camera* camera)override;
};
DC_END_NAMESPACE
