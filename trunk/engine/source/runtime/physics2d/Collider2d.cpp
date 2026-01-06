#include "Collider2d.h"
#include "RigidBody2d.h"
#include "Physics2d.h"
#include "Box2D/Box2D.h"
#include "runtime/component/Component.inl"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(Collider2d, Component);
Collider2d::Collider2d()
{
	_friction = Physics2d::GetDefaultFriction();
	_restitution = Physics2d::GetDefaultRestitution();
}
Collider2d::~Collider2d()
{
	if (_b2Body)
	{
		if(Physics2d::_physxWorld)
			Physics2d::_physxWorld->DestroyBody(_b2Body);
		_b2Body = nullptr;
	}
}
void Collider2d::Start()
{
	base::Start();

	b2BodyType type = b2_staticBody;//static物体在模拟时不会运动，就好像它具有无穷大的质量。在Box2D内部，会将static物体的质量存储为零。static物体可以让用户手动移动，它速度为零，另外也不会和其它static或kinematic物体相互碰撞。
	RigidBody2d* rigidBody = this->GetComponent<RigidBody2d>();
	if (rigidBody != nullptr)
	{
		if (_isTrigger)
			type = b2_kinematicBody;//kinematic物体在模拟时以一定速度运动, 但不受力的作用。它们可以让用户手动移动，但通常的做法是设置一定速度。kinematic物体的行为表现就好像它具有无穷大的质量，Box2D将它的质量存储为零。kinematic物体并不会和其它static或kinematic物体相互碰撞。
		else
			type = b2_dynamicBody;//dynamic物体被完全模拟。它们可以让用户手动移动，但通常它们都是受力的作用而运动。dynamic物体可以和其它所有类型的物体相互碰撞。dynamic物体的质量总是有限大的，非零的。如果你试图将它的质量设置为零，它会自动地将质量修改成一千克
	}
	Vector2 pos = Vector2(GetTransform()->GetPosition().x, GetTransform()->GetPosition().y) + _offset;
	float degree = GetTransform()->GetRotation().ToEuler().z;
	b2BodyDef body_def;
	body_def.type = type;
	body_def.position.Set(pos.x / PTM_RATIO, pos.y / PTM_RATIO);
	body_def.angle = Math::Deg2Rad * degree;
	body_def.userData = this;
	if (rigidBody != nullptr)
	{
		//if (rigidBody->IsAutoMass())
		body_def.gravityScale = rigidBody->IsUseGravity() ? rigidBody->GetGravityScale() : 0.0f;
		body_def.linearDamping = rigidBody->GetLinearDrag();
		body_def.angularDamping = rigidBody->GetAngularDrag();
		body_def.allowSleep = rigidBody->IsAllowSleep();
		body_def.fixedRotation = rigidBody->IsFixedRotation();
		body_def.bullet = rigidBody->GetCollisionDetection() == CollisionDetection2d::Continuous ? true : false;
	}
	_b2Body = Physics2d::_physxWorld->CreateBody(&body_def);
}
Object* Collider2d::Clone(Object* newObj)
{
	base::Clone(newObj);
	Collider2d* obj = dynamic_cast<Collider2d*>(newObj);
	if (!obj)return newObj;

	obj->SetIsTrigger(_isTrigger);
	obj->SetFriction(_friction);
	obj->SetRestitution(_restitution);
	obj->SetOffset(_offset);
	obj->SetCategoryBits(_categoryBits);
	obj->SetMaskBits(_maskBits);
	obj->SetGroupIndex(_groupIndex);

	return obj;
}
void Collider2d::ModifyPosition(const Vector2& pos)
{
	if (_b2Body == nullptr)return;
	float degree = GetTransform()->GetRotation().ToEuler().z;
	_b2Body->SetTransform(b2Vec2(pos.x / PTM_RATIO, pos.y / PTM_RATIO), Math::Deg2Rad * degree);
}
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(BoxCollider2d, Collider2d);
void BoxCollider2d::Start()
{
	base::Start();

	b2PolygonShape shape;
	UpdateSize(&shape);

	float density = 0;
	RigidBody2d* rigidBody = this->GetComponent<RigidBody2d>();
	if (rigidBody)
	{
		density = rigidBody->GetDensity();
	}
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &shape;
	fixtureDef.density = density;
	fixtureDef.friction = _friction;
	fixtureDef.restitution = _restitution;
	fixtureDef.filter.categoryBits = _categoryBits;
	fixtureDef.filter.maskBits = _maskBits;
	fixtureDef.filter.groupIndex = _groupIndex;
	_b2Body->CreateFixture(&fixtureDef);
}
Object* BoxCollider2d::Clone(Object* newObj)
{
	base::Clone(newObj);
	BoxCollider2d* obj = dynamic_cast<BoxCollider2d*>(newObj);
	if (!obj)return newObj;

	obj->SetScale(_localSize.width, _localSize.height);

	return obj;
}
void BoxCollider2d::SetScale(float w, float h)
{ 
	_localSize.Set(w, h);
	if (_b2Body != nullptr)
	{
		Transform* transform = this->GetTransform();
		Vector3 boxSize = transform->GetUnscaleBoundingBox().GetHalfSize();
		Size size(boxSize.x * transform->GetScale().x, boxSize.y * transform->GetScale().y);
		b2PolygonShape* shape = dynamic_cast<b2PolygonShape*>(_b2Body->GetFixtureList()[0].GetShape());
		UpdateSize(shape);
	}
}
void BoxCollider2d::UpdateSize(b2PolygonShape* shape)
{
	if (!shape)return;

	Transform* transform = this->GetTransform();
	Vector3 boxSize = transform->GetUnscaleBoundingBox().GetHalfSize();
	Size size(boxSize.x * transform->GetScale().x, boxSize.y * transform->GetScale().y);
	shape->SetAsBox((size.width * _localSize.width) / PTM_RATIO, (size.height * _localSize.height) / PTM_RATIO);
}
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(CircleCollider2d, Collider2d);
void CircleCollider2d::Start()
{
	base::Start();

	b2CircleShape shape;
	UpdateSize(&shape);

	float density = 0;
	RigidBody2d* rigidBody = GetGameObject()->GetComponent<RigidBody2d>();
	if (rigidBody != nullptr)
	{
		density = rigidBody->GetDensity();
	}
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &shape;
	fixtureDef.density = density;
	fixtureDef.friction = _friction;
	fixtureDef.restitution = _restitution;
	fixtureDef.filter.categoryBits = _categoryBits;
	fixtureDef.filter.maskBits = _maskBits;
	fixtureDef.filter.groupIndex = _groupIndex;
	_b2Body->CreateFixture(&fixtureDef);
}
Object* CircleCollider2d::Clone(Object* newObj)
{
	base::Clone(newObj);
	CircleCollider2d* obj = dynamic_cast<CircleCollider2d*>(newObj);
	if (!obj)return newObj;

	obj->SetScale(_radius);

	return obj;
}
void CircleCollider2d::SetScale(float r)
{ 
	_radius = r;
	if (_b2Body != nullptr)
	{
		b2CircleShape* shape = dynamic_cast<b2CircleShape*>(_b2Body->GetFixtureList()[0].GetShape());
		UpdateSize(shape);
	}
}
void CircleCollider2d::UpdateSize(b2CircleShape* shape)
{
	if (!shape)return;

	Transform* transform = GetGameObject()->GetTransform();
	Vector3 size = transform->GetUnscaleBoundingBox().GetHalfSize();;
	float radius = ((size.x * transform->GetScale().x + size.y * transform->GetScale().y) * 0.5f) * _radius;
	shape->m_radius = radius / PTM_RATIO;
}
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(PolygonCollider2d, Collider2d);
void PolygonCollider2d::Start()
{
	base::Start();

	RigidBody2d* rigidBody = this->GetComponent<RigidBody2d>();
	b2PolygonShape shape;
	shape.Set((b2Vec2*)(&_vertexs[0]), _vertexs.Size());

	float density = 0;
	if (rigidBody != nullptr)
	{
		density = rigidBody->GetDensity();
	}
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &shape;
	fixtureDef.density = density;
	fixtureDef.friction = _friction;
	fixtureDef.restitution = _restitution;
	fixtureDef.filter.categoryBits = _categoryBits;
	fixtureDef.filter.maskBits = _maskBits;
	fixtureDef.filter.groupIndex = _groupIndex;
	_b2Body->CreateFixture(&fixtureDef);
}
Object* PolygonCollider2d::Clone(Object* newObj)
{
	base::Clone(newObj);
	PolygonCollider2d* obj = dynamic_cast<PolygonCollider2d*>(newObj);
	if (!obj)return newObj;

	obj->Set(_vertexs);

	return obj;
}
void PolygonCollider2d::Set(const Vector2v& vertexs)
{
	AssertEx(vertexs.Size() <= b2_maxPolygonVertices, "vertex count out range");
	_vertexs.Resize(vertexs.Size());
	Memory::Copy(&_vertexs[0], &vertexs[0], vertexs.Size() * sizeof(Vector2));
	if (_b2Body != nullptr)
	{
		b2PolygonShape* shape = dynamic_cast<b2PolygonShape*>(_b2Body->GetFixtureList()[0].GetShape());
		shape->Set((b2Vec2*)(&_vertexs[0]), _vertexs.Size());
	}
}
DC_END_NAMESPACE
