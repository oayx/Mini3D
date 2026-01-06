#include "MeshManager.h"
#include "Mesh.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_REFECTION_TYPE(MeshManager);
void MeshManager::Destroy()
{
	DestroyMesh();
}
bool MeshManager::AddMesh(const String& name, Mesh* mesh)
{
	CHECK_RETURN_PTR_FALSE(mesh);
	if (name.IsEmpty())return false;
	if (mesh != nullptr && _meshes.Find(name) == _meshes.end())
	{
		mesh->Retain();
		AssertEx(_meshes.Add(name, mesh),"");
		return true;
	}
	Debuger::Error("Already have mesh:%s", name.c_str());
	return false;
}
Mesh* MeshManager::GetMesh(const String& name)
{
	if (name.IsEmpty())return nullptr;
	Meshes::const_iterator it = _meshes.Find(name);
	if (it != _meshes.end())
	{
		return it->second;
	}
	return nullptr;
}
void MeshManager::DestroyMesh(const String& name)
{
	Meshes::iterator it = _meshes.Find(name);
	if (it != _meshes.end())
	{
		SAFE_RELEASE(it->second);
		_meshes.Remove(it);
	}
}
void MeshManager::DestroyMesh()
{
	for (auto obj : _meshes)
	{
		SAFE_RELEASE(obj.second);
	}
	_meshes.Clear();
}
DC_END_NAMESPACE
