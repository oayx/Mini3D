
/*****************************************************************************
* Author： hannibal
* Date：2020/8/19
* Description：
*****************************************************************************/
#pragma once

#include "MeshUtils.h"
#include "runtime/component/GameObject.h"

DC_BEGIN_NAMESPACE
class Mesh;
class AnimationNode;
class AnimationClip;
class AnimationState;
class AnimationBlender;
/********************************************************************/
class ENGINE_DLL Animation final : public Component
{
	friend class Mesh;
	friend class GameObject;
	friend class AnimationState;
	friend class AnimationBlender;
	friend class SkinMeshRender;
	typedef Map<String, AnimationClip*> AnimationClips;
	DECLARE_OBJECT_CLONE;
	DECLARE_OBJECT_SERIALIZE(Animation);
	DISALLOW_COPY_ASSIGN(Animation);
	FRIEND_CONSTRUCT_DESTRUCT(Animation);
	BEGIN_DERIVED_REFECTION_TYPE(Animation, Component)
		CTORS(DEFAULT_CTOR(Animation))
		ADD_EDITOR_COMPONENT("Animation")
	END_REFECTION_TYPE;

	Animation();
	~Animation();

private:
	virtual void	Start()override;
	virtual void	Update()override;
	virtual void	OnDrawEditor()override;

public:
	void			Simulate(float dt);

	bool			Play();
	bool			Play(const String& name);
	bool			Play(const String& name, bool loop);
	bool			Play(const VecString& names);
	bool			PlayCross(const String& name, float fade_length, bool loop);//带过渡功能
	bool			PlayCross(const VecString& names, float fade_length);
	void			Stop();

public:
	const String&	GetName()const;
	uint			GetFrames()const;
	uint			GetFPS()const;
	float			GetDuration()const;

	bool			IsLoop()const;
	bool			IsPlaying()const;
	float			GetPosition()const;

	void			SetPlayOnAwake(bool b) { _playOnAwake = b; }
	void			SetAutoSimulate(bool b) { _autoSimulate = b; }
	void			SetUseUnscaledTime(bool b) { _useUnscaledTime = b; }

	void			SetSpeed(float speed) { AssertEx(speed >= 0, ""); _speed = speed; }
	float			GetSpeed()const { return _speed; }

	void			SetDefaultName(const String& name) { _defaultName = name; }

	AnimationClip*	GetClip(const String& name);

	void			OnComplete(Action callback) { _onComplete = callback; }

private:
	void			SetMesh(Mesh* mesh);
	bool			PlayImpl(const String& name, bool loop);
	void			OnPlayComplete(AnimationState* state);
	void			SwapState();

	const BoneTransforms& GetBoneTransform()const { return _boneTransformes; }

private:
	bool			_playOnAwake = true;		//唤醒时播放
	bool			_autoSimulate = true;		//是否自动模拟
	bool			_useUnscaledTime = true;	//是否受DeltaTime影响，默认true
	float			_speed = 1.0f;				//更新速度
	String			_defaultName = "";			//默认播放动画
	String			_meshFile = "";				//模型

	Action			_onComplete = nullptr;

	VecString		_playSet;					//待播放动作集合
	AnimationState*	_currentState = nullptr;
	AnimationState*	_oldState = nullptr;		//上个剪辑，用于动画过渡
	AnimationClips	_animationClips;
	AnimationBlender* _animationBlender = nullptr;

	BoneTransforms	_boneTransformes;
	BoneTransforms	_oldBoneTransformes;
};
DC_END_NAMESPACE
