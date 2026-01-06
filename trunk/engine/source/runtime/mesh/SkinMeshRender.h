
/*****************************************************************************
* Author： hannibal
* Date：2020/12/13
* Description： 蒙皮动画渲染 
*****************************************************************************/
#pragma once

#include "MeshRender.h"

DC_BEGIN_NAMESPACE
class Camera;
class Animation;
/********************************************************************/
class ENGINE_DLL SkinMeshRender final : public MeshRender
{
	friend class GameObject;
	FRIEND_CONSTRUCT_DESTRUCT(SkinMeshRender);
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(SkinMeshRender);
	BEGIN_DERIVED_REFECTION_TYPE(SkinMeshRender, MeshRender)
		CTORS(DEFAULT_CTOR(SkinMeshRender))
		ADD_EDITOR_COMPONENT("Mesh/SkinMeshRender")
	END_REFECTION_TYPE;

protected:
	~SkinMeshRender();

	virtual void OnPreRender(Camera* camera)override;
	virtual void OnAddComponent(Component* com)override;
	virtual void OnRemoveComponent(Component* com)override;

public:
	virtual void SetMesh(Mesh* mesh)override;

protected:
	Animation* _animation = nullptr;
};
DC_END_NAMESPACE
