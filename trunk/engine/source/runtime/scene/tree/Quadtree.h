
/*****************************************************************************
* Author： hannibal
* Date：2020/12/8
* Description： 四叉树
*****************************************************************************/
#pragma once
 
#include "Scenetree.h"

DC_BEGIN_NAMESPACE
class Camera;
/********************************************************************/
// 四叉树
class Quadtree Final : public Scenetree
{
	friend class SceneManager;
	typedef List<GameObject*> ListNodes;
	DEFAULT_CREATE(Quadtree);
	FRIEND_CONSTRUCT_DESTRUCT(Quadtree);
	DISALLOW_COPY_ASSIGN(Quadtree);
	BEGIN_DERIVED_REFECTION_TYPE(Quadtree, Scenetree)
	END_DERIVED_REFECTION_TYPE;

public:
	Quadtree(Quadtree *tree);
	Quadtree(const Aabb& aabb);
	~Quadtree();

	virtual void AddObject(GameObject *node, int depth = 0)override;
	virtual void RemoveAllObjects()override;
	virtual void FindVisibleObjects(Camera *camera, bool found_visible)override;
	virtual void FindObjects(List<GameObject*>& list, const Ray& ray)override;

private:
	// 添加节点
	void AddNode(GameObject *node);
	// 删除节点
	void RemoveNode(GameObject *node);

	int  NumOfNode()const { return m_iNumNode; };

	// 是否是两倍大小
	bool Container(const Aabb &box)const;
	// 看成四等份，看box属于那一部分；方式是判断中心点处于哪个Tree
	void GetChildIndex(const Aabb &box, int *x, int *y)const;

	// 递归增/减
	void Ref();
	void Unref();

private:
	int			m_iNumNode = 0;
	ListNodes	_nodes;				//这棵树当前节点关联的节点列表

	Aabb		_box;

	Quadtree*	_parentTree = nullptr;	//父节点,根节点的话为NULL
	Quadtree*	_childrens[2][2] = { nullptr };//四个子节点
};
DC_END_NAMESPACE
