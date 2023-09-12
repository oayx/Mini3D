#include "AnimationBlender.h"
#include "Animation.h"
#include "AnimationState.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(AnimationBlender, Object);
AnimationBlender::AnimationBlender(Animation* animation)
	: _animation(animation)
{
}
AnimationBlender::~AnimationBlender()
{
	_animation = nullptr;
}
void AnimationBlender::Reset()
{ 
	_fadePosition = 0.0f; 
}
void AnimationBlender::Stop()
{
	_fadeLength = 0.0f;
	_fadePosition = 0.0f;
}
void AnimationBlender::Update(float dt)
{
	if (_fadeLength > 0.0f && _fadePosition < _fadeLength)
	{
		_fadePosition += dt;
		if (_fadePosition > _fadeLength)_fadePosition = _fadeLength;
		float factor = _fadePosition / _fadeLength;
		this->Blender(_animation->_oldState, _animation->_currentState, factor);
	}
}
void AnimationBlender::Blender(AnimationState* src_state, AnimationState* dst_state, float factor)
{
	const BoneKeyFrames& src_keyframe = src_state->_boneKeyframes;
	BoneKeyFrames& dst_keyframe = dst_state->_boneKeyframes;
	for (int i = 0; i < src_keyframe.Size() && i < dst_keyframe.Size(); ++i)
	{
		dst_keyframe[i].Position = Vector3::Lerp(src_keyframe[i].Position, dst_keyframe[i].Position, factor);
		dst_keyframe[i].Scale = Vector3::Lerp(src_keyframe[i].Scale, dst_keyframe[i].Scale, factor);
		dst_keyframe[i].Rotate = Quaternion::Lerp(src_keyframe[i].Rotate, dst_keyframe[i].Rotate, factor);
	}
}
DC_END_NAMESPACE
