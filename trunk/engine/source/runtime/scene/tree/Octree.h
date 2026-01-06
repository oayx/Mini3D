
/*****************************************************************************
* Author： hannibal
* Date：2009/12/8
* Description： 八叉树
*****************************************************************************/
#pragma once

#include "Scenetree.h"

DC_BEGIN_NAMESPACE
class Camera;
/********************************************************************/
// 八叉树
class Octree final : public Scenetree
{
	friend class SceneManager;
	typedef List<GameObject*> ListNodes;
	DEFAULT_CREATE(Octree);
	FRIEND_CONSTRUCT_DESTRUCT(Octree);
	DISALLOW_COPY_ASSIGN(Octree);
	BEGIN_DERIVED_REFECTION_TYPE(Octree, Scenetree)
	END_REFECTION_TYPE;

public:
	Octree(Octree *octree);
	Octree(const Aabb& aabb);
	~Octree();

	//由于每次把空间八等份，导致如果物体处于世界中心，会被添加到最顶级的Octree里面
	//另外由于FindVisibleObjects判断最顶级Octree用的是Partial，还是会对物体再做一次可视判断，所以不会导致看不到的添加的渲染列表
	virtual void AddObject(GameObject *node, int depth = 0)override;
	virtual void RemoveAllObjects()override;
	virtual void FindVisibleObjects(Camera *camera, bool foundVisible)override;
	virtual void FindObjects(List<GameObject*>& list, const Ray& ray)override;

private:
	// 添加节点
	void AddNode(GameObject *node);
	// 删除节点
	void RemoveNode(GameObject *node);

	int  NumOfNode()const { return _numNode; };

	// 完全包括
	bool Container(const Aabb &box)const;
	// 把立方体看成八等份，看box属于那一部分；方式是判断中心点处于哪个Tree
	void GetChildIndex(const Aabb &box, int *x, int *y, int *z)const;

	// 递归增/减
	void Ref();
	void Unref();

private:
	int			_numNode = 0;
	ListNodes	_nodes;				//这棵树当前节点关联的节点列表

	Aabb		_box;

	Octree*		_parentTree = nullptr;//父节点,根节点的话为NULL
	Octree*		_childrens[2][2][2] = {nullptr};	//八个子节点
};//Octree
DC_END_NAMESPACE
