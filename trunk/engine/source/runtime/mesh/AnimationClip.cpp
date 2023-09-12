#include "AnimationClip.h"
#include "AnimationNode.h"
#include "Mesh.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(AnimationClip, Object);
AnimationClip::AnimationClip(const String& name)
	: _name(name)
{
}
AnimationClip::~AnimationClip()
{
	_rootNode = nullptr;
	for (auto node : _animationNodes)
	{
		SAFE_DELETE(node.second);
	}
	_animationNodes.Clear();
}
void AnimationClip::Update(float time, BoneKeyFrames& bone_keyframes)
{
	CHECK_RETURN_PTR_VOID(_rootNode);
	_rootNode->Interpolated(time, bone_keyframes);
}
AnimationNode* AnimationClip::CreateNode(Mesh* mesh, const String& name)
{
	if (_animationNodes.Find(name) != _animationNodes.end())
	{
		Debuger::Error("Already have the same node:%s", name.c_str());
		return nullptr;
	}
	AnimationNode* node = AnimationNode::Create(name);
	AssertEx(_animationNodes.Add(name, node),"");

	Bone* bone = mesh->GetBone(name);
	node->SetBone(bone);

	return node;
}
void AnimationClip::SetFrames(uint total_frames, uint fps)
{
	_totalFrames = total_frames;
	if (fps > 0)_fps = fps;
	_duration = (float)_totalFrames / (float)_fps;
}
bool AnimationClip::ClipAnimation(Mesh* mesh, const ClipAnimations& clips)
{
	for (ClipAnimationDesc clip_info : clips)
	{
		CHECK_RETURN_PTR_FALSE(!clip_info.Name.IsEmpty());
		CHECK_RETURN_PTR_FALSE(clip_info.StartFrame <= clip_info.EndFrame);
		CHECK_RETURN_PTR_FALSE(clip_info.EndFrame <= this->GetFrames());

		//复制Animation
		AnimationClip* clip = mesh->CreateClip(clip_info.Name);
		clip->SetFrames(clip_info.EndFrame - clip_info.StartFrame, this->GetFPS());
		clip->SetLoop(clip_info.Loop);

		//根节点
		_rootNode->ClipAnimation(mesh, clip, nullptr, clip_info);
	}
	return true;
}
DC_END_NAMESPACE
