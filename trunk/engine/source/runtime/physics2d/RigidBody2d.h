 
/*****************************************************************************
* Author： hannibal
* Date：2020/9/17
* Description：
*****************************************************************************/
#pragma once

#include "Physics2d_def.h"
class b2Body;

DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL RigidBody2d Final : public Component
{
	friend class GameObject;
	DECLARE_OBJECT_CLONE;
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(RigidBody2d);
	FRIEND_CONSTRUCT_DESTRUCT(RigidBody2d);
	BEGIN_DERIVED_REFECTION_TYPE(RigidBody2d, Component)
		CTORS(DEFAULT_CTOR(RigidBody2d))
		//ADD_EDITOR_COMPONENT("Physics2D/RigidBody")
	END_DERIVED_REFECTION_TYPE;

public:
	b2Body*	GetBody();

	void	SetLinearVelocity(const Vector2& v);
	void	SetAngularVelocity(float v);

	void	AddForceAtPosition(const Vector2& force, const Vector2& point);//力
	void	ApplyForce(const Vector2& force);//力
	void	AppleTorque(float torque);//扭矩
	void	ApplyLinearImpulse(const Vector2& impulse, const Vector2& point);//冲量
	void	ApplyAngularImpulse(float impulse);

public://序列化属性
	void	SetDensity(float density) { _density = density; _isAutoMass = false; }
	float	GetDensity()const { return _density; }

	void	UseGravity(bool b);
	bool	IsUseGravity()const { return _gravityEnable; }

	void	SetGravityScale(float scale) { _gravityScale = scale >= 0 ? scale : 0; }
	float	GetGravityScale()const { return _gravityScale; }

	void	SetLinearDrag(float drag) { _linearDrag = drag >= 0 ? drag : 0; }
	float	GetLinearDrag()const { return _linearDrag; }

	void	SetAngularDrag(float drag) { _angularDrag = drag >= 0 ? drag : 0; }
	float	GetAngularDrag()const { return _angularDrag; }

	void	UseAutoMass(bool b) { _isAutoMass = b; }
	bool	IsAutoMass()const { return _isAutoMass; }

	void	SetFixedRotation(bool b) { _fixedRotation = b; }
	bool	IsFixedRotation()const { return _fixedRotation; }

	void	SetAllowSleep(bool b) { _allowSleep = b; }
	bool	IsAllowSleep()const { return _allowSleep; }
	bool	IsSleep();

	void	SetCollisionDetection(CollisionDetection2d type) { _collisionDetection2d = type; }
	CollisionDetection2d GetCollisionDetection()const { return _collisionDetection2d; }

protected:
	bool	_gravityEnable = true;
	float	_gravityScale = 1;
	float	_linearDrag = 0;
	float	_angularDrag = 0.05f;
	float	_density = 1;
	bool	_isAutoMass = true;
	bool	_allowSleep = true;
	bool	_fixedRotation = false;
	CollisionDetection2d _collisionDetection2d = CollisionDetection2d::Discrete;
};
DC_END_NAMESPACE
