 
/*****************************************************************************
* Author： hannibal
* Date：2021/9/16
* Description：角色行为
*****************************************************************************/
#pragma once

#include "runtime/component/GameObject.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL CharacterMove final : public Component
{
	friend class GameObject;
	FRIEND_CONSTRUCT_DESTRUCT(CharacterMove);
	DECLARE_OBJECT_SERIALIZE(CharacterMove);
	BEGIN_DERIVED_REFECTION_TYPE(CharacterMove, Component)
		CTORS(DEFAULT_CTOR(CharacterMove))
		ADD_EDITOR_COMPONENT("Component/CharacterMove")
	END_REFECTION_TYPE;

	CharacterMove() = default;

protected:
	virtual void Update()override;
	virtual void OnDrawEditor()override;

private:
	float _moveSpeed = 1.0f;
};
DC_END_NAMESPACE