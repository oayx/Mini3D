 
/*****************************************************************************
* Author： hannibal
* Date：2020/9/24
* Description：
*****************************************************************************/
#pragma once

#include "Physics_def.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL RigidBody final : public Component
{
	friend class GameObject;
	DECLARE_OBJECT_CLONE;
	DECLARE_OBJECT_SERIALIZE(RigidBody);
	FRIEND_CONSTRUCT_DESTRUCT(RigidBody);
	BEGIN_DERIVED_REFECTION_TYPE(RigidBody, Component)
		CTORS(DEFAULT_CTOR(RigidBody))
		ADD_EDITOR_COMPONENT("Physics/RigidBody")
	END_REFECTION_TYPE;

protected:
	virtual void Start()override;
	virtual void OnDrawEditor()override;

public:
	void	SetLinearVelocity(const Vector3& v);
	void	SetAngularVelocity(const Vector3& v);
	Vector3 GetLinearVelocity();
	Vector3	GetAngularVelocity();

	void	AddForceAtPosition(const Vector3& force, const Vector3& point);//力
	void	ApplyForce(const Vector3& force);//力
	void	AppleTorque(const Vector3& torque);//扭矩
	void	ApplyImpulse(const Vector3& impulse, const Vector3& point);//冲量

	bool	IsSleep();
	void	SetActive(bool b);

public://需要序列化的属性
	void	SetMass(float mass);
	float	GetMass()const { return _mass; }

	void	SetLinearDrag(float drag) { _linearDrag = drag >= 0 ? drag : 0; }
	float	GetLinearDrag()const { return _linearDrag; }

	void	SetAngularDrag(float drag) { _angularDrag = drag >= 0 ? drag : 0; }
	float	GetAngularDrag()const { return _angularDrag; }

	void	SetUseGravity(bool b) { _useGravity = b; }
	bool	IsUseGravity()const { return _useGravity; }

	void	SetKinematic(bool b) { _isKinematic = b; }
	bool	IsKinematic()const { return _isKinematic; }

	void	SetFixedPosition(const Vector3& vec) { _fixedPosition = vec; }
	bool	IsFixedPositionX()const { return _fixedPosition.x; }
	bool	IsFixedPositionY()const { return _fixedPosition.y; }
	bool	IsFixedPositionZ()const { return _fixedPosition.z; }
	void	SetFixedRotation(const Vector3& vec) { _fixedRotation = vec; }
	bool	IsFixedRotationX()const { return _fixedRotation.x; }
	bool	IsFixedRotationY()const { return _fixedRotation.y; }
	bool	IsFixedRotationZ()const { return _fixedRotation.z; }

	void				SetCollisionDetection(CollisionDetection type) { _collisionDetection = type; }
	CollisionDetection	GetCollisionDetection()const { return _collisionDetection; }

private:
	btRigidBody*		_rigidBody = nullptr;

private:
	float				_mass = 1;
	float				_linearDrag = 0;
	float				_angularDrag = 0.05f;
	bool				_useGravity = true;
	bool				_isKinematic = false;			//当我们想自己控制物体的运动情况的时候，即不受Bullet内部的力学计算影响位置
	Vector3				_fixedPosition = Vector3::zero;
	Vector3				_fixedRotation = Vector3::zero;
	CollisionDetection	_collisionDetection = CollisionDetection::Discrete;
};
DC_END_NAMESPACE
