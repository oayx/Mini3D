#include "Mesh.h"
#include "MeshManager.h"
#include "Bone.h"
#include "KeyFrame.h"
#include "Animation.h"
#include "AnimationClip.h"
#include "AnimationNode.h"
#include "runtime/graphics/Material.h"
#include "runtime/graphics/null/HardwareBuffer.h"
#include "runtime/resources/AssetsManager.h"
#include "runtime/scene/RenderQueue.h"
#include <assimp/matrix4x4.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(Mesh, Resource);
//加载
static const Assimp::Importer* LoadScene(const String &file)
{
	String path = Resource::GetFullDataPath(file);
	Debuger::Log("Load mesh:%s", file.c_str());
	if (File::Exist(path))
	{
		uint flags = 
			aiProcess_CalcTangentSpace |
			aiProcess_ConvertToLeftHanded |		// 坐标系
			aiProcess_FindInvalidData |
			aiProcess_FindDegenerates |
			aiProcess_GenUVCoords |
			aiProcess_GenSmoothNormals |		// 计算法线和切线
			aiProcess_Triangulate |				// 只使用三角形
			aiProcess_LimitBoneWeights |		// 骨骼权重限制
			aiProcess_PopulateArmatureData |	// 计算FinalTransform，填充Bone mNode字段
			aiProcess_FindInstances |			// 以下是优化相关的
			aiProcess_OptimizeMeshes |
			aiProcess_JoinIdenticalVertices |
			//aiProcess_PreTransformVertices |	// 会导致异常
			aiProcess_RemoveRedundantMaterials |
			aiProcess_SortByPType;

		bool is_import_animation = false;
		MeshMeta* meta = dynamic_cast<MeshMeta*>(AssetsManager::GetMetaByFile(file));
		if (meta)
		{
			if (!meta->IsGenNormal())flags ^= aiProcess_GenSmoothNormals;
			if (!meta->IsGenTangent())flags ^= aiProcess_CalcTangentSpace;
			if (!meta->IsOptimizeMesh())flags ^= aiProcess_OptimizeMeshes;
			is_import_animation = meta->IsImportAnimation();
		}

		Assimp::Importer* importer = Memory::New<Assimp::Importer>();
		importer->SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_LINE | aiPrimitiveType_POINT);
		importer->SetPropertyFloat(AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE, 175.0f);
		importer->SetPropertyFloat(AI_CONFIG_PP_CT_MAX_SMOOTHING_ANGLE, 45.0f);
		importer->SetPropertyBool(AI_CONFIG_IMPORT_FBX_READ_CAMERAS, false);
		importer->SetPropertyBool(AI_CONFIG_IMPORT_FBX_READ_LIGHTS, false);
		importer->SetPropertyBool(AI_CONFIG_IMPORT_FBX_READ_TEXTURES, false);
		importer->SetPropertyBool(AI_CONFIG_IMPORT_FBX_READ_ANIMATIONS, is_import_animation);
		importer->SetPropertyBool(AI_CONFIG_IMPORT_FBX_OPTIMIZE_EMPTY_ANIMATION_CURVES, true);
		importer->SetPropertyBool(AI_CONFIG_IMPORT_COLLADA_IGNORE_UP_DIRECTION, true);
		importer->SetPropertyBool(AI_CONFIG_PP_PTV_KEEP_HIERARCHY, true);
		const aiScene* scene = nullptr;
		_MY_TRY
		{
			scene = importer->ReadFile(path.c_str(), flags);
		}_MY_CATCH;
		if (scene == nullptr)
		{
			Debuger::Error("mesh file read failed: %s, error:%s", file.c_str(), importer->GetErrorString());
			SAFE_DELETE(importer);
		}

		return importer;
	}
	else
	{
		Debuger::Error("mesh file not exist: %s", file.c_str());
		return nullptr;
	}
}
static bool IsValidNode(const aiNode* node)
{
	std::string nodeName = node->mName.data;
	if (nodeName.find_first_of('$') == std::string::npos || nodeName.find_first_of('$') == nodeName.find_last_of('$'))
		return true;
	return false;
}
//获得节点到根的完整变换
static void GetNodeFullTransform(const aiNode* node, aiMatrix4x4& mat)
{
	mat = node->mTransformation;
	while (node->mParent != nullptr)
	{
		node = node->mParent;
		if (IsValidNode(node))
		{
			mat = node->mTransformation * mat;
		}
	}
}
//查找动画
static aiNodeAnim* FindNodeAnim(const aiAnimation* anim, const String& name)
{
	for (uint i = 0; i < anim->mNumChannels; i++)
	{
		aiNodeAnim* node_anim = anim->mChannels[i];
		if (String(node_anim->mNodeName.data) == name)
		{
			return node_anim;
		}
	}
	return nullptr;
}
//是否有动画
static bool HasBones(const aiScene* scene)
{
	for (uint meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex)
	{
		aiMesh* mesh = scene->mMeshes[meshIndex];
		if (mesh->mNumBones > 0)
		{
			return true;
		}
	}
	return false;
}
Matrix4 aiToMatrix4(const aiMatrix4x4& mat)
{
	return Matrix4(
		mat.a1, mat.b1, mat.c1, mat.d1,
		mat.a2, mat.b2, mat.c2, mat.d2,
		mat.a3, mat.b3, mat.c3, mat.d3,
		mat.a4, mat.b4, mat.c4, mat.d4
	);
}
Mesh* Mesh::Create(const String& file)
{
	if (file.IsEmpty())return nullptr;
	Mesh* mesh = MeshManager::GetMesh(file);
	if (mesh != nullptr)
		return mesh;

	mesh = Memory::New<Mesh>();
	mesh->AutoRelease();
	if (mesh->Load(file))
	{
		MeshManager::AddMesh(file, mesh);
		return mesh;
	}
	else
	{
		return nullptr;
	}
}
Mesh::Mesh()
{
}
Mesh::~Mesh() 
{
	for (auto entity : _entities)
	{
		SAFE_DELETE(entity);
	}
	_entities.Clear();

	for (auto bone : _bones)
	{
		SAFE_DELETE(bone);
	}
	_bones.Clear();
	_bonesByName.Clear();
	_node2Names.Clear();
	_name2Nodes.Clear();

	ClearAnimations();
}
bool Mesh::LoadFromFile(const String &file)
{
	base::LoadFromFile(file);

	auto importer = LoadScene(file);
	if (importer == nullptr)return false;

	auto scene = importer->GetScene();
	//重命名
	this->RenameNode(scene->mRootNode);

	bool importAnimation = false;
	MeshMeta* meta = dynamic_cast<MeshMeta*>(AssetsManager::GetMetaByFile(file));
	if (meta && meta->IsImportAnimation())
	{
		importAnimation = true;
	}

	// 递归处理结点
	this->LoadNode(scene, scene->mRootNode, importAnimation);

	if (importAnimation)
	{
		//动画
		for (uint i = 0; i < scene->mNumAnimations; ++i)
		{
			aiAnimation* anim = scene->mAnimations[i];
			this->LoadAnimationClip(scene, anim, DEFAULT_ANIMATION);
		}
		//剪辑
		this->LoadMetaClip(file);
	}

	_node2Names.Clear();
	_name2Nodes.Clear();
	SAFE_DELETE(importer);
	return true;
}
bool Mesh::LoadAnimation(const String &file, const String& name)
{
	auto importer = LoadScene(file);
	if (importer == nullptr)return false;

	const aiScene* scene = importer->GetScene();
	if (scene->mNumAnimations != 1)
	{
		SAFE_DELETE(importer);
		return false;
	}
	LoadAnimationClip(scene, scene->mAnimations[0], name);

	SAFE_DELETE(importer);
	return true;
}
bool Mesh::ClipAnimation(const ClipAnimations& clips)
{
	if (clips.IsEmpty())return false;
	auto it = _animationClips.Find(DEFAULT_ANIMATION);
	if (it == _animationClips.end())return false;

	AnimationClip* main_clip = it->second;
	if (main_clip == nullptr)return false;
	main_clip->ClipAnimation(this, clips);

	//移除默认的
	SAFE_DELETE(it->second);
	_animationClips.Remove(it);

	return false;
}
void Mesh::ClearAnimations()
{
	for (auto clip : _animationClips)
	{
		SAFE_DELETE(clip.second);
	}
	_animationClips.Clear();
}
Vector<Triangle>& Mesh::GetTriangles()
{
	if (!_triangles.IsEmpty())
		return _triangles;

	for(MeshEntity* entity : _entities)
	{
		for (int face = 0; face < entity->Indexes.Size(); face += 3)
		{
			Triangle triangle(entity->Vertexs[entity->Indexes[face]], entity->Vertexs[entity->Indexes[face + 1]], entity->Vertexs[entity->Indexes[face + 2]]);
			_triangles.Add(triangle);
		}
	}
	return _triangles;
}
void Mesh::LoadNode(const aiScene* scene, const aiNode* node, bool importAnimation)
{
	// 先处理自身结点
	for (size_t i = 0; i < node->mNumMeshes; ++i)
	{
		// 注意node中的mesh是对scene中mesh的索引
		const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		if (mesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE || mesh->mNumVertices == 0 || mesh->mNumFaces == 0 || mesh->mFaces[0].mNumIndices != 3)
			continue;

		//创建entity
		MeshEntity* entity = MeshEntity::Create();
		_entities.Add(entity);
		//读取模型数据
		this->LoadMesh(scene, node, mesh, node->mMeshes[i], importAnimation);
	}

	// 再处理孩子结点
	for (size_t i = 0; i < node->mNumChildren; ++i)
	{
		this->LoadNode(scene, node->mChildren[i], importAnimation);
	}
}
void Mesh::LoadMesh(const aiScene* scene, const aiNode* node, const aiMesh* mesh, int meshIndex, bool importAnimation)
{
	//获得变化矩阵
	Matrix4 mat_node = Matrix4::identity;
	{
		aiMatrix4x4 aimat = node->mTransformation;
		while (node->mParent != nullptr)
		{
			node = node->mParent;
			aimat = node->mTransformation * aimat;
		}
		mat_node = aiToMatrix4(aimat);
	}

	//Vertex
	MeshEntity* entity = _entities[meshIndex];
	entity->Vertexs.Reserve(mesh->mNumVertices);
	if (mesh->mColors[0])entity->Colors.Reserve(mesh->mNumVertices);
	if (mesh->mTextureCoords[0])entity->TexCoords1.Reserve(mesh->mNumVertices);
	if (mesh->mTextureCoords[1])entity->TexCoords2.Reserve(mesh->mNumVertices);
	for (uint j = 0; j < mesh->mNumVertices; ++j)
	{
		auto v = mesh->mVertices[j];
		Vector3 vertex(v.x, v.y, v.z);
		entity->Vertexs.Add(importAnimation ? vertex : mat_node.TransformPoint(vertex));

		if (mesh->mColors[0])
		{
			auto c = mesh->mColors[0][j];
			entity->Colors.Add(Color(c.r, c.g, c.b, c.a));
		}
		if (mesh->mTextureCoords[0])
		{
			v = mesh->mTextureCoords[0][j];
			entity->TexCoords1.Add(Vector2(v.x, v.y));
		}
		if (mesh->mTextureCoords[1])
		{
			v = mesh->mTextureCoords[1][j];
			entity->TexCoords2.Add(Vector2(v.x, v.y));
		}
	}
	if (mesh->mNormals)
	{
		entity->Normals.Resize(mesh->mNumVertices);
		Memory::Copy(&entity->Normals[0], mesh->mNormals, mesh->mNumVertices * sizeof(Vector3));
	}
	if (mesh->mTangents)
	{
		entity->Tangents.Resize(mesh->mNumVertices);
		Memory::Copy(&entity->Tangents[0], mesh->mTangents, mesh->mNumVertices * sizeof(Vector3));
	}

	//Index
	entity->Indexes.Reserve(mesh->mNumFaces * 3);
	for (uint j = 0; j < mesh->mNumFaces; j++)
	{
		const aiFace& face = mesh->mFaces[j];
		AssertEx(face.mNumIndices == 3, "");
		entity->Indexes.Add(face.mIndices[0]);
		entity->Indexes.Add(face.mIndices[1]);
		entity->Indexes.Add(face.mIndices[2]);
	}

	//Bones
	if (mesh->mNumBones > 0)
	{
		entity->Weights.Resize(mesh->mNumVertices);
		for (uint i = 0; i < mesh->mNumBones; ++i)
		{
			aiBone* bone = mesh->mBones[i];

			//获得变化矩阵
			aiMatrix4x4 aimat;
			GetNodeFullTransform(bone->mNode, aimat);

			//创建骨骼
			Bone* bone3d = this->CreateBone(GetNodeName(bone->mNode));
			if (!bone3d)continue;
			bone3d->BoneOffset = aiToMatrix4(bone->mOffsetMatrix);
			bone3d->FinalTransform = bone3d->BoneOffset * aiToMatrix4(aimat);

			// 处理每一个骨骼中的所有weights(单个weight影响单个顶点)
			for (uint j = 0; j < bone->mNumWeights; j++)
			{
				uint vertexId = bone->mWeights[j].mVertexId;
				entity->Weights[vertexId].AddBoneData(bone3d->Id, bone->mWeights[j].mWeight);
			}
		}
	}

	//TODO:normalize bone blend weights

	//TODO:load blend shapes
}
void Mesh::LoadAnimationClip(const aiScene* scene, aiAnimation* anim, const String& name)
{
	if (_animationClips.Find(name) != _animationClips.end())
	{
		Debuger::Error("Already have the same clip:%s", name.c_str());
		return;
	}

	AnimationClip* clip = this->CreateClip(name);
	uint maxFrames = Math::Ceil<uint>((float)anim->mDuration) + 1;
	clip->SetFrames(maxFrames, (int)anim->mTicksPerSecond);
	_animationClips.Add(name, clip);

	//root node
	AnimationNode* animationNode = clip->CreateNode(this, GetNodeName(scene->mRootNode));
	animationNode->SetTransfrom(aiToMatrix4(scene->mRootNode->mTransformation));
	clip->SetRootNode(animationNode);

	for (uint i = 0; i < scene->mRootNode->mNumChildren; i++)
	{
		LoadAnimationNode(scene, scene->mRootNode->mChildren[i], anim, clip, animationNode, maxFrames);
	}
}
void Mesh::LoadAnimationNode(const aiScene* scene, const aiNode* node,const aiAnimation* anim, AnimationClip* clip, AnimationNode* parentNode, uint maxFrames)
{
	if (IsValidNode(node))
	{
		String nodeName = GetNodeName(node);
		AnimationNode* animationNode = clip->CreateNode(this, nodeName);
		animationNode->SetTransfrom(aiToMatrix4(node->mTransformation));
		parentNode->AddChildren(animationNode);
		parentNode = animationNode;

		//帧数据
		aiNodeAnim* node_anim = FindNodeAnim(anim, nodeName);
		if (node_anim != nullptr)
		{
			auto it = _bonesByName.Find(nodeName);
			if (it == _bonesByName.end())
			{//创建骨骼	
				aiMatrix4x4 aimat;
				GetNodeFullTransform(node, aimat);
				Bone* bone3d = this->CreateBone(nodeName);
				bone3d->BoneOffset = Matrix4::identity;
				bone3d->FinalTransform = bone3d->BoneOffset * aiToMatrix4(aimat);
			}

			for (uint k = 0; k < maxFrames; ++k)
			{
				const aiVectorKey& posKey = node_anim->mNumPositionKeys > k ? node_anim->mPositionKeys[k] : node_anim->mPositionKeys[0];
				const aiQuatKey& rotateKey = node_anim->mNumRotationKeys > k ? node_anim->mRotationKeys[k] : node_anim->mRotationKeys[0];
				const aiVectorKey& scaleKey = node_anim->mNumScalingKeys > k ? node_anim->mScalingKeys[k] : node_anim->mScalingKeys[0];
				KeyFrame* keyFrame = animationNode->CreateKeyFrame(k);
				keyFrame->Position = Vector3(posKey.mValue.x, posKey.mValue.y, posKey.mValue.z);
				keyFrame->Rotate = Quaternion(rotateKey.mValue.x, rotateKey.mValue.y, rotateKey.mValue.z, rotateKey.mValue.w);
				keyFrame->Scale = Vector3(scaleKey.mValue.x, scaleKey.mValue.y, scaleKey.mValue.z);
			}
		}
	}

	for (uint i = 0; i < node->mNumChildren; i++)
	{
		LoadAnimationNode(scene, node->mChildren[i], anim, clip, parentNode, maxFrames);
	}
}
void Mesh::LoadMetaClip(const String& file)
{
	MeshMeta* meta = dynamic_cast<MeshMeta*>(AssetsManager::GetMetaByFile(file));
	if (!meta)
		return;
	if (meta->GetClips().Size() > 0)
	{
		ClipAnimation(meta->GetClips());
	}
}
void Mesh::RenameNode(const aiNode* node)
{
	String nodeName = node->mName.data;
	if (IsValidNode(node))
	{
		if (_name2Nodes.Find(nodeName) != _name2Nodes.end())
		{//重命名
			nodeName += " 1";
			while (_name2Nodes.Find(nodeName) != _name2Nodes.end())
			{//防止多个重用同一个名称
				nodeName += " 1";
			}
			_node2Names.Add(node, nodeName);
			Debuger::Debug("rename node:%s to:%s", node->mName.data, nodeName.c_str());
		}
		AssertEx(_name2Nodes.Add(nodeName, node), "");
	}

	for (uint i = 0; i < node->mNumChildren; i++)
	{
		aiNode* childNode = node->mChildren[i];
		RenameNode(childNode);
	}
}
String Mesh::GetNodeName(const aiNode* node)
{
	auto it = _node2Names.Find(node);
	if (it != _node2Names.end())
	{
		return it->second;
	}
	return node->mName.data;
}
AnimationClip* Mesh::CreateClip(const String& name)
{
	if (_animationClips.Find(name) != _animationClips.end())
	{
		Debuger::Error("Already have animation clip:%s", name.c_str());
		return nullptr;
	}
	AnimationClip* clip = AnimationClip::Create(name);
	AssertEx(_animationClips.Add(name, clip),"");
	return clip;
}
Bone* Mesh::CreateBone(const String& name)
{
	auto it = _bonesByName.Find(name);
	if (it != _bonesByName.end())
	{
		Debuger::Error("already have bone name:%s", name.c_str());
		return nullptr;
	}

	uint id = (uint)_bones.Size();
	Bone* bone = Bone::Create(name, id);
	_bones.Add(bone);
	_bonesByName.AddOrUpdate(name, id);
	return bone;
}
Bone* Mesh::GetBone(const String& name)const
{
	auto it = _bonesByName.Find(name);
	if (it == _bonesByName.end())
	{
		return nullptr;
	}
	return GetBone(it->second);
}
Bone* Mesh::GetBone(uint id)const
{
	if (id >= 0 && id < GetNumBones())
		return _bones[id];
	else
		return nullptr;
}
DC_END_NAMESPACE