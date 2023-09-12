 
/*****************************************************************************
* Author： hannibal
* Date：2021/10/6
* Description：场景相机
*****************************************************************************/
#pragma once

#include "runtime/component/GameObject.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL SceneFreeCamera Final : public Component
{
	friend class GameObject;
	FRIEND_CONSTRUCT_DESTRUCT(SceneFreeCamera);
	DECLARE_OBJECT_SERIALIZE(SceneFreeCamera);
	BEGIN_DERIVED_REFECTION_TYPE(SceneFreeCamera, Component)
		CTORS(DEFAULT_CTOR(SceneFreeCamera))
		ADD_EDITOR_COMPONENT("Component/SceneFreeCamera")
	END_DERIVED_REFECTION_TYPE;

	SceneFreeCamera() = default;

protected:
	virtual void Update()override;
	virtual void OnDrawEditor()override;

private:
	float _moveSpeed = 1.0f;
};
DC_END_NAMESPACE