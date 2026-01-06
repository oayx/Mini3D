 
/*****************************************************************************
* Author： hannibal
* Date：2021/9/6
* Description：跟随角色
*****************************************************************************/
#pragma once

#include "runtime/component/GameObject.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL ThirdFollowCamera final : public Component
{
	friend class GameObject;
	FRIEND_CONSTRUCT_DESTRUCT(ThirdFollowCamera);
	DECLARE_OBJECT_SERIALIZE(ThirdFollowCamera);
	BEGIN_DERIVED_REFECTION_TYPE(ThirdFollowCamera, Component)
		CTORS(DEFAULT_CTOR(ThirdFollowCamera))
		//ADD_EDITOR_COMPONENT("Component/ThirdFollowCamera")
	END_REFECTION_TYPE;

	ThirdFollowCamera() = default;

protected:
	virtual void Update()override;
	virtual void OnDrawEditor()override;

private:
	Vector3 _cameraOffset = Vector3::zero;
	Vector3 _cameraLookatOffset = Vector3::zero;
};
/********************************************************************/
class ENGINE_DLL ThirdFreeCamera final : public Component
{
	friend class GameObject;
	FRIEND_CONSTRUCT_DESTRUCT(ThirdFreeCamera);
	DECLARE_OBJECT_SERIALIZE(ThirdFreeCamera);
	BEGIN_DERIVED_REFECTION_TYPE(ThirdFreeCamera, Component)
		CTORS(DEFAULT_CTOR(ThirdFreeCamera))
		ADD_EDITOR_COMPONENT("Component/ThirdFreeCamera")
	END_REFECTION_TYPE;

	ThirdFreeCamera() = default;

protected:
	virtual void Start()override;
	virtual void Update()override;
	virtual void OnDrawEditor()override;

private:
	float _moveSpeed = 1.0f;
	Vector3 _cameraOffset = Vector3::zero;
	Vector3 _cameraLookatOffset = Vector3::zero;
};
DC_END_NAMESPACE