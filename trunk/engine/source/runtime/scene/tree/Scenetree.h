
/*****************************************************************************
* Author： hannibal
* Date：2020/12/8
* Description： 场景树
*****************************************************************************/
#pragma once
 
#include "runtime/component/GameObject.h"
#include "runtime/scene/SceneUtils.h"

DC_BEGIN_NAMESPACE
class Camera;
/********************************************************************/
// 场景树
class Scenetree : public Object
{
	BEGIN_DERIVED_REFECTION_TYPE(Scenetree, Object)
	END_REFECTION_TYPE;

public:
	virtual void AddObject(GameObject *node, int depth = 0) = 0;
	virtual void RemoveAllObjects() = 0;
	virtual void FindVisibleObjects(Camera *camera, bool foundVisible) = 0;
	virtual void FindObjects(List<GameObject*>& list, const Ray& ray) = 0;
};
ENGINE_FUN_DLL void CreateSceneTree(SceneTreeType type);
void DestroySceneTree();
Scenetree* GetSceneTree();
DC_END_NAMESPACE
