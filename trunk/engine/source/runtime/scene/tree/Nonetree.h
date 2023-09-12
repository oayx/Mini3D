
/*****************************************************************************
* Author： hannibal
* Date：2020/12/8
* Description：不用树管理
*****************************************************************************/
#pragma once
 
#include "Scenetree.h"

DC_BEGIN_NAMESPACE
class Camera;
/********************************************************************/
class Nonetree Final : public Scenetree
{
	friend class SceneManager;
	typedef List<GameObject*> ListNodes;
	DEFAULT_CREATE(Nonetree);
	FRIEND_CONSTRUCT_DESTRUCT(Nonetree);
	DISALLOW_COPY_ASSIGN(Nonetree);
	BEGIN_DERIVED_REFECTION_TYPE(Nonetree, Scenetree)
	END_DERIVED_REFECTION_TYPE;

public:
	Nonetree() = default;
	~Nonetree() { _nodes.Clear(); }

	virtual void AddObject(GameObject *node, int depth = 0)override;
	virtual void RemoveAllObjects()override;
	virtual void FindVisibleObjects(Camera *camera, bool found_visible)override;
	virtual void FindObjects(List<GameObject*>& list, const Ray& ray)override;

private:
	ListNodes _nodes;//这棵树当前节点关联的节点列表
};
DC_END_NAMESPACE
