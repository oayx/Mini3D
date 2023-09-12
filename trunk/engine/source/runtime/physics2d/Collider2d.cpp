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
	if (m_b2Body != nullptr)
	{
		Physics2d::_physxWorld->DestroyBody(m_b2Body);
		m_b2Body = nullptr;
	}
}
void Collider2d::Start()
{
	base::Start();

	b2BodyType type = b2_staticBody;//static物体在模拟时不会运动，就好像它具有无穷大的质量。在Box2D内部，会将static物体的质量存储为零。static物体可以让用户手动移动，它速度为零，另外也不会和其它static或kinematic物体相互碰撞。
	RigidBody2d* rigid_body = this->GetComponent<RigidBody2d>();
	if (rigid_body != nullptr)
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
	if (rigid_body != nullptr)
	{
		//if (rigid_body->IsAutoMass())
		body_def.gravityScale = rigid_body->IsUseGravity() ? rigid_body->GetGravityScale() : 0.0f;
		body_def.linearDamping = rigid_body->GetLinearDrag();
		body_def.angularDamping = rigid_body->GetAngularDrag();
		body_def.allowSleep = rigid_body->IsAllowSleep();
		body_def.fixedRotation = rigid_body->IsFixedRotation();
		body_def.bullet = rigid_body->GetCollisionDetection() == CollisionDetection2d::Continuous ? true : false;
	}
	m_b2Body = Physics2d::_physxWorld->CreateBody(&body_def);
}
Object* Collider2d::Clone(Object* new_obj)
{
	base::Clone(new_obj);
	Collider2d* obj = dynamic_cast<Collider2d*>(new_obj);
	if (!obj)return new_obj;

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
	if (m_b2Body == nullptr)return;
	float degree = GetTransform()->GetRotation().ToEuler().z;
	m_b2Body->SetTransform(b2Vec2(pos.x / PTM_RATIO, pos.y / PTM_RATIO), Math::Deg2Rad * degree);
}
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(BoxCollider2d, Collider2d);
void BoxCollider2d::Start()
{
	base::Start();

	b2PolygonShape shape;
	UpdateSize(&shape);

	float density = 0;
	RigidBody2d* rigid_body = this->GetComponent<RigidBody2d>();
	if (rigid_body)
	{
		density = rigid_body->GetDensity();
	}
	b2FixtureDef fixture_def;
	fixture_def.shape = &shape;
	fixture_def.density = density;
	fixture_def.friction = _friction;
	fixture_def.restitution = _restitution;
	fixture_def.filter.categoryBits = _categoryBits;
	fixture_def.filter.maskBits = _maskBits;
	fixture_def.filter.groupIndex = _groupIndex;
	m_b2Body->CreateFixture(&fixture_def);
}
Object* BoxCollider2d::Clone(Object* new_obj)
{
	base::Clone(new_obj);
	BoxCollider2d* obj = dynamic_cast<BoxCollider2d*>(new_obj);
	if (!obj)return new_obj;

	obj->SetScale(_localSize.width, _localSize.height);

	return obj;
}
void BoxCollider2d::SetScale(float w, float h)
{ 
	_localSize.Set(w, h);
	if (m_b2Body != nullptr)
	{
		Transform* transform = this->GetTransform();
		Vector3 box_size = transform->GetUnscaleBoundingBox().GetHalfSize();
		Size size(box_size.x * transform->GetScale().x, box_size.y * transform->GetScale().y);
		b2PolygonShape* shape = dynamic_cast<b2PolygonShape*>(m_b2Body->GetFixtureList()[0].GetShape());
		UpdateSize(shape);
	}
}
void BoxCollider2d::UpdateSize(b2PolygonShape* shape)
{
	if (!shape)return;

	Transform* transform = this->GetTransform();
	Vector3 box_size = transform->GetUnscaleBoundingBox().GetHalfSize();
	Size size(box_size.x * transform->GetScale().x, box_size.y * transform->GetScale().y);
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
	RigidBody2d* rigid_body = GetGameObject()->GetComponent<RigidBody2d>();
	if (rigid_body != nullptr)
	{
		density = rigid_body->GetDensity();
	}
	b2FixtureDef fixture_def;
	fixture_def.shape = &shape;
	fixture_def.density = density;
	fixture_def.friction = _friction;
	fixture_def.restitution = _restitution;
	fixture_def.filter.categoryBits = _categoryBits;
	fixture_def.filter.maskBits = _maskBits;
	fixture_def.filter.groupIndex = _groupIndex;
	m_b2Body->CreateFixture(&fixture_def);
}
Object* CircleCollider2d::Clone(Object* new_obj)
{
	base::Clone(new_obj);
	CircleCollider2d* obj = dynamic_cast<CircleCollider2d*>(new_obj);
	if (!obj)return new_obj;

	obj->SetScale(_radius);

	return obj;
}
void CircleCollider2d::SetScale(float r)
{ 
	_radius = r;
	if (m_b2Body != nullptr)
	{
		b2CircleShape* shape = dynamic_cast<b2CircleShape*>(m_b2Body->GetFixtureList()[0].GetShape());
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

	RigidBody2d* rigid_body = this->GetComponent<RigidBody2d>();
	BoxCollider2d* box = dynamic_cast<BoxCollider2d*>(this);
	b2PolygonShape shape;
	shape.Set((b2Vec2*)(&_vertexs[0]), _vertexs.Size());

	float density = 0;
	if (rigid_body != nullptr)
	{
		density = rigid_body->GetDensity();
	}
	b2FixtureDef fixture_def;
	fixture_def.shape = &shape;
	fixture_def.density = density;
	fixture_def.friction = _friction;
	fixture_def.restitution = _restitution;
	fixture_def.filter.categoryBits = _categoryBits;
	fixture_def.filter.maskBits = _maskBits;
	fixture_def.filter.groupIndex = _groupIndex;
	m_b2Body->CreateFixture(&fixture_def);
}
Object* PolygonCollider2d::Clone(Object* new_obj)
{
	base::Clone(new_obj);
	PolygonCollider2d* obj = dynamic_cast<PolygonCollider2d*>(new_obj);
	if (!obj)return new_obj;

	obj->Set(_vertexs);

	return obj;
}
void PolygonCollider2d::Set(const Vector2v& vertexs)
{
	AssertEx(vertexs.Size() <= b2_maxPolygonVertices, "vertex count out range");
	_vertexs.Resize(vertexs.Size());
	Memory::Copy(&_vertexs[0], &vertexs[0], vertexs.Size() * sizeof(Vector2));
	if (m_b2Body != nullptr)
	{
		b2PolygonShape* shape = dynamic_cast<b2PolygonShape*>(m_b2Body->GetFixtureList()[0].GetShape());
		shape->Set((b2Vec2*)(&_vertexs[0]), _vertexs.Size());
	}
}
DC_END_NAMESPACE
