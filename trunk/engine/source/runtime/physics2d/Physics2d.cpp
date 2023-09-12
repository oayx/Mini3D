#include "Physics2d.h"
#include "Collider2d.h"
#include "PhysicsDebugDraw2d.h"
#include "runtime/component/GameObject.h"
#include "runtime/renderable/Gizmos.h"
#include "runtime/scene/SceneManager.h"
#include "runtime/Application.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
b2World* Physics2d::_physxWorld = nullptr;
ContactListener2d* Physics2d::_contactListener2d = nullptr;
PhysicsDebugDraw2d* Physics2d::_debugDraw = nullptr;
int Physics2d::_velocityIterations = 10;
int Physics2d::_positionIterations = 10;
bool Physics2d::_autoSimulate = true;
bool Physics2d::_useUnscaledTime = false;
float Physics2d::_defaultFriction = 0.6f;
float Physics2d::_defaultRestitution = 0.0f;
IMPL_DERIVED_REFECTION_TYPE(Physics2d, Object);
void Physics2d::Initialize()
{
	_physxWorld = DBG_NEW b2World(b2Vec2(0, -9.8f));
	_physxWorld->SetAllowSleeping(true);
	_physxWorld->SetContinuousPhysics(true);

	_contactListener2d = ContactListener2d::Create();
	_physxWorld->SetContactListener(_contactListener2d);
}
void Physics2d::Destroy()
{
	SAFE_DELETE(_debugDraw);
	SAFE_DELETE(_contactListener2d);
	SAFE_DELETE(_physxWorld);
}
void Physics2d::Update()
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
void Physics2d::Simulate(float dt)
{
	if (_physxWorld != nullptr)
	{
		_physxWorld->Step(dt, _velocityIterations, _positionIterations);

		for (b2Body* body = _physxWorld->GetBodyList(); nullptr != body; body = body->GetNext())
		{
			if (nullptr != body->GetUserData())
			{
				Collider2d* collider = reinterpret_cast<Collider2d*>(body->GetUserData());
				if (!collider)continue;
				Transform* transform = collider->GetTransform();
				transform->SetPosition(Vector3(body->GetPosition().x * PTM_RATIO, body->GetPosition().y * PTM_RATIO, transform->GetPosition().z));
				transform->SetRotation(Quaternion((body->GetAngle()) * Math::Rad2Deg, Vector3::forward));
			}
		}
	}
}
bool Physics2d::Raycast(const Vector2& origin, const Vector2& direction, float distance, RaycastHit2D& hit_info)
{
	return Raycast(origin, direction, distance, 0, hit_info);
}
bool Physics2d::Raycast(const Vector2& origin, const Vector2& direction, float distance, uint layerMask, RaycastHit2D& hit_info)
{
	RayCastClosestCallback callback(std::move(hit_info), layerMask);
	Vector2 end = origin + direction.Normalize() * distance;
	_physxWorld->RayCast(&callback, b2Vec2(origin.x / PTM_RATIO, origin.y / PTM_RATIO), b2Vec2(end.x / PTM_RATIO, end.y / PTM_RATIO));
	hit_info = std::move(callback._info);
	return callback._hit;
}
bool Physics2d::RaycastAll(const Vector2& origin, const Vector2& direction, float distance, Vector<RaycastHit2D>& list)
{
	return RaycastAll(origin, direction, distance, 0, list);
}
bool Physics2d::RaycastAll(const Vector2& origin, const Vector2& direction, float distance, uint layerMask, Vector<RaycastHit2D>& list)
{
	RayCastMultipleCallback callback(std::move(list), layerMask);
	Vector2 end = origin + direction.Normalize() * distance;
	_physxWorld->RayCast(&callback, b2Vec2(origin.x / PTM_RATIO, origin.y / PTM_RATIO), b2Vec2(end.x / PTM_RATIO, end.y / PTM_RATIO));
	list = std::move(callback._list);
	return !list.IsEmpty();
}
void Physics2d::EnableDebug(bool b)
{
	if (b)
	{
		if (_debugDraw == nullptr)_debugDraw = PhysicsDebugDraw2d::Create();
		_debugDraw->AppendFlags(b2Draw::e_shapeBit);
		_physxWorld->SetDebugDraw(_debugDraw);

		if (!SceneManager::GetEngineObject()->GetComponent<PhysicsComponent2d>())
			SceneManager::GetEngineObject()->AddComponent<PhysicsComponent2d>();
	}
	else
	{
		_physxWorld->SetDebugDraw(nullptr);
		SceneManager::GetEngineObject()->RemoveComponent<PhysicsComponent2d>();
	}
}
void Physics2d::SetGravity(const Vector2& gravity)
{
	if (_physxWorld == nullptr)return;
	_physxWorld->SetGravity(b2Vec2(gravity.x, gravity.y));
}
Vector2 Physics2d::GetGravity()
{
	if (_physxWorld == nullptr)return Vector2::zero;
	const b2Vec2& v = _physxWorld->GetGravity();
	return Vector2(v.x, v.y);
}
void Physics2d::SetAllowSleep(bool sleep)
{
	if (_physxWorld == nullptr)return;
	_physxWorld->SetAllowSleeping(sleep);
}
void Physics2d::SleepAll(bool sleep)
{
	for (b2Body* body = _physxWorld->GetBodyList(); nullptr != body; body = body->GetNext())
	{
		body->SetAwake(!sleep);
	}
}
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(PhysicsComponent2d, Component);
void PhysicsComponent2d::OnEnable()
{
	Function<void(Camera*)> handle;
	handle.Bind<PhysicsComponent2d, &PhysicsComponent2d::OnDrawGizmos>(this);
	Gizmos::Register(handle);
}
void PhysicsComponent2d::OnDisable()
{
	Function<void(Camera*)> handle;
	handle.Bind<PhysicsComponent2d, &PhysicsComponent2d::OnDrawGizmos>(this);
	Gizmos::Unregister(handle);
}
void PhysicsComponent2d::OnDrawGizmos(Camera* camera)
{
	Physics2d::_physxWorld->DrawDebugData();
}
DC_END_NAMESPACE
