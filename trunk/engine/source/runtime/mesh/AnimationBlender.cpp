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
void AnimationBlender::Blender(AnimationState* srcState, AnimationState* dstState, float factor)
{
	const BoneKeyFrames& srcKeyframe = srcState->_boneKeyframes;
	BoneKeyFrames& dst_keyframe = dstState->_boneKeyframes;
	for (int i = 0; i < srcKeyframe.Size() && i < dst_keyframe.Size(); ++i)
	{
		dst_keyframe[i].Position = Vector3::Lerp(srcKeyframe[i].Position, dst_keyframe[i].Position, factor);
		dst_keyframe[i].Scale = Vector3::Lerp(srcKeyframe[i].Scale, dst_keyframe[i].Scale, factor);
		dst_keyframe[i].Rotate = Quaternion::Lerp(srcKeyframe[i].Rotate, dst_keyframe[i].Rotate, factor);
	}
}
DC_END_NAMESPACE
