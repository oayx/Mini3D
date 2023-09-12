
/*****************************************************************************
* Author： hannibal
* Date：2020/8/19
* Description：动画数据
*****************************************************************************/
#pragma once

#include "MeshUtils.h"

DC_BEGIN_NAMESPACE
class Mesh;
class AnimationNode;
/********************************************************************/
class ENGINE_DLL AnimationClip Final : public Object
{
	friend class Mesh;
	friend class Animation;
	friend class AnimationNode;
	friend class AnimationState;
	typedef Map<String, AnimationNode*> AnimationNodes;
	DEFAULT_CREATE(AnimationClip);
	FRIEND_CONSTRUCT_DESTRUCT(AnimationClip);
	DISALLOW_COPY_ASSIGN(AnimationClip);
	BEGIN_DERIVED_REFECTION_TYPE(AnimationClip, Object)
	END_DERIVED_REFECTION_TYPE;

	AnimationClip(const String& name);
	~AnimationClip();

private:
	void			Update(float time, BoneKeyFrames& bone_keyframes);

	AnimationNode*	CreateNode(Mesh* mesh, const String& name);
	AnimationNode*	GetRootNode()const { return _rootNode; }
	void			SetRootNode(AnimationNode* node) { _rootNode = node; }

	bool			ClipAnimation(Mesh* mesh, const ClipAnimations& clips);

public:
	const			String& GetName()const { return _name; }
	void			SetFrames(uint total_frames, uint fps);
	uint			GetFrames()const { return _totalFrames; }
	uint			GetFPS()const { return _fps; }
	float			GetDuration()const { return _duration; }
	void			SetLoop(bool b) { _isLoop = b; }
	bool			IsLoop()const { return _isLoop; }

private:
	String			_name = "";
	uint			_totalFrames = 0;		//总帧数
	uint			_fps = 30;				//每秒帧数
	float			_duration = 0;			//持续时间
	bool			_isLoop = false;
	AnimationNode*	_rootNode = nullptr;
	AnimationNodes	_animationNodes;
};
DC_END_NAMESPACE
