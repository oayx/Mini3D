
/*****************************************************************************
* Author： hannibal
* Date：2020/12/13
* Description： 多边形 
*****************************************************************************/
#pragma once

#include "runtime/resources/Resources.h"
#include "runtime/graphics/RenderDefine.h"
#include "MeshUtils.h"

struct aiMesh;
struct aiNode;
struct aiScene;
struct aiAnimation;
DC_BEGIN_NAMESPACE
class Bone;
class AnimationNode;
class AnimationClip;
/********************************************************************/
//mesh资源
class ENGINE_DLL Mesh Final : public Resource
{
	friend class MeshRender;
	friend class SkinMeshRender;
	friend class MeshManager;
	friend class AnimationClip;
	friend class Animation;
	typedef Vector<MeshEntity*> Entities;
	typedef Vector<Bone*> Bones;
	typedef Map<String, uint> BoneMap;
	typedef Map<String, AnimationClip*> AnimationClips;
	FRIEND_CONSTRUCT_DESTRUCT(Mesh);
	BEGIN_DERIVED_REFECTION_TYPE(Mesh, Resource)
	END_DERIVED_REFECTION_TYPE;
	Mesh();
	~Mesh();

public:
	static Mesh*	Create(const String& file);
	virtual bool	LoadFromFile(const String& file)override;

	bool			LoadAnimation(const String &file, const String& name);//加载独立动画文件
	bool			ClipAnimation(const ClipAnimations& clips);//动画剪辑
	void			ClearAnimations();

	Vector<Triangle>& GetTriangles();

private:
	void 			LoadNode(const aiScene* scene, const aiNode* node, bool import_animation);
	void 			LoadMesh(const aiScene* scene, const aiNode* node, const aiMesh* mesh, int mesh_index, bool import_animation);
	void 			LoadAnimationClip(const aiScene* scene, aiAnimation* anim, const String& name);
	void			LoadAnimationNode(const aiScene* scene, const aiNode* node, const aiAnimation* anim, AnimationClip* clip, AnimationNode* parent_node, uint max_frames);
	
	void			LoadMetaClip(const String& file);

	void 			RenameNode(const aiNode* node);	//重命名同名节点
	String			GetNodeName(const aiNode* node);//获得节点真实名称(包括重命名过)

	AnimationClip*	CreateClip(const String& name);

	Bone*			CreateBone(const String& name);	//骨骼数据
	Bone*			GetBone(const String& name)const;
	Bone*			GetBone(uint id)const;
	uint			GetNumBones()const { return (uint)_bones.Size(); }

private:
	Entities		_entities;			//模型数据
	Vector<Triangle>_triangles;		//三角形

	Bones			_bones;			//骨骼数据
	BoneMap			_bonesByName;

	AnimationClips	_animationClips;	//动画数据

	Map<const aiNode*, String> _node2Names;//保存重命名过的节点
	Map<String, const aiNode*> _name2Nodes;//保存重命名过的节点
};
DC_END_NAMESPACE