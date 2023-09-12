
/*****************************************************************************
* Author： hannibal
* Date：2021/1/19
* Description：
*****************************************************************************/
#pragma once

#include "runtime/graphics/RenderDefine.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
//默认动画
#define DEFAULT_ANIMATION "animation"
typedef Vector<Matrix4> BoneTransforms;

//单个顶点最多受影响骨骼的数量
#define NUM_BONES_PER_VEREX 4

// 存储每个顶点所对应的骨骼数据
struct VertexBoneData
{
	uint IDs[NUM_BONES_PER_VEREX];		// 存储每个顶点所对应的骨骼ID, 设每个顶点最多被4个骨骼所影响
	float Weights[NUM_BONES_PER_VEREX]; // 存储每个顶点所对应的骨骼权重，分别对应IDs中的骨骼，最终weights之和为1

	VertexBoneData()
	{
		memset(IDs, 0, sizeof(IDs));
		memset(Weights, 0, sizeof(Weights));
	}

	void AddBoneData(uint id, float weight)
	{
		for (uint i = 0; i < ARRAY_SIZE(IDs); i++)
		{
			if (Weights[i] == 0.0f) {
				IDs[i] = id;
				Weights[i] = weight;
				return;
			}
		}
		AssertEx(false, "");
	}
};
static_assert(sizeof(VertexBoneData) == NUM_BONES_PER_VEREX * (sizeof(float) + sizeof(uint)), "invalid bytes");

inline VertexSize GetVertexSizeByBone()
{
	switch (NUM_BONES_PER_VEREX)
	{
	case 1: return VertexSize::Float1;
	case 2: return VertexSize::Float2;
	case 3: return VertexSize::Float3;
	case 4: return VertexSize::Float4;
	default:AssertEx(false, ""); return VertexSize::Float4;
	}
}

//最小模型单位
struct MeshEntity
{
	DECLARE_ALLOCATOR;
	DEFAULT_CREATE(MeshEntity);
	FRIEND_CONSTRUCT_DESTRUCT(MeshEntity);
public:
	Vector3v				Vertexs;
	Vector<Color>			Colors;
	Vector3v				Normals;
	Vector3v				Tangents;
	Vector2v				TexCoords1;
	Vector2v				TexCoords2;
	Vector<uint>			Indexes;	// 索引
	Vector<VertexBoneData>	Weights;	// 每个顶点的权重信息
};

//动画剪辑
struct ClipAnimationDesc 
{
	String		Name = "";
	uint		StartFrame = 0;
	uint		EndFrame = 0;
	bool		Loop = false;

public:
	ClipAnimationDesc() {}
	ClipAnimationDesc(const String& name, uint start, uint end, bool loop = false)
	{
		this->Name = name; this->StartFrame = start; this->EndFrame = end; this->Loop = loop;
	}
public: 
	template<class TransferFunction> 
	void Transfer(TransferFunction& transfer, void* name = nullptr)
	{
		TRANSFER_SIMPLE(Name);
		TRANSFER_SIMPLE(StartFrame);
		TRANSFER_SIMPLE(EndFrame);
		TRANSFER_SIMPLE(Loop);
	}
};
typedef Vector<ClipAnimationDesc> ClipAnimations;

//骨骼变换数据
struct BoneKeyFrame
{
	Vector3		Position;
	Vector3		Scale;
	Quaternion	Rotate;
};
typedef Vector<BoneKeyFrame> BoneKeyFrames;

DC_END_NAMESPACE
