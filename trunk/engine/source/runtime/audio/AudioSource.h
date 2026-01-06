 
/*****************************************************************************************************/
// @author hannibal
// @date   2020/08/04
// @brief  声音-管理器
/*****************************************************************************************************/
#pragma once

#include "runtime/component/GameObject.h"

DC_BEGIN_NAMESPACE
class AudioClip;
class AudioSourcePrivate;
/********************************************************************/
class ENGINE_DLL AudioSource final : public Component
{
	friend class GameObject;
	DECLARE_OBJECT_CLONE;
	DECLARE_OBJECT_SERIALIZE(AudioSource);
	DISALLOW_COPY_ASSIGN(AudioSource);
	FRIEND_CONSTRUCT_DESTRUCT(AudioSource);
	BEGIN_DERIVED_REFECTION_TYPE(AudioSource, Component)
		CTORS(DEFAULT_CTOR(AudioSource))
		ADD_EDITOR_COMPONENT("Audio/AudioSource")
	END_REFECTION_TYPE;

private:
	AudioSource();
	virtual ~AudioSource();

public:
	enum class State
	{
		Unknown,

		Initial,
		Playing,
		Paused,
		Stopped,
	};

	void SetClip(AudioClip* clip);
	//循环
	void SetLoop(bool loop);
	//音量
	void SetVolume(float volume);
	//速度
	void SetPitch(float pitch);
	void Play();
	void Pause();
	void Stop();
	State GetState()const;
	const AudioClip* GetClip() const { return _clip; }

protected:
	virtual void Start()override;
	virtual void OnEnable()override;
	virtual void OnDisable()override;
	virtual void OnTransformChanged()override;
	virtual void OnDrawEditor()override;

private:
	AudioSourcePrivate* _private = nullptr;
	AudioClip* _clip = nullptr;

	bool	_playOnAwake = true;
	bool	_isLoop = false;
	float	_volume = 1.0f;
	float	_pitch = 1.0f;		//速度
};
DC_END_NAMESPACE
