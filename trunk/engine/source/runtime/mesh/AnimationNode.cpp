#include "AnimationNode.h"
#include "Animation.h"
#include "AnimationClip.h"
#include "KeyFrame.h"
#include "Bone.h"
#include "Mesh.h"
#include "assimp/quaternion.inl"
#include <assimp/vector3.inl>
#include <assimp/matrix4x4.inl>

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(AnimationNode, Object);
extern Matrix4 aiToMatrix4(const aiMatrix4x4& mat);
AnimationNode::AnimationNode(const String& name)
	: _name(name)
{
}
AnimationNode::~AnimationNode()
{
	_bone = nullptr;
	_parent = nullptr;
	_childrens.Clear();

	for (auto key_frame : _keyFrames)
	{
		SAFE_DELETE(key_frame);
	}
	_keyFrames.Clear();
}
void AnimationNode::Interpolated(float time, BoneKeyFrames& bone_keyframes)
{
	if (_keyFrames.Size() > 0)
	{
		this->InterpolatedTransfrom(time, bone_keyframes);
	}
	for (const auto& child_node : _childrens)
	{
		child_node->Interpolated(time, bone_keyframes);
	}
}
void AnimationNode::InterpolatedTransfrom(float time, BoneKeyFrames& bone_keyframes)
{
	AssertEx(_keyFrames.Size() > 0, "");
	Vector3 position; Vector3 scale; Quaternion rotate;
	if (_keyFrames.Size() == 1 || time == MAX_float)
	{//最后一帧(只有一帧或非循环动画才会进入)
		uint frame = (uint)_keyFrames.Size() - 1;
		position = _keyFrames[frame]->Position;
		scale = _keyFrames[frame]->Scale;
		rotate = _keyFrames[frame]->Rotate;
	}
	else
	{
		uint frame = FindKeyFrame(time);
		uint next_frame = (frame + 1);
		float factor = 0.0f;
		if (next_frame >= _keyFrames.Size())
		{//跟第一帧做过渡
			next_frame = 0;
			float delta_time = 1.0f;
			factor = (time - (float)_keyFrames[frame]->Time) / delta_time;
		}
		else
		{
			float delta_time = (float)(_keyFrames[next_frame]->Time - _keyFrames[frame]->Time);
			factor = (time - (float)_keyFrames[frame]->Time) / delta_time;
		}
		AssertEx(factor >= 0.0f && factor <= 1.0f, "");
		//位置
		{
			const Vector3& start = _keyFrames[frame]->Position;
			const Vector3& end = _keyFrames[next_frame]->Position;
			position = start + factor * (end - start);
		}

		//缩放 
		{
			const Vector3& start = _keyFrames[frame]->Scale;
			const Vector3& end = _keyFrames[next_frame]->Scale;
			scale = start + factor * (end - start);
		}

		//旋转
		{
			const Quaternion& start = _keyFrames[frame]->Rotate;
			const Quaternion& end = _keyFrames[next_frame]->Rotate;
			rotate = Quaternion::Lerp(start, end, factor);
		}
	}
	if (_bone)
	{
		BoneKeyFrame keyframe = { position, scale, rotate };
		bone_keyframes[_bone->Id] = keyframe;
	}
}
void AnimationNode::AddChildren(AnimationNode* node)
{
	CHECK_RETURN_PTR_VOID(node);
	_childrens.Add(node);
	node->_parent = this;
}
KeyFrame* AnimationNode::CreateKeyFrame(uint time)
{
	KeyFrame* key_frame = KeyFrame::Create(time);
	_keyFrames.Add(key_frame);
	return key_frame;
}
uint AnimationNode::FindKeyFrame(float time)const
{
	for (uint i = 0; i < _keyFrames.Size() - 1; i++)
	{
		if (time < _keyFrames[i + 1]->Time)
		{
			return i;
		}
	}
	return (uint)_keyFrames.Size() - 1;//最后一帧
}
void AnimationNode::ClipAnimation(Mesh* mesh, AnimationClip* clip, AnimationNode* parent_node, const ClipAnimationDesc& clip_info)
{
	//复制当前节点
	AnimationNode* animation_node = clip->CreateNode(mesh, this->_name);
	animation_node->SetBone(this->_bone);
	animation_node->SetTransfrom(this->_transfrom);
	if (parent_node == nullptr)
	{
		clip->SetRootNode(animation_node);
	}
	else
	{
		parent_node->AddChildren(animation_node);
	}
	//复制动画数据
	for (uint i = 0, j = clip_info.StartFrame; i < clip_info.EndFrame - clip_info.StartFrame && j < (uint)_keyFrames.Size(); ++i, ++j)
	{
		KeyFrame* frame = animation_node->CreateKeyFrame(i);
		frame->Copy(this->_keyFrames[j]);
	}
	//子节点
	for (AnimationNode* child_node : this->_childrens)
	{
		child_node->ClipAnimation(mesh, clip, animation_node, clip_info);
	}
}
DC_END_NAMESPACE
