 
/*****************************************************************************
* Author： hannibal
* Date：2020/9/24
* Description：物理刚体
*****************************************************************************/
#pragma once

#include "Physics_def.h"

#pragma warning(push)
#pragma warning(disable : 4275)
DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL Collider : public Component, public btMotionState
{
	friend class GameObject;
	friend class RigidBody;
	DISALLOW_COPY_ASSIGN(Collider);
	DECLARE_OBJECT_CLONE;
	DECLARE_OBJECT_SERIALIZE(Collider);
	BEGIN_DERIVED_REFECTION_TYPE(Collider, Component)
	END_REFECTION_TYPE;

protected:
	Collider();
	~Collider();

public:
	virtual void getWorldTransform(btTransform& worldTrans) const override;
	virtual void setWorldTransform(const btTransform& worldTrans) override;

protected:
	virtual void Start()override;
	virtual void OnEnable()override;
	virtual void OnDisable()override;
	virtual bool CanRemove()override;
	virtual void OnTransformChanged()override;
	virtual void OnDrawEditor()override;
	virtual btCollisionShape* CreateShape() = 0;
	virtual btVector3 CalculateInertia(float mass) const;//根据密度自动计算惯性  

public:
	btRigidBody* GetBody()const { return _rigidBody; }   

	void	SetIsTrigger(bool b) { _isTrigger = b; }
	bool	IsTrigger()const { return _isTrigger; }
	void	SetOffset(const Vector3& offset) { _offset = offset; }
	void	SetFriction(float friction);
	void	SetRestitution(float restitution);

	void	SetMaskBits(short bits) { _maskBits = bits; }
	void	SetGroupIndex(short group) { _groupIndex = group; }


protected:
	void	UpdateScale();
	void	UpdatePositionRotate();

protected:
	ColliderType _colliderType = ColliderType::None;
	btRigidBody* _rigidBody = nullptr;
	btCollisionShape* _shape = nullptr;

	Vector3 _localScale = Vector3::one;
	Vector3 _transformScale = Vector3::zero;

protected:
	bool	_isTrigger = false;
	Vector3 _offset = Vector3::zero;
	float	_friction = 0;
	float	_restitution = 0;

	short	_maskBits = 0x7FFF;
	short	_groupIndex = 1;
};
/********************************************************************/
class ENGINE_DLL BoxCollider final : public Collider
{
	friend class GameObject;
	FRIEND_CONSTRUCT_DESTRUCT(BoxCollider);
	DISALLOW_COPY_ASSIGN(BoxCollider);
	DECLARE_OBJECT_SERIALIZE(BoxCollider);
	BEGIN_DERIVED_REFECTION_TYPE(BoxCollider, Collider)
		CTORS(DEFAULT_CTOR(BoxCollider))
		ADD_EDITOR_COMPONENT("Physics/BoxCollider")
	END_REFECTION_TYPE;

	BoxCollider() { _colliderType = ColliderType::Box; }

protected:
	virtual void OnDrawEditor()override;
	virtual btCollisionShape* CreateShape()override;

public:
	void SetScale(const Vector3& scale) { _localScale = scale; UpdateScale(); }
};
/********************************************************************/
class ENGINE_DLL SphereCollider final : public Collider
{
	friend class GameObject;
	FRIEND_CONSTRUCT_DESTRUCT(SphereCollider);
	DISALLOW_COPY_ASSIGN(SphereCollider);
	DECLARE_OBJECT_SERIALIZE(SphereCollider);
	BEGIN_DERIVED_REFECTION_TYPE(SphereCollider, Collider)
		CTORS(DEFAULT_CTOR(SphereCollider))
		ADD_EDITOR_COMPONENT("Physics/SphereCollider")
	END_REFECTION_TYPE;

	SphereCollider() { _colliderType = ColliderType::Sphere; }

protected:
	virtual void OnDrawEditor()override;
	virtual btCollisionShape* CreateShape()override;

public:
	void SetScale(float r) { _localScale.Set(r, r, r); UpdateScale();}
};
/********************************************************************/
class ENGINE_DLL CapsuleCollider final : public Collider
{
	friend class GameObject;
	FRIEND_CONSTRUCT_DESTRUCT(CapsuleCollider);
	DISALLOW_COPY_ASSIGN(CapsuleCollider);
	DECLARE_OBJECT_SERIALIZE(CapsuleCollider);
	BEGIN_DERIVED_REFECTION_TYPE(CapsuleCollider, Collider)
		CTORS(DEFAULT_CTOR(CapsuleCollider))
		ADD_EDITOR_COMPONENT("Physics/CapsuleCollider")
	END_REFECTION_TYPE;

