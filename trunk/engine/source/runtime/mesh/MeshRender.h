
/*****************************************************************************
* Author： hannibal
* Date：2020/12/13
* Description： mesh渲染 
*****************************************************************************/
#pragma once

#include "runtime/renderable/Renderer.h"

DC_BEGIN_NAMESPACE
class Mesh;
/********************************************************************/
class ENGINE_DLL MeshRender : public Renderer
{
	friend class GameObject;
	FRIEND_CONSTRUCT_DESTRUCT(MeshRender);
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(MeshRender);
	DECLARE_OBJECT_CLONE;
	DECLARE_OBJECT_SERIALIZE(MeshRender);
	BEGIN_DERIVED_REFECTION_TYPE(MeshRender, Renderer)
		CTORS(DEFAULT_CTOR(MeshRender))
		ADD_EDITOR_COMPONENT("Mesh/MeshRender")
	END_DERIVED_REFECTION_TYPE;

protected:
	~MeshRender();
	virtual bool CanRemove()override;
	virtual void OnDrawEditor()override;

public:
	virtual void SetMesh(Mesh* mesh);
	Mesh* GetMesh()const { return _mesh; }

protected:
	Mesh* _mesh = nullptr;
};
DC_END_NAMESPACE
