
/*****************************************************************************************************/
// @author hannibal
// @date   2017/04/2
// @brief  平衡二叉树
/*****************************************************************************************************/
#pragma once

#include "core/BaseType.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
//平衡二叉树结点
template <typename T>
struct AVLNode
{
	T data;
	int height = 0; //结点所在高度
	AVLNode<T> *left = nullptr;
	AVLNode<T> *right = nullptr;
	AVLNode<T>(const T theData) : data(theData), left(nullptr), right(nullptr), height(0) {}
};
/********************************************************************/
/*平衡二叉树
	平衡二叉树（Balanced Binary Tree）又被称为AVL树（有别于AVL算法），且具有以下性质：
	它是一 棵空树或它的左右两个子树的高度差的绝对值不超过1，并且左右两个子树都是一棵平衡二叉树
*/
/*二叉查找树
	二叉查找树（BST：Binary Search Tree）是一种特殊的二叉树，它改善了二叉树节点查找的效率。二叉查找树有以下性质：
	对于任意一个节点 n:
	(1) 其左子树（left subtree）下的每个后代节点（descendant node）的值都小于节点 n 的值；
	(2) 其右子树（right subtree）下的每个后代节点的值都大于节点 n 的值。
*/
template <class T>
class AVLTree
{
public:
	AVLTree<T>() {}
	~AVLTree<T>() { SAFE_DELETE(_rootNode); }

	//插入结点
	void Add(T x) noexcept;
	//删除结点
	bool Remove(T x) noexcept;
	//查找是否存在给定值的结点
	bool Contains(const T x) const noexcept;
	//前序遍历
	void PreOrder() noexcept;
	//中序遍历
	void InOrder() noexcept;

private:
	//插入结点
	void Add(AVLNode<T> *&t, T x) noexcept;
	//删除结点
	bool Remove(AVLNode<T> *&t, T x) noexcept;
	//查找是否存在给定值的结点
	bool Contains(AVLNode<T> *t, const T x) const noexcept;
	//前序遍历
	void PreOrder(AVLNode<T> *t) noexcept;
	//中序遍历
	void InOrder(AVLNode<T> *t) noexcept;
	AVLNode<T> *FindMin(AVLNode<T> *t) const noexcept;
	AVLNode<T> *FindMax(AVLNode<T> *t) const noexcept;
	//求树的高度
	int GetHeight(AVLNode<T> *t) noexcept;
	//单旋转 左
	AVLNode<T> *LL(AVLNode<T> *t) noexcept;
	//单旋转 右
	AVLNode<T> *RR(AVLNode<T> *t) noexcept;
	//双旋转 右左
	AVLNode<T> *LR(AVLNode<T> *t) noexcept;
	//双旋转 左右
	AVLNode<T> *RL(AVLNode<T> *t) noexcept;

private:
	AVLNode<T> *_rootNode = nullptr;
};
template <typename T>
void AVLTree<T>::Add(T x) noexcept
{
	Add(_rootNode, x);
}
template <typename T>
void AVLTree<T>::Add(AVLNode<T> *&t, T x) noexcept
{
	if (t == nullptr)
		t = new AVLNode<T>(x);
	else if (x < t->data)
	{
		Add(t->left, x);
		//判断平衡情况
		if (GetHeight(t->left) - GetHeight(t->right) > 1)
		{
			//分两种情况 左左或左右

			if (x < t->left->data)//左左
				t = LL(t);
			else                  //左右
				t = LR(t);
		}
	}
	else if (x > t->data)
	{
		Add(t->right, x);
		if (GetHeight(t->right) - GetHeight(t->left) > 1)
		{
			if (x > t->right->data)
				t = RR(t);
			else
				t = RL(t);
		}
	}
	else//数据重复
	{
		t->height = std::max(GetHeight(t->left), GetHeight(t->right)) + 1;
	}
}
template <typename T>
bool AVLTree<T>::Remove(T x) noexcept
{
	return Remove(_rootNode, x);
}
template <typename T>
bool AVLTree<T>::Remove(AVLNode<T> *&t, T x) noexcept
{
	//t为空 未找到要删除的结点
	if (t == nullptr)
		return false;
	//找到了要删除的结点
	else if (t->data == x)
	{
		//左右子树都非空
		if (t->left != nullptr && t->right != nullptr)
		{//在高度更大的那个子树上进行删除操作

			//左子树高度大，删除左子树中值最大的结点，将其赋给根结点
			if (GetHeight(t->left) > GetHeight(t->right))
			{
				t->data = FindMax(t->left)->data;
				Remove(t->left, t->data);
			}
			else//右子树高度更大，删除右子树中值最小的结点，将其赋给根结点
			{
				t->data = FindMin(t->right)->data;
				Remove(t->right, t->data);
			}
		}
		else
		{//左右子树有一个不为空，直接用需要删除的结点的子结点替换即可
			AVLNode<T> *old = t;
			t = t->left ? t->left : t->right;//t赋值为不空的子结点
			delete (old);
		}
	}
	else if (x < t->data)//要删除的结点在左子树上
	{
		//递归删除左子树上的结点
		Remove(t->left, x);
		//判断是否仍然满足平衡条件
		if (GetHeight(t->right) - GetHeight(t->left) > 1)
		{
			if (GetHeight(t->right->left) > GetHeight(t->right->right))
			{
				//RL双旋转
				t = RL(t);
			}
			else
			{//RR单旋转
				t = RR(t);
			}
		}
		else//满足平衡条件 调整高度信息
		{
			t->height = std::max(GetHeight(t->left), GetHeight(t->right)) + 1;
		}
	}
	else//要删除的结点在右子树上
	{
		//递归删除右子树结点
		Remove(t->right, x);
		//判断平衡情况
		if (GetHeight(t->left) - GetHeight(t->right) > 1)
		{
			if (GetHeight(t->left->right) > GetHeight(t->left->left))
			{
				//LR双旋转
				t = LR(t);
			}
			else
			{
				//LL单旋转
				t = LL(t);
			}
		}
		else//满足平衡性 调整高度
		{
			t->height = std::max(GetHeight(t->left), GetHeight(t->right)) + 1;
		}
	}

	return true;
}
//查找结点
template <typename T>
bool AVLTree<T>::Contains(const T x) const noexcept
{
	return Contains(_rootNode, x);
}
template <typename T>
bool AVLTree<T>::Contains(AVLNode<T> *t, const T x) const noexcept
{
	if (t == nullptr)
		return false;
	if (x < t->data)
		return Contains(t->left, x);
	else if (x > t->data)
		return Contains(t->right, x);
	else
		return true;
}
//前序遍历
template <typename T>
void AVLTree<T>::PreOrder() noexcept
{
	PreOrder(_rootNode);
}
template <typename T>
void AVLTree<T>::PreOrder(AVLNode<T> *t) noexcept
{
	if (t)
	{
		std::cout << t->data << ' ';
		PreOrder(t->left);
		PreOrder(t->right);
	}
}
//中序遍历
template <typename T>
void AVLTree<T>::InOrder() noexcept
{
	InOrder(_rootNode);
}
template <typename T>
void AVLTree<T>::InOrder(AVLNode<T> *t) noexcept
{
	if (t)
	{
		InOrder(t->left);
		std::cout << t->data << ' ';
		InOrder(t->right);
	}
}
template <typename T>
AVLNode<T> * AVLTree<T>::FindMax(AVLNode<T> *t) const noexcept
{
	if (t == nullptr)
		return nullptr;
	if (t->right == nullptr)
		return t;
	return FindMax(t->right);
}

