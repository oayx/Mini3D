#include "RigidBody.h"
#include "Collider.h"
#include "Physics.h"
#include "external/Box2D/Box2D.h"
#include "runtime/component/Component.inl"
#include "editor/common/ECommonComponent.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(RigidBody, Component);
void RigidBody::Start()
{
	base::Start();
	Collider* collider = this->GetComponent<Collider>();
	AssertEx(collider, "");
	_rigidBody = collider->GetBody();

	SetMass(_mass);
	_rigidBody->setActivationState(DISABLE_DEACTIVATION);
	_rigidBody->setGravity(_useGravity ? TobtVec3(Physics::GetGravity()) : btVector3(0.0f, 0.0f, 0.0f));
	_rigidBody->setDamping(_linearDrag, _angularDrag);
	if (_isKinematic)
	{
		_rigidBody->setCollisionFlags(_rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		_rigidBody->setActivationState(DISABLE_DEACTIVATION);
	}
	else
	{
		_rigidBody->setCollisionFlags(_rigidBody->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT);
		_rigidBody->setActivationState(ACTIVE_TAG);
	}
}
void RigidBody::SetLinearVelocity(const Vector3& v)
{
	_rigidBody->setLinearVelocity(TobtVec3(v));
}
void RigidBody::SetAngularVelocity(const Vector3& v)
{
	_rigidBody->setAngularVelocity(TobtVec3(v));
}
Vector3 RigidBody::GetLinearVelocity()
{
	return FrombtVec3(_rigidBody->getLinearVelocity());
}
Vector3	RigidBody::GetAngularVelocity()
{
	return FrombtVec3(_rigidBody->getAngularVelocity());
}
void RigidBody::AddForceAtPosition(const Vector3& force, const Vector3& point)
{
	_rigidBody->applyForce(TobtVec3(force), TobtVec3(point));
}
void RigidBody::ApplyForce(const Vector3& force)
{
	_rigidBody->applyCentralForce(TobtVec3(force));
}
void RigidBody::AppleTorque(const Vector3& torque)
{
	_rigidBody->applyTorque(TobtVec3(torque));
}
void RigidBody::ApplyImpulse(const Vector3& impulse, const Vector3& point)
{
	_rigidBody->applyImpulse(TobtVec3(impulse), TobtVec3(point));
}
bool RigidBody::IsSleep()
{
	return true;
}
void RigidBody::SetActive(bool b)
{
	_rigidBody->setActivationState(b ? ACTIVE_TAG : WANTS_DEACTIVATION);
}
Object* RigidBody::Clone(Object* newObj)
{
	base::Clone(newObj);
	RigidBody* obj = dynamic_cast<RigidBody*>(newObj);
	if (!obj)return newObj;

	obj->SetMass(_mass);
	obj->SetLinearDrag(_linearDrag);
	obj->SetAngularDrag(_angularDrag);
	obj->SetUseGravity(_useGravity);
	obj->SetKinematic(_isKinematic);
	obj->SetFixedPosition(_fixedPosition);
	obj->SetFixedRotation(_fixedRotation);
	obj->SetCollisionDetection(_collisionDetection);

	return obj;
}
void RigidBody::SetMass(float mass)
{ 
	_mass = mass;
	btVector3 inertia(0.0f, 0.0f, 0.0f);
	Collider* collider = this->GetComponent<Collider>();
	if (mass > 0.0f && collider)
	{
		inertia = collider->CalculateInertia(mass);
	}
	_rigidBody->setMassProps(mass, inertia);
}
void RigidBody::OnDrawEditor()
{
	base::OnDrawEditor();

	{
		ImGuiEx::Label("Mass");
		if (ImGui::InputScalar("##Mass", ImGuiDataType_Float, &_mass, nullptr, "%.3f"))
		{
		}

		ImGuiEx::Label("Drag");
		if (ImGui::InputScalar("##Drag", ImGuiDataType_Float, &_linearDrag, nullptr, "%.3f"))
		{
		}

		ImGuiEx::Label("Angular Drag");
		if (ImGui::InputScalar("##Angular Drag", ImGuiDataType_Float, &_angularDrag, nullptr, "%.3f"))
		{
		}

		ImGuiEx::Label("Use Gravity");
		if (ImGui::Checkbox("##Use Gravity", &_useGravity))
		{
		}

		ImGuiEx::Label("Is Kinematic");
		if (ImGui::Checkbox("##IsKinematic", &_isKinematic))
		{
		}
	}
	{
		const char* szFlags[] = { "Discrete", "Continuous" };
		ImGuiEx::Label("Collision Detection");
		int currentSelect = (int)_collisionDetection;
		if (ImGui::Combo("##CollisionDetection", &currentSelect, szFlags, ARRAY_SIZE(szFlags)))
		{
			_collisionDetection = CollisionDetection(currentSelect);
		}
	}
	{
		ImGui::TextUnformatted("Constraints");
		{
			ImGui::PushID(1);
			ImGuiEx::Label("    Freeze Position");
			bool is_pos_x = _fixedPosition.x == 0;
			if (ImGui::Checkbox("X", &is_pos_x))
			{
				_fixedPosition.x = !is_pos_x;
			}
			ImGui::SameLine();
			bool is_pos_y = _fixedPosition.y == 0;
			if (ImGui::Checkbox("Y", &is_pos_y))
			{
				_fixedPosition.y = !is_pos_y;
			}
			ImGui::SameLine();
			bool is_pos_z = _fixedPosition.z == 0;
			if (ImGui::Checkbox("Z", &is_pos_z))
			{
				_fixedPosition.z = !is_pos_z;
			}
			ImGui::PopID();
		}
		{
			ImGui::PushID(2);
			ImGuiEx::Label("    Freeze Rotation");
			bool is_rot_x = _fixedRotation.x == 0;
			if (ImGui::Checkbox("X", &is_rot_x))
			{
				_fixedRotation.x = !is_rot_x;
			}
			ImGui::SameLine();
			bool is_rot_y = _fixedRotation.y == 0;
			if (ImGui::Checkbox("Y", &is_rot_y))
			{
				_fixedRotation.y = !is_rot_y;
			}
			ImGui::SameLine();
			bool is_rot_z = _fixedRotation.z == 0;
			if (ImGui::Checkbox("Z", &is_rot_z))
			{
				_fixedRotation.z = !is_rot_z;
			}
			ImGui::PopID();
		}
	}
}
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(RigidBody);
template<class TransferFunction> inline
void RigidBody::Transfer(TransferFunction& transfer, void* ptr)
{
	base::Transfer(transfer, ptr);

	TRANSFER_SIMPLE(_mass);
	TRANSFER_SIMPLE(_linearDrag);
	TRANSFER_SIMPLE(_angularDrag);
	TRANSFER_SIMPLE(_useGravity);
	TRANSFER_SIMPLE(_isKinematic);
	TRANSFER_SIMPLE(_fixedPosition);
	TRANSFER_SIMPLE(_fixedRotation);
	TRANSFER_ENUM(_collisionDetection);
}
DC_END_NAMESPACE