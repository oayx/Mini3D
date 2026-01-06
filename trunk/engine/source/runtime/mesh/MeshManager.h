
/*****************************************************************************
* Author： hannibal
* Date：2020/12/13
* Description： mesh管理
*****************************************************************************/
#pragma once

#include "core/Object.h"

DC_BEGIN_NAMESPACE
class Mesh;
/********************************************************************/
class MeshManager final : public object
{
	friend class Mesh;
	friend class Application;
	typedef Map<String, Mesh*> Meshes;
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(MeshManager);
	BEGIN_REFECTION_TYPE(MeshManager)
	END_FINAL_REFECTION_TYPE;

private:
	~MeshManager() {}
	static void Destroy();

	static bool AddMesh(const String& name, Mesh* mesh);

public:
	static Mesh* GetMesh(const String& name);
	static void DestroyMesh(const String& name);
	static void DestroyMesh();

private:
	inline static Meshes _meshes;
};
DC_END_NAMESPACE
