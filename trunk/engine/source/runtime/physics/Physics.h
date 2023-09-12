 
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
class ENGINE_DLL Physics Final : public Object
{
	friend class Application;
	friend class RigidBody;
	friend class Collider;
	friend class ContactListener;
	friend class Project;
	friend class PhysicsComponent;
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(Physics);
	BEGIN_DERIVED_REFECTION_TYPE(Physics, Object)
	END_DERIVED_REFECTION_TYPE;

private:
	static void		Initialize();
	static void		Destroy();
	static void		Update();
	static void		Simulate(float dt);

public:
	static bool		Raycast(const Vector3& origin, const Vector3& direction, float distance, RaycastHit& hit_info);
	static bool		Raycast(const Vector3& origin, const Vector3& direction, float distance, uint layerMask, RaycastHit& hit_info);
	static bool		RaycastAll(const Vector3& origin, const Vector3& direction, float distance, Vector<RaycastHit>& list);
	static bool		RaycastAll(const Vector3& origin, const Vector3& direction, float distance, uint layerMask, Vector<RaycastHit>& list);

	static void		EnableDebug(bool b);
	static bool		IsEnableDebug() { return m_btPhysxWorld->getDebugDrawer(); }

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
	static			btDynamicsWorld* GetWorld() { return m_btPhysxWorld; }

private:
	static			btDynamicsWorld*	m_btPhysxWorld;
	static			btAxisSweep3*		m_btBroadphase;
	static			btCollisionDispatcher* m_btDispatcher;
	static			btSequentialImpulseConstraintSolver* m_btSolver;
	static			btDefaultCollisionConfiguration* m_btCollisionConfiguration;

	static			ContactListener*	_contactListener;
	static			PhysicsDebugDraw*	_debugDraw;

	static Vector3	_gravity;
	static int		_subSteps;
	static bool		_autoSimulate;		//是否自动模拟
	static bool		_useUnscaledTime;	//是否受DeltaTime影响，默认true
	static bool		_autoSyncTransform;//是否自动同步变换

	static float	_defaultFriction;
	static float	_defaultRestitution;

	static Vector3	_worldCenter;
	static Vector3	_worldExtent;
};

class PhysicsComponent : public Component
{
	friend class GameObject;
	BEGIN_DERIVED_REFECTION_TYPE(PhysicsComponent, Component)
	END_DERIVED_REFECTION_TYPE;

protected:
	virtual void OnEnable()override;
	virtual void OnDisable()override;
	virtual void OnDrawGizmos(Camera* camera)override;
};
DC_END_NAMESPACE
