#include "CharacterController.h"
#include "RigidBody.h"
#include "Physics.h"
#include "runtime/component/Component.inl"
#include "editor/common/ECommonComponent.h"
#include "external/bullet/BulletDynamics/Character/btKinematicCharacterController.h"
#include "external/bullet/BulletCollision/CollisionDispatch/btGhostObject.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(CharacterController, Collider);
CharacterController::CharacterController()
{

}
CharacterController::~CharacterController()
{
	if (_ghostObject)
	{
		Physics::GetWorld()->removeCollisionObject(_ghostObject);
		delete _ghostObject;
		_ghostObject = nullptr;
	}
	if (_shape)
	{
		delete _shape;
		_shape = nullptr;
	}
	if (_characterController)
	{
		Physics::GetWorld()->removeCharacter(_characterController);
		delete _characterController;
		_characterController = nullptr;
	}
}
void CharacterController::Start()
{
	base::Start();

	Transform* transform = GetGameObject()->GetTransform();
	const Aabb& box = transform->GetUnscaleBoundingBox();
	Vector3 half_size = box.GetHalfSize();
	_shape = new btCapsuleShape(half_size.x, half_size.y);

	btTransform bt_transform;//NOTE:不处理旋转
	bt_transform.setIdentity();
	bt_transform.setOrigin(TobtVec3(transform->GetPosition()) + btVector3(0.0f, _shape->getHalfHeight() * 4.0f, 0.0f));
	_ghostObject = new btPairCachingGhostObject();
	_ghostObject->setWorldTransform(bt_transform);
	_ghostObject->setCollisionShape(_shape);
	_ghostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);
	_ghostObject->setUserPointer(this);
	Physics::GetWorld()->addCollisionObject(_ghostObject, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);

	_characterController = new btKinematicCharacterController(_ghostObject, _shape, _stepHeight);
	SetGravity(Physics::GetGravity().y);
	if (GetGameObject()->ActiveInHierarchy())
	{
		Physics::GetWorld()->addCharacter(_characterController);
	}
}
void CharacterController::OnEnable()
{
	base::OnEnable();
	if (_characterController)
	{
		Physics::GetWorld()->addCharacter(_characterController);
	}
}
void CharacterController::OnDisable()
{
	base::OnDisable();
	if (_characterController)
	{
		Physics::GetWorld()->removeCharacter(_characterController);
	}
}
void CharacterController::Update()
{
	base::Update();
	if (_ghostObject)
	{
		Transform* transform = GetGameObject()->GetTransform();

		const btTransform& bt_transform = _ghostObject->getWorldTransform();
		const btVector3& bt_pos = bt_transform.getOrigin();
		transform->SetPosition(FrombtVec3(bt_pos - btVector3(0.0f, _shape->getHalfHeight() * 4.0f,0.0f)));
		//NOTE:不处理旋转
	}
}
Object* CharacterController::Clone(Object* new_obj)
{
	base::Clone(new_obj);
	CharacterController* obj = dynamic_cast<CharacterController*>(new_obj);
	if (!obj)return new_obj;

	obj->SetStepHeight(_stepHeight);

	return obj;
}
void CharacterController::Move(const Vector3& dir)
{
	if (dir.Lenght() < 0.001f)return;
	if (_characterController)
	{
		_characterController->setVelocityForTimeInterval(TobtVec3(dir), Time::GetDeltaTime());
	}
}
void CharacterController::SetGravity(float gravity)
{
	if (_characterController)
	{
		_characterController->setGravity(-gravity);
	}
}
void CharacterController::SetStepHeight(float step)
{
	_stepHeight = step;
}
bool CharacterController::OnGround()const
{
	if (_characterController)
	{
		return _characterController->onGround();
	}
	return false;
}
void CharacterController::OnDrawEditor()
{
	base::OnDrawEditor();
}
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(CharacterController);
template<class TransferFunction> inline
void CharacterController::Transfer(TransferFunction& transfer, void* ptr)
{
	base::Transfer(transfer, ptr);

	TRANSFER_SIMPLE(_stepHeight);
}
DC_END_NAMESPACE
