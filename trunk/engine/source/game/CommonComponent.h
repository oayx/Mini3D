 
/*****************************************************************************
* Author： hannibal
* Date：2021/1/6
* Description：通用组件
*****************************************************************************/
#pragma once

#include "runtime/component/GameObject.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
//旋转
class ENGINE_DLL RotateComponent final : public Component
{
	friend class GameObject;
	FRIEND_CONSTRUCT_DESTRUCT(RotateComponent);
	BEGIN_DERIVED_REFECTION_TYPE(RotateComponent, Component)
		CTORS(DEFAULT_CTOR(RotateComponent))
	END_REFECTION_TYPE;

	Vector3 _euler = Vector3::zero;

private:
	RotateComponent() {}
	RotateComponent(const Vector3& euler) : _euler(euler) {}

	virtual void Update()override
	{
		GetTransform()->Rotate(Quaternion(_euler*Time::GetDeltaTime()));
	}

public:
	void SetRotate(const Vector3& euler) { _euler = euler; }
};
/********************************************************************/
//绕点旋转
class ENGINE_DLL RotateAroundComponent final : public Component
{
	friend class GameObject;
	FRIEND_CONSTRUCT_DESTRUCT(RotateAroundComponent);
	BEGIN_DERIVED_REFECTION_TYPE(RotateAroundComponent, Component)
		CTORS(DEFAULT_CTOR(RotateAroundComponent))
	END_REFECTION_TYPE;

	float _degree = 0.0f;
	Vector3 _targetPos = Vector3::zero;
	Vector3 _worldAxis = Vector3::up;

private:
	virtual void Update()override
	{
		GetTransform()->RotateAround(_targetPos, _worldAxis, _degree*Time::GetDeltaTime());
	}

public:
	void Set(const Vector3& point, const Vector3& world_axis, float degree) { _targetPos = point; _worldAxis = world_axis; _degree = degree; }
	void SetTargetPos(const Vector3& target) { _targetPos = target; }
	void SetAxis(const Vector3& world_axis) { _worldAxis = world_axis; }
	void SetDegree(float degree) { _degree = degree; }
};
/********************************************************************/
//回调
class ENGINE_DLL CallbackComponent final : public Component
{
	friend class GameObject;
	FRIEND_CONSTRUCT_DESTRUCT(CallbackComponent);
	BEGIN_DERIVED_REFECTION_TYPE(CallbackComponent, Component)
		CTORS(DEFAULT_CTOR(CallbackComponent))
	END_REFECTION_TYPE;

private:
	Action _updateCallback = nullptr;
	Action _enableCallback = nullptr;
	Action _disableCallback = nullptr;
	Action _destroyCallback = nullptr;
public:
	//callback->SetUpdateCallback([node] { });
	void SetUpdateCallback(const Action& callback) { _updateCallback = callback; }
	virtual void Update()override
	{
		if (_updateCallback != nullptr)_updateCallback();
	}

	void SetEnableCallback(const Action& callback) { _enableCallback = callback; }
	virtual void OnEnable()override
	{
		if (_enableCallback != nullptr)_enableCallback();
	}

	void SetDisableCallback(const Action& callback) { _disableCallback = callback; }
	virtual void OnDisable()override
	{
		if (_disableCallback != nullptr)_disableCallback();
	}

	void SetDestroyCallback(const Action& callback) { _destroyCallback = callback; }
	virtual void OnDestroy()override
	{
		if (_destroyCallback != nullptr)_destroyCallback();
	}
};
DC_END_NAMESPACE
