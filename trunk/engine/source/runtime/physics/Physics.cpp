#include "Physics.h"
#include "Collider.h"
#include "PhysicsDebugDraw.h"
#include "runtime/component/GameObject.h"
#include "runtime/resources/Resources.h"
#include "runtime/renderable/Gizmos.h"
#include "runtime/scene/SceneManager.h"
#include "runtime/Application.h"
#include "external/bullet/BulletCollision/CollisionDispatch/btGhostObject.h"
 
DC_BEGIN_NAMESPACE
static const String PhysicsFilePath = "settings/PhysicsSettings.asset";
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(Physics, Object);
void Physics::Initialize()
{
	Load();

	// 冲突配置包含内存的默认设置，冲突设置。高级用户可以创建自己的配置。
	_btCollisionConfiguration = new btDefaultCollisionConfiguration();
	// 使用默认的冲突调度程序。对于并行处理，您可以使用不同的分派器(参见Extras/BulletMultiThreaded)
	_btDispatcher = new btCollisionDispatcher(_btCollisionConfiguration);

	_btBroadphase = new btAxisSweep3(TobtVec3(_worldCenter - _worldExtent), TobtVec3(_worldCenter + _worldExtent));
	_btBroadphase->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());

	// 默认约束求解器。对于并行处理，您可以使用不同的解决程序(参见Extras/BulletMultiThreaded)
	_btSolver = new btSequentialImpulseConstraintSolver;

	_btPhysxWorld = new btDiscreteDynamicsWorld(_btDispatcher, _btBroadphase, _btSolver, _btCollisionConfiguration);
	_btPhysxWorld->getDispatchInfo().m_allowedCcdPenetration = 0.0001f;
	_btPhysxWorld->setGravity(TobtVec3(_gravity));

	_contactListener = ContactListener::Create();
}
void Physics::Destroy()
{
	SAFE_DELETE(_debugDraw);
	SAFE_DELETE(_contactListener);
	if (_btDispatcher)
	{
		delete _btDispatcher;
		_btDispatcher = nullptr;
	}
	if (_btBroadphase)
	{
		delete _btBroadphase;
		_btBroadphase = nullptr;
	}
	if (_btSolver)
	{
		delete _btSolver;
		_btSolver = nullptr;
	}
	if (_btCollisionConfiguration)
	{
		delete _btCollisionConfiguration;
		_btCollisionConfiguration = nullptr;
	}
	if (_btPhysxWorld)
	{
		delete _btPhysxWorld;
		_btPhysxWorld = nullptr;
	}
}
void Physics::Update()
{
	if (!Application::IsPlay() || Application::IsPause())
		return;
	if (_autoSimulate)
	{
		//时间缩放
		float dt = _useUnscaledTime ? Time::GetUnscaledDeltaTime() : Time::GetDeltaTime();
		Simulate(dt);
	}
}
void Physics::Simulate(float dt)
{
	DC_PROFILE_FUNCTION;
	if (_btPhysxWorld != nullptr)
	{
		_btPhysxWorld->stepSimulation(dt, _subSteps);
		//碰撞检查
		_contactListener->CollisionDetection();
	}
}
bool Physics::Raycast(const Vector3& origin, const Vector3& direction, float distance, RaycastHit& hitInfo)
{
	return Raycast(origin, direction, distance, 0, hitInfo);
}
bool Physics::Raycast(const Vector3& origin, const Vector3& direction, float distance, uint layerMask, RaycastHit& hitInfo)
{
	Vector3 end = origin + direction.Normalize() * distance;
	btVector3 btStart = TobtVec3(origin);
	btVector3 btEnd = TobtVec3(end);
	btCollisionWorld::ClosestRayResultCallback btResult(btStart, btEnd);
	_btPhysxWorld->rayTest(btStart, btEnd, btResult);
	if (btResult.hasHit() && btResult.m_collisionObject != nullptr)
	{
		Collider* collider = reinterpret_cast<BoxCollider*>(btResult.m_collisionObject->getUserPointer());
		if (collider != nullptr)
		{
			hitInfo.gameobject = collider->GetGameObject();
			hitInfo.point = FrombtVec3(btResult.m_hitPointWorld);
			hitInfo.normal = FrombtVec3(btResult.m_hitNormalWorld);
			return true;
		}
	}

	return false;
}
bool Physics::RaycastAll(const Vector3& origin, const Vector3& direction, float distance, Vector<RaycastHit>& list)
{
	return RaycastAll(origin, direction, distance, 0, list);
}
bool Physics::RaycastAll(const Vector3& origin, const Vector3& direction, float distance, uint layerMask, Vector<RaycastHit>& list)
{
	Vector3 end = origin + direction.Normalize() * distance;
	btVector3 btStart = TobtVec3(origin);
	btVector3 btEnd = TobtVec3(end);
	btCollisionWorld::AllHitsRayResultCallback btResult(btStart, btEnd);
	_btPhysxWorld->rayTest(btStart, btEnd, btResult);
	if (btResult.hasHit())
	{
		for (int i = 0; i < btResult.m_collisionObjects.size(); ++i)
		{
			Collider* collider = reinterpret_cast<BoxCollider*>(btResult.m_collisionObjects[i]->getUserPointer());
			if (collider != nullptr)
			{
				RaycastHit hitInfo;
				hitInfo.gameobject = collider->GetGameObject();
				hitInfo.point = FrombtVec3(btResult.m_hitPointWorld[i]);
				hitInfo.normal = FrombtVec3(btResult.m_hitNormalWorld[i]);
				list.Add(hitInfo);
			}
		}
	}
	return !list.IsEmpty();
}
void Physics::EnableDebug(bool b)
{
	if (b)
	{
		if (_debugDraw == nullptr)
		{
			_debugDraw = PhysicsDebugDraw::Create();
		}
		if(!SceneManager::GetEngineObject()->GetComponent<PhysicsComponent>())
			SceneManager::GetEngineObject()->AddComponent<PhysicsComponent>();
	}
	else
	{
		SceneManager::GetEngineObject()->RemoveComponent<PhysicsComponent>();
	}
	_btPhysxWorld->setDebugDrawer(b ? _debugDraw : nullptr);
}
void Physics::SetGravity(const Vector3& gravity)
{
	if (_btPhysxWorld == nullptr)return;
	_gravity = gravity;
	_btPhysxWorld->setGravity(TobtVec3(gravity));
}
void Physics::SetAllowSleep(bool sleep)
{
	if (_btPhysxWorld == nullptr)return;
}
void Physics::SleepAll(bool sleep)
{
}
void Physics::Load()
{
	String fullPath = Resource::GetFullDataPath(PhysicsFilePath);
	SerializeRead transfer(fullPath);
	{
		TRANSFER_SIMPLE(_gravity);
		TRANSFER_SIMPLE(_subSteps);
		TRANSFER_SIMPLE(_autoSimulate);
		TRANSFER_SIMPLE(_useUnscaledTime);
		TRANSFER_SIMPLE(_autoSyncTransform);
		TRANSFER_SIMPLE(_defaultFriction);
		TRANSFER_SIMPLE(_defaultRestitution);
		TRANSFER_SIMPLE(_worldCenter);
		TRANSFER_SIMPLE(_worldExtent);
	}
}
void Physics::Save()
{
	String fullPath = Resource::GetFullDataPath(PhysicsFilePath);
	SerializeWrite transfer(fullPath);
	{
		TRANSFER_SIMPLE(_gravity);
		TRANSFER_SIMPLE(_subSteps);
		TRANSFER_SIMPLE(_autoSimulate);
		TRANSFER_SIMPLE(_useUnscaledTime);
		TRANSFER_SIMPLE(_autoSyncTransform);
		TRANSFER_SIMPLE(_defaultFriction);
		TRANSFER_SIMPLE(_defaultRestitution);
		TRANSFER_SIMPLE(_worldCenter);
		TRANSFER_SIMPLE(_worldExtent);
	}
}
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(PhysicsComponent, Component);
void PhysicsComponent::OnEnable()
{
	Function<void(Camera*)> handle;
	handle.Bind<PhysicsComponent, &PhysicsComponent::OnDrawGizmos>(this);
	Gizmos::Register(handle);
}
void PhysicsComponent::OnDisable()
{
	Function<void(Camera*)> handle;
	handle.Bind<PhysicsComponent, &PhysicsComponent::OnDrawGizmos>(this);
	Gizmos::Unregister(handle);
}
void PhysicsComponent::OnDrawGizmos(Camera* camera)
{
	Physics::_btPhysxWorld->debugDrawWorld();
}
DC_END_NAMESPACE