	CapsuleCollider() { _colliderType = ColliderType::Capsule; }

protected:
	virtual void OnDrawEditor()override;
	virtual btCollisionShape* CreateShape()override;

public:
	void SetScale(float radius, float height) { _localScale.Set(radius, height, radius); UpdateScale();}
};
/********************************************************************/
class ENGINE_DLL CylinderCollider final : public Collider
{
	friend class GameObject;
	FRIEND_CONSTRUCT_DESTRUCT(CylinderCollider);
	DISALLOW_COPY_ASSIGN(CylinderCollider);
	DECLARE_OBJECT_SERIALIZE(CylinderCollider);
	BEGIN_DERIVED_REFECTION_TYPE(CylinderCollider, Collider)
		CTORS(DEFAULT_CTOR(CylinderCollider))
		ADD_EDITOR_COMPONENT("Physics/CylinderCollider")
	END_REFECTION_TYPE;

	CylinderCollider() { _colliderType = ColliderType::Cylinder; }

protected:
	virtual void OnDrawEditor()override;
	virtual btCollisionShape* CreateShape()override;

public:
	void SetScale(const Vector3& scale) { _localScale = scale; UpdateScale();}
};
/********************************************************************/
class ENGINE_DLL PlaneCollider final : public Collider
{
	friend class GameObject;
	FRIEND_CONSTRUCT_DESTRUCT(PlaneCollider);
	DISALLOW_COPY_ASSIGN(PlaneCollider);
	DECLARE_OBJECT_SERIALIZE(PlaneCollider);
	BEGIN_DERIVED_REFECTION_TYPE(PlaneCollider, Collider)
		CTORS(DEFAULT_CTOR(PlaneCollider))
		ADD_EDITOR_COMPONENT("Physics/PlaneCollider")
	END_REFECTION_TYPE;

	PlaneCollider() { _colliderType = ColliderType::Plane; }

protected:
	virtual void OnDrawEditor()override;
	virtual btCollisionShape* CreateShape()override;
};
/********************************************************************/
class ENGINE_DLL TerrainCollider final : public Collider
{
	friend class GameObject;
	FRIEND_CONSTRUCT_DESTRUCT(TerrainCollider);
	DISALLOW_COPY_ASSIGN(TerrainCollider);
	DECLARE_OBJECT_SERIALIZE(TerrainCollider);
	BEGIN_DERIVED_REFECTION_TYPE(TerrainCollider, Collider)
		CTORS(DEFAULT_CTOR(TerrainCollider))
		ADD_EDITOR_COMPONENT("Physics/TerrainCollider")
	END_REFECTION_TYPE;

	TerrainCollider() { _colliderType = ColliderType::Terrain; }

protected:
	virtual bool CanAdd(GameObject* object)override;
	virtual void OnDrawEditor()override;
	virtual btCollisionShape* CreateShape()override;

protected:
	Vector<float> _heightData;
};
/********************************************************************/
class ENGINE_DLL MeshCollider final : public Collider
{
	friend class GameObject;
	FRIEND_CONSTRUCT_DESTRUCT(MeshCollider);
	DISALLOW_COPY_ASSIGN(MeshCollider);
	DECLARE_OBJECT_SERIALIZE(MeshCollider);
	BEGIN_DERIVED_REFECTION_TYPE(MeshCollider, Collider)
		CTORS(DEFAULT_CTOR(MeshCollider))
		ADD_EDITOR_COMPONENT("Physics/MeshCollider")
	END_REFECTION_TYPE;

	MeshCollider() { _colliderType = ColliderType::Mesh; }
	~MeshCollider() { delete _btMesh; }

protected:
	virtual bool CanAdd(GameObject* object)override;
	virtual void OnDrawEditor()override;
	virtual btCollisionShape* CreateShape()override;
	virtual btVector3 CalculateInertia(float mass) const override;

private:
	btTriangleMesh* _btMesh = nullptr;
};
DC_END_NAMESPACE
#pragma warning(pop)