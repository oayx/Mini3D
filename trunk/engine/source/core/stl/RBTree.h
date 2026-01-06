
/*****************************************************************************************************/
// @author hannibal
// @date   2017/04/2
// @brief  红黑树
/*****************************************************************************************************/
#pragma once

#include "core/BaseType.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
enum RBTColor { RED, BLACK };

template <class T>
class RBTNode 
{
public:
	RBTColor color;		// 颜色
	T key;				// 关键字(键值)
	RBTNode *left;		// 左孩子
	RBTNode *right;		// 右孩子
	RBTNode *parent;	// 父结点

	RBTNode(T value, RBTColor c, RBTNode *p, RBTNode *l, RBTNode *r) :
		key(value), color(c), parent(), left(l), right(r) {}
};
/********************************************************************/
/*红黑树
	红黑树(Red - Black Tree，简称R - B Tree)，它一种特殊的二叉查找树。
	红黑树是特殊的二叉查找树，意味着它满足二叉查找树的特征：任意一个节点所包含的键值，大于等于左孩子的键值，小于等于右孩子的键值。
	除了具备该特性之外，红黑树还包括许多额外的信息。

	红黑树的每个节点上都有存储位表示节点的颜色，颜色是红(Red)或黑(Black)。
	红黑树的特性 :
	(1) 每个节点或者是黑色，或者是红色。
	(2) 根节点是黑色。
	(3) 每个叶子节点是黑色。[注意：这里叶子节点，是指为空的叶子节点！]
	(4) 如果一个节点是红色的，则它的子节点必须是黑色的。
	(5) 从一个节点到该节点的子孙节点的所有路径上包含相同数目的黑节点。

	关于它的特性，需要注意的是：
	第一，特性(3)中的叶子节点，是只为空(NIL或null)的节点。
	第二，特性(5)，确保没有一条路径会比其他路径长出俩倍。因而，红黑树是相对是接近平衡的二叉树。
*/
template <class T>
class RBTree final : public object
{
	DISALLOW_COPY_ASSIGN(RBTree);

public:
	RBTree();
	~RBTree();

	// 前序遍历"红黑树"
	void PreOrder() noexcept;
	// 中序遍历"红黑树"
	void InOrder() noexcept;
	// 后序遍历"红黑树"
	void PostOrder() noexcept;

	// (递归实现)查找"红黑树"中键值为key的节点
	RBTNode<T>* Search(T key) noexcept;
	// (非递归实现)查找"红黑树"中键值为key的节点
	RBTNode<T>* IterativeSearch(T key) noexcept;

	// 查找最小结点：返回最小结点的键值。
	T Minimum() noexcept;
	// 查找最大结点：返回最大结点的键值。
	T Maximum() noexcept;

	// 找结点(x)的后继结点。即，查找"红黑树中数据值大于该结点"的"最小结点"。
	RBTNode<T>* Successor(RBTNode<T> *x) noexcept;
	// 找结点(x)的前驱结点。即，查找"红黑树中数据值小于该结点"的"最大结点"。
	RBTNode<T>* Predecessor(RBTNode<T> *x) noexcept;

	// 将结点(key为节点键值)插入到红黑树中
	void Insert(T key) noexcept;

	// 删除结点(key为节点键值)
	void Remove(T key) noexcept;

	// 打印红黑树
	void Print() noexcept;

private:
	// 前序遍历"红黑树"
	void PreOrder(RBTNode<T>* tree) const noexcept;
	// 中序遍历"红黑树"
	void InOrder(RBTNode<T>* tree) const noexcept;
	// 后序遍历"红黑树"
	void PostOrder(RBTNode<T>* tree) const noexcept;

	// (递归实现)查找"红黑树x"中键值为key的节点
	RBTNode<T>* Search(RBTNode<T>* x, T key) const noexcept;
	// (非递归实现)查找"红黑树x"中键值为key的节点
	RBTNode<T>* IterativeSearch(RBTNode<T>* x, T key) const noexcept;

	// 查找最小结点：返回tree为根结点的红黑树的最小结点。
	RBTNode<T>* Minimum(RBTNode<T>* tree) noexcept;
	// 查找最大结点：返回tree为根结点的红黑树的最大结点。
	RBTNode<T>* Maximum(RBTNode<T>* tree) noexcept;

	// 左旋
	void LeftRotate(RBTNode<T>* &root, RBTNode<T>* x) noexcept;
	// 右旋
	void RightRotate(RBTNode<T>* &root, RBTNode<T>* y) noexcept;
	// 插入函数
	void Insert(RBTNode<T>* &root, RBTNode<T>* node) noexcept;
	// 插入修正函数
	void InsertFixUp(RBTNode<T>* &root, RBTNode<T>* node) noexcept;
	// 删除函数
	void Remove(RBTNode<T>* &root, RBTNode<T> *node) noexcept;
	// 删除修正函数
	void RemoveFixUp(RBTNode<T>* &root, RBTNode<T> *node, RBTNode<T> *parent) noexcept;

	// 销毁红黑树
	void Destroy(RBTNode<T>* &tree) noexcept;

	// 打印红黑树
	void Print(RBTNode<T>* tree, T key, int direction) noexcept;

private:
	RBTNode<T> *_rootNode;    // 根结点
};
DC_END_NAMESPACE