template <typename T>
AVLNode<T> * AVLTree<T>::FindMin(AVLNode<T> *t) const noexcept
{
	if (t == nullptr)
		return nullptr;
	if (t->left == nullptr)
		return t;
	return FindMin(t->left);
}
template <typename T>
int AVLTree<T>::GetHeight(AVLNode<T> *t) noexcept
{
	if (t == nullptr)
		return -1;
	else
		return t->height;
}
//单旋转
//左左插入导致的不平衡
template <typename T>
AVLNode<T> * AVLTree<T>::LL(AVLNode<T> *t) noexcept
{
	AVLNode<T> *q = t->left;
	t->left = q->right;
	q->right = t;
	t = q;
	t->height = std::max(GetHeight(t->left), GetHeight(t->right)) + 1;
	q->height = std::max(GetHeight(q->left), GetHeight(q->right)) + 1;
	return q;
}
//单旋转
//右右插入导致的不平衡
template <typename T>
AVLNode<T> * AVLTree<T>::RR(AVLNode<T> *t) noexcept
{
	AVLNode<T> *q = t->right;
	t->right = q->left;
	q->left = t;
	t = q;
	t->height = std::max(GetHeight(t->left), GetHeight(t->right)) + 1;
	q->height = std::max(GetHeight(q->left), GetHeight(q->right)) + 1;
	return q;
}
//双旋转
//插入点位于t的左儿子的右子树
template <typename T>
AVLNode<T> * AVLTree<T>::LR(AVLNode<T> *t) noexcept
{
	//双旋转可以通过两次单旋转实现
	//对t的左结点进行RR旋转，再对根节点进行LL旋转
	RR(t->left);
	return LL(t);
}
//双旋转
//插入点位于t的右儿子的左子树
template <typename T>
AVLNode<T> * AVLTree<T>::RL(AVLNode<T> *t) noexcept
{
	LL(t->right);
	return RR(t);
}
DC_END_NAMESPACE