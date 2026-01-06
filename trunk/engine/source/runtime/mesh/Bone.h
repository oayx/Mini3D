
/*****************************************************************************
* Author： hannibal
* Date：2020/8/19
* Description：
*****************************************************************************/
#pragma once

#include "core/Object.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL Bone final : public Object
{
	friend class Mesh;
	friend class Animation;
	DEFAULT_CREATE(Bone);
	FRIEND_CONSTRUCT_DESTRUCT(Bone);
	DISALLOW_COPY_ASSIGN(Bone);
	BEGIN_DERIVED_REFECTION_TYPE(Bone, Object)
	END_REFECTION_TYPE;

	Bone(const String& name, uint id) : Name(name), Id(id) {}
	~Bone() {}

public:
	String		Name = "";
	uint		Id = 0;
	Matrix4		BoneOffset;
	Matrix4		FinalTransform;
};
DC_END_NAMESPACE
