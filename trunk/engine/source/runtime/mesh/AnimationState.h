
/*****************************************************************************
* Author： hannibal
* Date：2020/8/19
* Description：动画播放状态
*****************************************************************************/
#pragma once

#include "MeshUtils.h"
#include "KeyFrame.h"

DC_BEGIN_NAMESPACE
class Animation;
class AnimationClip;
class AnimationNode;
/********************************************************************/
class ENGINE_DLL AnimationState Final : public Object
{
	friend class Animation;
	friend class AnimationBlender;
	DEFAULT_CREATE(AnimationState);
	FRIEND_CONSTRUCT_DESTRUCT(AnimationState);
	DISALLOW_COPY_ASSIGN(AnimationState);
	BEGIN_DERIVED_REFECTION_TYPE(AnimationState, Object)
	END_DERIVED_REFECTION_TYPE;

	AnimationState(Animation* animation);
	~AnimationState();

private:
	void			Update(float dt);

	void			Play(AnimationClip* clip, bool loop);
	void			Stop();

	void			SetBoneKeyFrames(int size) { _boneKeyframes.Resize(size); }
	void			TransfromBone(const Matrix4& parent_mat, BoneTransforms& bone_transforms);
	void			TransfromBone(AnimationNode* node, const Matrix4& parent_mat, BoneTransforms& bone_transforms);

public:
	const String&	GetName()const;
	uint			GetFrames()const;
	uint			GetFPS()const;
	float			GetDuration()const;

	bool			IsLoop()const { return _isLoop; }
	bool			IsPlaying()const { return _isPlaying; }
	float			GetPosition()const { return _playPosition; }

private:
	bool			_isLoop = false;			//是否循环
	bool			_isPlaying = false;
	float			_playPosition = 0.0f;		//播放位置

	Animation*		_animation = nullptr;
	AnimationClip*	_currentClip = nullptr;
	BoneKeyFrames	_boneKeyframes;
};
DC_END_NAMESPACE
