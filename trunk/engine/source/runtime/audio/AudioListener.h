 
/*****************************************************************************************************/
// @author hannibal
// @date   2020/08/04
// @brief  声音-听者
/*****************************************************************************************************/
#pragma once

#include "runtime/component/GameObject.h"

DC_BEGIN_NAMESPACE
class AudioListenerPrivate;
/********************************************************************/
class ENGINE_DLL AudioListener final : public Component
{
	friend class GameObject;
	DECLARE_OBJECT_CLONE;
	DISALLOW_COPY_ASSIGN(AudioListener);
	FRIEND_CONSTRUCT_DESTRUCT(AudioListener);
	BEGIN_DERIVED_REFECTION_TYPE(AudioListener, Component)
		CTORS(DEFAULT_CTOR(AudioListener))
		ADD_EDITOR_COMPONENT("Audio/AudioListener")
	END_REFECTION_TYPE;

private:
	AudioListener();
	virtual ~AudioListener();

protected:
	virtual void OnEnable()override;
	virtual void OnDisable()override;
	virtual void OnTransformChanged()override;
	virtual void OnDestroy()override;

private:
	AudioListenerPrivate* _private = nullptr;
};
DC_END_NAMESPACE
