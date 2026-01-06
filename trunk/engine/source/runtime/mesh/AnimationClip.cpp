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
void AnimationClip::Update(float time, BoneKeyFrames& boneKeyframes)
{
	CHECK_RETURN_PTR_VOID(_rootNode);
	_rootNode->Interpolated(time, boneKeyframes);
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
void AnimationClip::SetFrames(uint totalFrames, uint fps)
{
	_totalFrames = totalFrames;
	if (fps > 0)_fps = fps;
	_duration = (float)_totalFrames / (float)_fps;
}
bool AnimationClip::ClipAnimation(Mesh* mesh, const ClipAnimations& clips)
{
	for (ClipAnimationDesc clipInfo : clips)
	{
		CHECK_RETURN_PTR_FALSE(!clipInfo.Name.IsEmpty());
		CHECK_RETURN_PTR_FALSE(clipInfo.StartFrame <= clipInfo.EndFrame);
		CHECK_RETURN_PTR_FALSE(clipInfo.EndFrame <= this->GetFrames());

		//复制Animation
		AnimationClip* clip = mesh->CreateClip(clipInfo.Name);
		clip->SetFrames(clipInfo.EndFrame - clipInfo.StartFrame, this->GetFPS());
		clip->SetLoop(clipInfo.Loop);

		//根节点
		_rootNode->ClipAnimation(mesh, clip, nullptr, clipInfo);
	}
	return true;
}
DC_END_NAMESPACE
