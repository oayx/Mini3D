
/*****************************************************************************
* Author： hannibal
* Date：2020/8/19
* Description：单个骨骼节点动画数据
*****************************************************************************/
#pragma once

#include "MeshUtils.h"

DC_BEGIN_NAMESPACE
class Mesh;
class Bone;
class KeyFrame;
class AnimationClip;
/********************************************************************/
class ENGINE_DLL AnimationNode Final : public Object
{
	friend class Mesh;
	friend class Animation;
	friend class AnimationClip;
	friend class AnimationState;
	typedef Vector<KeyFrame*> KeyFrames;
	typedef Vector<AnimationNode*> AnimationNodes;
	DEFAULT_CREATE(AnimationNode);
	FRIEND_CONSTRUCT_DESTRUCT(AnimationNode);
	DISALLOW_COPY_ASSIGN(AnimationNode);
	BEGIN_DERIVED_REFECTION_TYPE(AnimationNode, Object)
	END_DERIVED_REFECTION_TYPE;

	AnimationNode(const String& name);
	~AnimationNode();

private:
	const String&	GetName()const { return _name; }

	AnimationNode*	GetParent()const { return _parent; }
	void			AddChildren(AnimationNode* node);
	int				GetChildrenCount()const { return _childrens.Size(); }
	AnimationNode*	GetChildren(int index)const { return _childrens[index]; }

	void			Interpolated(float time, BoneKeyFrames& bone_keyframes);
	void			InterpolatedTransfrom(float time, BoneKeyFrames& bone_keyframes);

	void			SetTransfrom(const Matrix4& t) { _transfrom = t; }
	const Matrix4&	GetTransfrom()const { return _transfrom; }
	void			SetBone(Bone* bone) { _bone = bone; }
	Bone*			GetBone()const { return _bone; }

	void			ClipAnimation(Mesh* mesh, AnimationClip* clip, AnimationNode* parent_node, const ClipAnimationDesc& clip_info);

private:
	KeyFrame*		CreateKeyFrame(uint time);
	uint			FindKeyFrame(float time)const;

private:
	String			_name = "";
	AnimationNode*	_parent = nullptr;
	AnimationNodes	_childrens;
	
	Bone*			_bone = nullptr;

	Matrix4			_transfrom;
	KeyFrames		_keyFrames;
};
DC_END_NAMESPACE
