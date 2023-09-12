#include "Collider.h"
#include "RigidBody.h"
#include "Physics.h"
#include "runtime/component/Component.inl"
#include "runtime/mesh/Mesh.h"
#include "runtime/mesh/MeshRender.h"
#include "runtime/terrain/Terrain.h"
#include "editor/common/ECommonComponent.h"
#include "external/bullet/BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(Collider, Component);
Collider::Collider()
{
	_friction = Physics::GetDefaultFriction();
	_restitution = Physics::GetDefaultRestitution();
}
Collider::~Collider()
{
	if (_shape)
	{
		delete _shape;
		_shape = nullptr;
	}
	if (_rigidBody)
	{
		Physics::GetWorld()->removeRigidBody(_rigidBody);
		delete _rigidBody;
		_rigidBody = nullptr;
	}
}
void Collider::getWorldTransform(btTransform& worldTrans) const
{
	Transform* transform = _gameObject->GetTransform();

	const Vector3& pos = transform->GetPosition();
	const Quaternion& rotate = transform->GetRotation();
	worldTrans.setOrigin(TobtVec3(pos));
	worldTrans.setRotation(TobtQuaternion(rotate));
}

void Collider::setWorldTransform(const btTransform& worldTrans)
{
	if (!this->IsEnable() || !_gameObject->ActiveInHierarchy() || !Physics::IsAutoSyncTransform())
		return;

	Transform* transform = _gameObject->GetTransform();
	const btVector3& bt_pos = worldTrans.getOrigin();
	const btQuaternion& bt_rotation = worldTrans.getRotation();
	Vector3 pos = FrombtVec3(bt_pos);
	Quaternion q = Quaternion(bt_rotation.getX(), bt_rotation.getY(), bt_rotation.getZ(), bt_rotation.getW());
	RigidBody* rigid_body = this->GetComponent<RigidBody>();
	if (rigid_body)
	{
		if (rigid_body->IsFixedPositionX() || rigid_body->IsFixedPositionY() || rigid_body->IsFixedPositionZ())
		{
			const Vector3& obj_pos = transform->GetPosition();
			if (rigid_body->IsFixedPositionX())pos.x = obj_pos.x;
			if (rigid_body->IsFixedPositionY())pos.y = obj_pos.y;
			if (rigid_body->IsFixedPositionZ())pos.z = obj_pos.z;
		}

		if (rigid_body->IsFixedRotationX() || rigid_body->IsFixedRotationY() || rigid_body->IsFixedRotationZ())
		{
			Vector3 euler = q.ToEuler();
			Vector3 obj_euler = transform->GetRotation().ToEuler();
			if (rigid_body->IsFixedRotationX())euler.x = obj_euler.x;
			if (rigid_body->IsFixedRotationY())euler.y = obj_euler.y;
			if (rigid_body->IsFixedRotationZ())euler.z = obj_euler.z;
			q = Quaternion(euler);
		}
	}
	transform->SetPosition(pos);
	transform->SetRotation(q);
}
void Collider::Start()
{
	base::Start();

	//创建形状
	_shape = this->CreateShape();
	CHECK_RETURN_PTR_VOID(_shape);

	//设置密度（特殊地，密度为0时会被认为静态刚体，非0时则作为动态刚体）
	float mass = 0.0f;
	//惯性    　　　
	btVector3 inertia(0.0f, 0.0f, 0.0f);
	RigidBody* rigid_body = this->GetComponent<RigidBody>();
	if (rigid_body)
	{
		mass = rigid_body->GetMass();
		if (mass > 0)
		{
			inertia = this->CalculateInertia(mass);
		}
	}

	//根据设置信息 创建刚体
	_rigidBody = new btRigidBody(mass, this, _shape, inertia);
	_rigidBody->setFriction(_friction);
	_rigidBody->setRestitution(_restitution);
	_rigidBody->setUserPointer(this);
	//将刚体添加到 物理世界
	if (GetGameObject()->ActiveInHierarchy())
	{
		Physics::GetWorld()->addRigidBody(_rigidBody, _groupIndex, _maskBits);
	}

	GetTransform()->SetTransformDirty();
}
void Collider::OnEnable()
{
	base::OnEnable();
	if (_rigidBody)
	{
		Physics::GetWorld()->addRigidBody(_rigidBody, _groupIndex, _maskBits);
	}
}
void Collider::OnDisable()
{
	base::OnDisable();
	if (_rigidBody)
	{
		Physics::GetWorld()->removeRigidBody(_rigidBody);
	}
}
bool Collider::CanRemove()
{
	return !GetComponent<RigidBody>();
}
void Collider::OnTransformChanged()
{
	base::OnTransformChanged();
	Transform* transform = GetGameObject()->GetTransform();
	Vector3 scale = transform->GetScale();
	if (scale != _transformScale)
	{
		_transformScale = scale;
		UpdateScale();
	}
	UpdatePositionRotate();
}
btVector3 Collider::CalculateInertia(float mass)const
{
	btVector3 inertia(0.0f, 0.0f, 0.0f);
	if(mass > 0.0f && _shape)_shape->calculateLocalInertia(mass, inertia);
	return inertia;
}
Object* Collider::Clone(Object* new_obj)
{
	base::Clone(new_obj);
	Collider* obj = dynamic_cast<Collider*>(new_obj);
	if (!obj)return new_obj;

	obj->SetIsTrigger(_isTrigger);
	obj->SetOffset(_offset);
	obj->SetFriction(_friction);
	obj->SetRestitution(_restitution);
	obj->SetMaskBits(_maskBits);
	obj->SetGroupIndex(_groupIndex);

	return obj;
}
void Collider::SetFriction(float friction)
{ 
	_friction = Math::Clamp01(friction);
	if (_rigidBody)
	{
		_rigidBody->setFriction(_friction);
	}
}
void Collider::SetRestitution(float restitution)
{ 
	_restitution = Math::Clamp01(restitution);
	if (_rigidBody)
	{
		_rigidBody->setRestitution(_restitution);
	}
}
void Collider::UpdateScale()
{
	if (!_rigidBody)return;
	Transform* transform = GetGameObject()->GetTransform();
	btCollisionShape* shape = _rigidBody->getCollisionShape();
	if (shape)
	{
		Vector3 scale = transform->GetScale();
		switch (_colliderType)
		{
		case ColliderType::Sphere: scale = Vector3::one * scale.Max(); break;
		case ColliderType::Terrain: scale = Vector3(scale.x, 1.0f, scale.z); break;
		default:break;
		}
		shape->setLocalScaling(TobtVec3(_localScale * scale));
	}
}
void Collider::UpdatePositionRotate()
{
	if (!_rigidBody)return;
	Transform* transform = GetGameObject()->GetTransform();
	const Vector3& pos = transform->GetPosition();
	const Quaternion& rotate = transform->GetRotation();
	btTransform bt_transform;
	bt_transform.setIdentity();
	bt_transform.setOrigin(TobtVec3(pos));
	bt_transform.setRotation(TobtQuaternion(rotate));
	_rigidBody->setWorldTransform(bt_transform);
}
void Collider::OnDrawEditor()
{
	base::OnDrawEditor();

	if(_rigidBody)
	{
		ImGuiEx::Label("Friction");
		float min_value = 0.0f;
		float man_value = MAX_float;
		if (ImGui::DragScalar("##Friction", ImGuiDataType_Float, &_friction, 0.001f, &min_value, &man_value, "%.3f"))
		{
			_rigidBody->setFriction(_friction);
		}

		ImGuiEx::Label("Restitution");
		min_value = 0.0f;
		man_value = 1.0f;
		if (ImGui::DragScalar("##Restitution", ImGuiDataType_Float, &_restitution, 0.001f, &min_value, &man_value, "%.3f"))
		{
			_rigidBody->setRestitution(_restitution);
		}
	}
	{
		ImGuiEx::Label("MaskBits");
		int mask = _maskBits;
		if (ImGui::InputScalar("##MaskBits", ImGuiDataType_S16, &mask, nullptr))
		{
			_maskBits = mask;
		}

		ImGuiEx::Label("GroupIndex");
		int group = _groupIndex;
		if (ImGui::InputScalar("##GroupIndex", ImGuiDataType_S16, &group, nullptr))
		{
			_groupIndex = group;
		}
	}
	{
		ImGuiEx::Label("Center");
		if (ImGui::DragFloat3("##Center", _offset.p, 0.01f))
		{

		}
	}
}
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(Collider);
template<class TransferFunction> inline
void Collider::Transfer(TransferFunction& transfer, void* ptr)
{
	base::Transfer(transfer, ptr);

	TRANSFER_SIMPLE(_isTrigger);
	TRANSFER_SIMPLE(_offset);
	TRANSFER_SIMPLE(_friction);
	TRANSFER_SIMPLE(_restitution);
	TRANSFER_SIMPLE(_maskBits);
	TRANSFER_SIMPLE(_groupIndex);
}
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(BoxCollider, Collider);
btCollisionShape* BoxCollider::CreateShape()
{
	Transform* transform = GetGameObject()->GetTransform();
	const Aabb& box = transform->GetUnscaleBoundingBox();
	Vector3 half_size = box.GetHalfSize();
	return new btBoxShape(TobtVec3(half_size));
}
void BoxCollider::OnDrawEditor()
{
	base::OnDrawEditor();
	ImGuiEx::Label("Scale");
	if (ImGui::DragFloat3("##Scale", _localScale.p, 0.01f, 0.0f))
	{
		UpdateScale();
	}
}
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(BoxCollider);
template<class TransferFunction> inline
void BoxCollider::Transfer(TransferFunction& transfer, void* ptr)
{
	base::Transfer(transfer, ptr);

	TRANSFER_SIMPLE(_localScale);
}
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(SphereCollider, Collider);
btCollisionShape* SphereCollider::CreateShape()
{
	Transform* transform = GetGameObject()->GetTransform();
	const Aabb& box = transform->GetUnscaleBoundingBox();
	Vector3 half_size = box.GetHalfSize();
	float radius = half_size.x;
	if (half_size.y > radius)radius = half_size.y;
	if (half_size.z > radius)radius = half_size.z;
	return new btSphereShape(radius);
}
void SphereCollider::OnDrawEditor()
{
	base::OnDrawEditor();
	ImGuiEx::Label("Radius Scale");
	float min_value = 0.001f;
	if (ImGui::DragScalar("##Radius Scale", ImGuiDataType_Float, &_localScale.x, 0.01f, &min_value))
	{
		_localScale = Vector3(_localScale.x, _localScale.x, _localScale.x);
		UpdateScale();
	}
}
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(SphereCollider);
template<class TransferFunction> inline
void SphereCollider::Transfer(TransferFunction& transfer, void* ptr)
{
	base::Transfer(transfer, ptr);

	TRANSFER_SIMPLE(_localScale);
}
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(CapsuleCollider, Collider);
btCollisionShape* CapsuleCollider::CreateShape()
{
	Transform* transform = GetGameObject()->GetTransform();
	const Aabb& box = transform->GetUnscaleBoundingBox();
	Vector3 half_size = box.GetHalfSize();
	return new btCapsuleShape(half_size.x, half_size.y);
}
void CapsuleCollider::OnDrawEditor()
{
	base::OnDrawEditor();
	ImGuiEx::Label("Radius Scale");
	float min_value = 0.001f;
	if (ImGui::DragScalar("##Radius", ImGuiDataType_Float, &_localScale.x, 0.01f, &min_value))
	{
		UpdateScale();
	}
	ImGuiEx::Label("Height Scale");
	if (ImGui::DragScalar("##Height", ImGuiDataType_Float, &_localScale.y, 0.01f, &min_value))
	{
		UpdateScale();
	}
}
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(CapsuleCollider);
template<class TransferFunction> inline
void CapsuleCollider::Transfer(TransferFunction& transfer, void* ptr)
{
	base::Transfer(transfer, ptr);

	TRANSFER_SIMPLE(_localScale);
}
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(CylinderCollider, Collider);
btCollisionShape* CylinderCollider::CreateShape()
{
	Transform* transform = GetGameObject()->GetTransform();
	const Aabb& box = transform->GetUnscaleBoundingBox();
	Vector3 half_size = box.GetHalfSize();
	return new btCylinderShape(TobtVec3(half_size));
}
void CylinderCollider::OnDrawEditor()
{
	base::OnDrawEditor();
	ImGuiEx::Label("Scale");
	if (ImGui::DragFloat3("##Scale", _localScale.p, 0.01f, 0.0f))
	{
		UpdateScale();
	}
}
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(CylinderCollider);
template<class TransferFunction> inline
void CylinderCollider::Transfer(TransferFunction& transfer, void* ptr)
{
	base::Transfer(transfer, ptr);

	TRANSFER_SIMPLE(_localScale);
}
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(PlaneCollider, Collider);
btCollisionShape* PlaneCollider::CreateShape()
{
	btStaticPlaneShape* shape = new btStaticPlaneShape(TobtVec3(-Vector3::forward), 0.0f);
	return shape;
}
void PlaneCollider::OnDrawEditor()
{
	base::OnDrawEditor();
}
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(PlaneCollider);
template<class TransferFunction> inline
void PlaneCollider::Transfer(TransferFunction& transfer, void* ptr)
{
	base::Transfer(transfer, ptr);
}
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(TerrainCollider, Collider);
bool TerrainCollider::CanAdd(GameObject* object)
{
	return object->GetComponent<Terrain>();
}
btCollisionShape* TerrainCollider::CreateShape()
{
	Terrain* terrain = GetComponent<Terrain>();
	const float* height_data = terrain->GetHeightData();
	int rows = terrain->GetTileRows();
	int cols = terrain->GetTileCols();
	
	_heightData.Resize(rows * cols);
	for (int row = 0; row < rows; ++row)
	{//转换成bullet的右手坐标系
		Memory::Copy(&_heightData[(rows - row - 1) * cols], &height_data[row * cols], cols * sizeof(float));
	}
	float offset_y = -(terrain->GetMinHeight() + terrain->GetMaxHeight());
	btHeightfieldTerrainShape* shape = new btHeightfieldTerrainShape(
		cols, rows,
		_heightData.Data(), 1.0f,
		terrain->GetMinHeight(), terrain->GetMaxHeight(), 1, PHY_FLOAT, true);
	shape->setUseDiamondSubdivision(true);
	return shape;
}
void TerrainCollider::OnDrawEditor()
{
	base::OnDrawEditor();
}
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(TerrainCollider);
template<class TransferFunction> inline
void TerrainCollider::Transfer(TransferFunction& transfer, void* ptr)
{
	base::Transfer(transfer, ptr);

}
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(MeshCollider, Collider);
bool MeshCollider::CanAdd(GameObject* object)
{
	MeshRender* render = object->GetComponent<MeshRender>();
	return (render && render->GetMesh());
}
btCollisionShape* MeshCollider::CreateShape()
{
	MeshRender* render = GetComponent<MeshRender>();
	Mesh* mesh = render->GetMesh();
	if (!mesh)return nullptr;

	const Vector<Triangle>& triangles = mesh->GetTriangles();
	m_btMesh = new btTriangleMesh(triangles.Size() >= USHRT_MAX);
	for(const auto& triangle : triangles)
	{
		m_btMesh->addTriangle(TobtVec3(triangle.first), TobtVec3(triangle.secord), TobtVec3(triangle.third));
	}
	btBvhTriangleMeshShape* shape = new btBvhTriangleMeshShape(m_btMesh, true);
	return shape;
}
btVector3 MeshCollider::CalculateInertia(float mass) const
{
	btVector3 inertia(0.0f, 0.0f, 0.0f);

	MeshRender* render = GetComponent<MeshRender>();
	Mesh* mesh = render->GetMesh();
	if (!mesh)return inertia;

	btVector3 halfExtents = TobtVec3(render->GetComponent<Transform>()->GetUnscaleBoundingBox().GetHalfSize());
	btScalar lx = btScalar(2.0f)*(halfExtents.x());
	btScalar ly = btScalar(2.0f)*(halfExtents.y());
	btScalar lz = btScalar(2.0f)*(halfExtents.z());
	inertia.setValue(mass / (btScalar(12.0f)) * (ly*ly + lz * lz), mass / (btScalar(12.0f)) * (lx*lx + lz * lz), mass / (btScalar(12.0f)) * (lx*lx + ly * ly));
	return inertia;
}
void MeshCollider::OnDrawEditor()
{
	base::OnDrawEditor();
}
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(MeshCollider);
template<class TransferFunction> inline
void MeshCollider::Transfer(TransferFunction& transfer, void* ptr)
{
	base::Transfer(transfer, ptr);

}
DC_END_NAMESPACE
