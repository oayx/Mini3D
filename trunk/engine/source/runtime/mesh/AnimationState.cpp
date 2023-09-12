#include "AnimationState.h"
#include "AnimationClip.h"
#include "AnimationNode.h"
#include "Animation.h"
#include "Bone.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(AnimationState, Object);
AnimationState::AnimationState(Animation* animation)
	: _animation(animation)
{
}
AnimationState::~AnimationState()
{
	_animation = nullptr;
	_currentClip = nullptr;
}
void AnimationState::Update(float dt)
{
	if (_isPlaying && _currentClip != nullptr)
	{
		_playPosition += dt;

		//计算播放进度
		bool is_play_complete = false;
		float time = _playPosition * _currentClip->GetFPS();
		if (_currentClip->GetFrames() > 1)
		{
			if (_isLoop)
			{
				if (time > (float)_currentClip->GetFrames())
				{
					time = (float)::fmod(time, _currentClip->GetFrames());
					_playPosition = (float)::fmod(_playPosition, _currentClip->_duration);
					is_play_complete = true;
				}
			}
			else
			{
				if (time > (float)_currentClip->GetFrames() - 1.0f)
				{//停留在最后位置
					time = MAX_float;
					is_play_complete = true;
				}
			}
		}
		else
		{
			time = 0.0f;
		}

		_currentClip->Update(time, _boneKeyframes);

		if (is_play_complete)
		{
			_animation->OnPlayComplete(this);
		}
	}
}
void AnimationState::Play(AnimationClip* clip, bool loop)
{
	_isPlaying = true;
	_playPosition = 0;
	_currentClip = clip;
	_isLoop = loop;
}
void AnimationState::Stop()
{
	_isPlaying = false;
	_currentClip = nullptr;
	_playPosition = 0;
}
void AnimationState::TransfromBone(const Matrix4& parent_mat, BoneTransforms& bone_transforms)
{
	if (_currentClip != nullptr)
	{
		this->TransfromBone(_currentClip->_rootNode, parent_mat, bone_transforms);
	}
}
void AnimationState::TransfromBone(AnimationNode* node, const Matrix4& parent_mat, BoneTransforms& bone_transforms)
{
	Matrix4 node_mat = node->GetTransfrom();
	if (node->GetBone() != nullptr)
	{
		const BoneKeyFrame& keyframe = _boneKeyframes[node->GetBone()->Id];
		node_mat = Matrix4(keyframe.Position, keyframe.Rotate, keyframe.Scale);
	}
	Matrix4 full_mat = node_mat * parent_mat;//这个要反着变换

	if (node->GetBone() != nullptr)
	{
		Matrix4 mat = node->GetBone()->BoneOffset * full_mat;//这个要反着变换
		bone_transforms[node->GetBone()->Id] = mat;
	}

	for (int i = 0; i <node->GetChildrenCount(); ++i)
	{
		this->TransfromBone(node->GetChildren(i), full_mat, bone_transforms);
	}
}
const String& AnimationState::GetName()const
{
	if (_currentClip == nullptr)
		return String::Empty;
	return _currentClip->_name;
}
uint AnimationState::GetFrames()const
{
	if (_currentClip == nullptr)
		return 0;
	return _currentClip->_totalFrames;
}
uint AnimationState::GetFPS()const
{
	if (_currentClip == nullptr)
		return 0;
	return _currentClip->_fps;
}
float AnimationState::GetDuration()const
{
	if (_currentClip == nullptr)
		return 0;
	return _currentClip->_duration;
}
DC_END_NAMESPACE
