#include "RigidBody2d.h"
#include "Collider2d.h"
#include "Physics2d.h"
#include "Box2D/Box2D.h"
#include "runtime/component/Component.inl"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(RigidBody2d, Component);
b2Body*	RigidBody2d::GetBody()
{
	Collider2d* collider = this->GetComponent<Collider2d>();
	if (collider != nullptr)
	{
		return collider->GetBody();
	}
	return nullptr;
}
void RigidBody2d::SetLinearVelocity(const Vector2& v)
{
	if (GetBody() != nullptr)
	{
		GetBody()->SetLinearVelocity(b2Vec2(v.x, v.y));
	}
}
void RigidBody2d::SetAngularVelocity(float v)
{
	if (GetBody() != nullptr)
	{
		GetBody()->SetAngularVelocity(v);
	}
}
void RigidBody2d::AddForceAtPosition(const Vector2& force, const Vector2& point)
{
	if (GetBody() != nullptr)
	{
		GetBody()->ApplyForce(b2Vec2(force.x, force.y), b2Vec2(point.x, point.y), true);
	}
}
void RigidBody2d::ApplyForce(const Vector2& force)
{
	if (GetBody() != nullptr)
	{
		GetBody()->ApplyForce(b2Vec2(force.x, force.y), true);
	}
}
void RigidBody2d::AppleTorque(float torque)
{
	if (GetBody() != nullptr)
	{
		GetBody()->ApplyTorque(torque, true);
	}
}
void RigidBody2d::ApplyLinearImpulse(const Vector2& impulse, const Vector2& point)
{
	if (GetBody() != nullptr)
	{
		GetBody()->ApplyLinearImpulse(b2Vec2(impulse.x, impulse.y), b2Vec2(point.x, point.y), true);
	}
}
void RigidBody2d::ApplyAngularImpulse(float impulse)
{
	if (GetBody() != nullptr)
	{
		GetBody()->ApplyAngularImpulse(impulse, true);
	}
}
Object* RigidBody2d::Clone(Object* newObj)
{
	base::Clone(newObj);
	RigidBody2d* obj = dynamic_cast<RigidBody2d*>(newObj);
	if (!obj)return newObj;

	obj->SetDensity(_density);
	obj->UseGravity(_gravityEnable);
	obj->SetGravityScale(_gravityScale);
	obj->SetLinearDrag(_linearDrag);
	obj->SetAngularDrag(_angularDrag);
	obj->UseAutoMass(_isAutoMass);
	obj->SetFixedRotation(_fixedRotation);
	obj->SetAllowSleep(_allowSleep);
	obj->SetCollisionDetection(_collisionDetection2d);

	return obj;
}
void RigidBody2d::UseGravity(bool b)
{ 
	_gravityEnable = b;
	if (GetBody() != nullptr)
	{
		GetBody()->SetGravityScale(b ? _gravityScale : 0.0f);
	}
}
bool RigidBody2d::IsSleep()
{
	if (GetBody() != nullptr)
	{
		return GetBody()->IsAwake();
	}
	return true;
}
DC_END_NAMESPACE
