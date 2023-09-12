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
btDynamicsWorld* Physics::m_btPhysxWorld = nullptr;
btAxisSweep3* Physics::m_btBroadphase;
btCollisionDispatcher* Physics::m_btDispatcher;
btSequentialImpulseConstraintSolver* Physics::m_btSolver;
btDefaultCollisionConfiguration* Physics::m_btCollisionConfiguration;
ContactListener* Physics::_contactListener = nullptr;
PhysicsDebugDraw* Physics::_debugDraw = nullptr;
Vector3	Physics::_gravity = Vector3(0, -9.8f, 0);
int Physics::_subSteps = 10;
bool Physics::_autoSimulate = true;
bool Physics::_useUnscaledTime = false;
bool Physics::_autoSyncTransform = true;
float Physics::_defaultFriction = 0.6f;
float Physics::_defaultRestitution = 0.0f;
Vector3 Physics::_worldCenter = Vector3::zero;
Vector3 Physics::_worldExtent = Vector3(500.0f, 500.0f, 500.0f);
IMPL_DERIVED_REFECTION_TYPE(Physics, Object);
void Physics::Initialize()
{
	Load();

	// 冲突配置包含内存的默认设置，冲突设置。高级用户可以创建自己的配置。
	m_btCollisionConfiguration = new btDefaultCollisionConfiguration();
	// 使用默认的冲突调度程序。对于并行处理，您可以使用不同的分派器(参见Extras/BulletMultiThreaded)
	m_btDispatcher = new btCollisionDispatcher(m_btCollisionConfiguration);

	m_btBroadphase = new btAxisSweep3(TobtVec3(_worldCenter - _worldExtent), TobtVec3(_worldCenter + _worldExtent));
	m_btBroadphase->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());

	// 默认约束求解器。对于并行处理，您可以使用不同的解决程序(参见Extras/BulletMultiThreaded)
	m_btSolver = new btSequentialImpulseConstraintSolver;

	m_btPhysxWorld = new btDiscreteDynamicsWorld(m_btDispatcher, m_btBroadphase, m_btSolver, m_btCollisionConfiguration);
	m_btPhysxWorld->getDispatchInfo().m_allowedCcdPenetration = 0.0001f;
	m_btPhysxWorld->setGravity(TobtVec3(_gravity));

	_contactListener = ContactListener::Create();
}
void Physics::Destroy()
{
	SAFE_DELETE(_debugDraw);
	SAFE_DELETE(_contactListener);
	if (m_btDispatcher)
	{
		delete m_btDispatcher;
		m_btDispatcher = nullptr;
	}
	if (m_btBroadphase)
	{
		delete m_btBroadphase;
		m_btBroadphase = nullptr;
	}
	if (m_btSolver)
	{
		delete m_btSolver;
		m_btSolver = nullptr;
	}
	if (m_btCollisionConfiguration)
	{
		delete m_btCollisionConfiguration;
		m_btCollisionConfiguration = nullptr;
	}
	if (m_btPhysxWorld)
	{
		delete m_btPhysxWorld;
		m_btPhysxWorld = nullptr;
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
	if (m_btPhysxWorld != nullptr)
	{
		m_btPhysxWorld->stepSimulation(dt, _subSteps);
		//碰撞检查
		_contactListener->CollisionDetection();
	}
}
bool Physics::Raycast(const Vector3& origin, const Vector3& direction, float distance, RaycastHit& hit_info)
{
	return Raycast(origin, direction, distance, 0, hit_info);
}
bool Physics::Raycast(const Vector3& origin, const Vector3& direction, float distance, uint layerMask, RaycastHit& hit_info)
{
	Vector3 end = origin + direction.Normalize() * distance;
	btVector3 btStart = TobtVec3(origin);
	btVector3 btEnd = TobtVec3(end);
	btCollisionWorld::ClosestRayResultCallback btResult(btStart, btEnd);
	m_btPhysxWorld->rayTest(btStart, btEnd, btResult);
	if (btResult.hasHit() && btResult.m_collisionObject != nullptr)
	{
		Collider* collider = reinterpret_cast<BoxCollider*>(btResult.m_collisionObject->getUserPointer());
		if (collider != nullptr)
		{
			hit_info.gameobject = collider->GetGameObject();
			hit_info.point = FrombtVec3(btResult.m_hitPointWorld);
			hit_info.normal = FrombtVec3(btResult.m_hitNormalWorld);
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
	m_btPhysxWorld->rayTest(btStart, btEnd, btResult);
	if (btResult.hasHit())
	{
		for (int i = 0; i < btResult.m_collisionObjects.size(); ++i)
		{
			Collider* collider = reinterpret_cast<BoxCollider*>(btResult.m_collisionObjects[i]->getUserPointer());
			if (collider != nullptr)
			{
				RaycastHit hit_info;
				hit_info.gameobject = collider->GetGameObject();
				hit_info.point = FrombtVec3(btResult.m_hitPointWorld[i]);
				hit_info.normal = FrombtVec3(btResult.m_hitNormalWorld[i]);
				list.Add(hit_info);
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
	m_btPhysxWorld->setDebugDrawer(b ? _debugDraw : nullptr);
}
void Physics::SetGravity(const Vector3& gravity)
{
	if (m_btPhysxWorld == nullptr)return;
	_gravity = gravity;
	m_btPhysxWorld->setGravity(TobtVec3(gravity));
}
void Physics::SetAllowSleep(bool sleep)
{
	if (m_btPhysxWorld == nullptr)return;
}
void Physics::SleepAll(bool sleep)
{
}
void Physics::Load()
{
	String full_path = Resource::GetFullDataPath(PhysicsFilePath);
	SerializeRead transfer(full_path);
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
	String full_path = Resource::GetFullDataPath(PhysicsFilePath);
	SerializeWrite transfer(full_path);
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
	Physics::m_btPhysxWorld->debugDrawWorld();
}
DC_END_NAMESPACE
