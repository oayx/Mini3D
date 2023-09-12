 
/*****************************************************************************
* Author： hannibal
* Date：2021/9/24
* Description：角色控制器
*****************************************************************************/
#pragma once

#include "Physics_def.h"

class btPairCachingGhostObject;
class btKinematicCharacterController;

DC_BEGIN_NAMESPACE
/********************************************************************/
class CharacterController Final : public Component
{
	friend class GameObject;
	DISALLOW_COPY_ASSIGN(CharacterController);
	DECLARE_OBJECT_CLONE;
	DECLARE_OBJECT_SERIALIZE(CharacterController);
	FRIEND_CONSTRUCT_DESTRUCT(CharacterController);
	BEGIN_DERIVED_REFECTION_TYPE(CharacterController, Component)
		CTORS(DEFAULT_CTOR(CharacterController))
		ADD_EDITOR_COMPONENT("Physics/CharacterController")
	END_DERIVED_REFECTION_TYPE;

	CharacterController();
	~CharacterController();

private:
	virtual void Start()override;
	virtual void OnEnable()override;
	virtual void OnDisable()override;
	virtual void Update()override;
	virtual void OnDrawEditor()override;

public:
	void Move(const Vector3& dir);
	void SetGravity(float gravity);
	void SetStepHeight(float step);
	bool OnGround()const;

private:
	float _radius = 0.5f;
	float _height = 1.0f;
	float _stepHeight = 0.25f;

	btCapsuleShape* _shape = nullptr;
	btPairCachingGhostObject* _ghostObject = nullptr;
	btKinematicCharacterController* _characterController = nullptr;
};
DC_END_NAMESPACE