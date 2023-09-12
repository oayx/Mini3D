
/*****************************************************************************
* Author： hannibal
* Date：2020/8/19
* Description：动画过渡
*****************************************************************************/
#pragma once

#include "MeshUtils.h"

DC_BEGIN_NAMESPACE
class Animation;
class AnimationState;
/********************************************************************/
class ENGINE_DLL AnimationBlender Final : public Object
{
	friend class Animation;
	DEFAULT_CREATE(AnimationBlender);
	FRIEND_CONSTRUCT_DESTRUCT(AnimationBlender);
	DISALLOW_COPY_ASSIGN(AnimationBlender);
	BEGIN_DERIVED_REFECTION_TYPE(AnimationBlender, Object)
	END_DERIVED_REFECTION_TYPE;

	AnimationBlender(Animation* animation);
	~AnimationBlender();

private:
	void		SetLength(float length) { _fadeLength = length; }
	float		GetLength()const { return _fadeLength; }
	void		Reset();
	void		Stop();

	void		Update(float dt);
	void		Blender(AnimationState* src_state, AnimationState* dst_state, float factor);

private:
	Animation*	_animation = nullptr;
	float		_fadeLength = 0.0f;		//动画过渡时长
	float		_fadePosition = 0.0f;		//过渡位置
};
DC_END_NAMESPACE
