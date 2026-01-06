#include "RBTree.h"
#include <iomanip>

#define rb_parent(r)		((r)->parent)
#define rb_color(r)			((r)->color)
#define rb_is_red(r)		((r)->color==RED)
#define rb_is_black(r)		((r)->color==BLACK)
#define rb_set_black(r)		do { (r)->color = BLACK; } while (0)
#define rb_set_red(r)		do { (r)->color = RED; } while (0)
#define rb_set_parent(r,p)  do { (r)->parent = (p); } while (0)
#define rb_set_color(r,c)	do { (r)->color = (c); } while (0)
DC_BEGIN_NAMESPACE
/*****************************************************************************************************/
template <class T>
RBTree<T>::RBTree() :_rootNode(NULL)
{
	_rootNode = NULL;
}
template <class T>
RBTree<T>::~RBTree()
{
	Destroy(_rootNode);
}

template <class T>
void RBTree<T>::PreOrder(RBTNode<T>* tree) const noexcept
{
	if (tree != NULL)
	{
		std::cout << tree->key << " ";
		PreOrder(tree->left);
		PreOrder(tree->right);
	}
}
template <class T>
void RBTree<T>::PreOrder() noexcept
{
	PreOrder(_rootNode);
}

template <class T>
void RBTree<T>::InOrder(RBTNode<T>* tree) const noexcept
{
	if (tree != NULL)
	{
		InOrder(tree->left);
		std::cout << tree->key << " ";
		InOrder(tree->right);
	}
}
template <class T>
void RBTree<T>::InOrder() noexcept
{
	InOrder(_rootNode);
}

template <class T>
void RBTree<T>::PostOrder(RBTNode<T>* tree) const noexcept
{
	if (tree != NULL)
	{
		PostOrder(tree->left);
		PostOrder(tree->right);
		std::cout << tree->key << " ";
	}
}
template <class T>
void RBTree<T>::PostOrder() noexcept
{
	PostOrder(_rootNode);
}

template <class T>
RBTNode<T>* RBTree<T>::Search(RBTNode<T>* x, T key) const noexcept
{
	if (x == NULL || x->key == key)
		return x;

	if (key < x->key)
		return Search(x->left, key);
	else
		return Search(x->right, key);
}
template <class T>
RBTNode<T>* RBTree<T>::Search(T key) noexcept
{
	Search(_rootNode, key);
}

/*
 * (非递归实现)查找"红黑树x"中键值为key的节点
 */
template <class T>
RBTNode<T>* RBTree<T>::IterativeSearch(RBTNode<T>* x, T key) const noexcept
{
	while ((x != NULL) && (x->key != key))
	{
		if (key < x->key)
			x = x->left;
		else
			x = x->right;
	}

	return x;
}
template <class T>
RBTNode<T>* RBTree<T>::IterativeSearch(T key) noexcept
{
	IterativeSearch(_rootNode, key);
}

template <class T>
RBTNode<T>* RBTree<T>::Minimum(RBTNode<T>* tree) noexcept
{
	if (tree == NULL)
		return NULL;

	while (tree->left != NULL)
		tree = tree->left;
	return tree;
}
template <class T>
T RBTree<T>::Minimum() noexcept
{
	RBTNode<T> *p = Minimum(_rootNode);
	if (p != NULL)
		return p->key;

	return (T)NULL;
}

template <class T>
RBTNode<T>* RBTree<T>::Maximum(RBTNode<T>* tree) noexcept
{
	if (tree == NULL)
		return NULL;

	while (tree->right != NULL)
		tree = tree->right;
	return tree;
}
template <class T>
T RBTree<T>::Maximum() noexcept
{
	RBTNode<T> *p = Maximum(_rootNode);
	if (p != NULL)
		return p->key;

	return (T)NULL;
}

template <class T>
RBTNode<T>* RBTree<T>::Successor(RBTNode<T> *x) noexcept
{
	// 如果x存在右孩子，则"x的后继结点"为 "以其右孩子为根的子树的最小结点"。
	if (x->right != NULL)
		return Minimum(x->right);

	// 如果x没有右孩子。则x有以下两种可能：
	// (01) x是"一个左孩子"，则"x的后继结点"为 "它的父结点"。
	// (02) x是"一个右孩子"，则查找"x的最低的父结点，并且该父结点要具有左孩子"，找到的这个"最低的父结点"就是"x的后继结点"。
	RBTNode<T>* y = x->parent;
	while ((y != NULL) && (x == y->right))
	{
		x = y;
		y = y->parent;
	}

	return y;
}

template <class T>
RBTNode<T>* RBTree<T>::Predecessor(RBTNode<T> *x) noexcept
{
	// 如果x存在左孩子，则"x的前驱结点"为 "以其左孩子为根的子树的最大结点"。
	if (x->left != NULL)
		return Maximum(x->left);

	// 如果x没有左孩子。则x有以下两种可能：
	// (01) x是"一个右孩子"，则"x的前驱结点"为 "它的父结点"。
	// (01) x是"一个左孩子"，则查找"x的最低的父结点，并且该父结点要具有右孩子"，找到的这个"最低的父结点"就是"x的前驱结点"。
	RBTNode<T>* y = x->parent;
	while ((y != NULL) && (x == y->left))
	{
		x = y;
		y = y->parent;
	}

	return y;
}

/*
 * 对红黑树的节点(x)进行左旋转
 *
 * 左旋示意图(对节点x进行左旋)：
 *      px                              px
 *     /                               /
 *    x                               y
 *   /  \      --(左旋)-->           / \                #
 *  lx   y                          x  ry
 *     /   \                       /  \
 *    ly   ry                     lx  ly
 *
 *
 */
template <class T>
void RBTree<T>::LeftRotate(RBTNode<T>* &root, RBTNode<T>* x) noexcept
{
	// 设置x的右孩子为y
	RBTNode<T> *y = x->right;

	// 将 “y的左孩子” 设为 “x的右孩子”；
	// 如果y的左孩子非空，将 “x” 设为 “y的左孩子的父亲”
	x->right = y->left;
	if (y->left != NULL)
		y->left->parent = x;

	// 将 “x的父亲” 设为 “y的父亲”
	y->parent = x->parent;

	if (x->parent == NULL)
	{
		root = y;            // 如果 “x的父亲” 是空节点，则将y设为根节点
	}
	else
	{
		if (x->parent->left == x)
			x->parent->left = y;    // 如果 x是它父节点的左孩子，则将y设为“x的父节点的左孩子”
		else
			x->parent->right = y;    // 如果 x是它父节点的左孩子，则将y设为“x的父节点的左孩子”
	}

	// 将 “x” 设为 “y的左孩子”
	y->left = x;
	// 将 “x的父节点” 设为 “y”
	x->parent = y;
}

/*
 * 对红黑树的节点(y)进行右旋转
 *
 * 右旋示意图(对节点y进行左旋)：
 *            py                               py
 *           /                                /
 *          y                                x
 *         /  \      --(右旋)-->            /  \                     #
 *        x   ry                           lx   y
 *       / \                                   / \                   #
 *      lx  rx                                rx  ry
 *
 */
template <class T>
void RBTree<T>::RightRotate(RBTNode<T>* &root, RBTNode<T>* y) noexcept
{
	// 设置x是当前节点的左孩子。
	RBTNode<T> *x = y->left;

	// 将 “x的右孩子” 设为 “y的左孩子”；
	// 如果"x的右孩子"不为空的话，将 “y” 设为 “x的右孩子的父亲”
	y->left = x->right;
	if (x->right != NULL)
		x->right->parent = y;

	// 将 “y的父亲” 设为 “x的父亲”
	x->parent = y->parent;

	if (y->parent == NULL)
	{
		root = x;            // 如果 “y的父亲” 是空节点，则将x设为根节点
	}
	else
	{
		if (y == y->parent->right)
			y->parent->right = x;    // 如果 y是它父节点的右孩子，则将x设为“y的父节点的右孩子”
		else
			y->parent->left = x;    // (y是它父节点的左孩子) 将x设为“x的父节点的左孩子”
	}

	// 将 “y” 设为 “x的右孩子”
	x->right = y;

	// 将 “y的父节点” 设为 “x”
	y->parent = x;
}

/*
 * 红黑树插入修正函数
 *
 * 在向红黑树中插入节点之后(失去平衡)，再调用该函数；
 * 目的是将它重新塑造成一颗红黑树。
 *
 * 参数说明：
 *     root 红黑树的根
 *     node 插入的结点        // 对应《算法导论》中的z
 */
template <class T>
void RBTree<T>::InsertFixUp(RBTNode<T>* &root, RBTNode<T>* node) noexcept
{
	RBTNode<T> *parent, *gparent;

	// 若“父节点存在，并且父节点的颜色是红色”
	while ((parent = rb_parent(node)) && rb_is_red(parent))
	{
		gparent = rb_parent(parent);

		//若“父节点”是“祖父节点的左孩子”
		if (parent == gparent->left)
		{
			// Case 1条件：叔叔节点是红色
			{
				RBTNode<T> *uncle = gparent->right;
				if (uncle && rb_is_red(uncle))
				{
					rb_set_black(uncle);
					rb_set_black(parent);
					rb_set_red(gparent);
					node = gparent;
					continue;
				}
			}

			// Case 2条件：叔叔是黑色，且当前节点是右孩子
			if (parent->right == node)
			{
				RBTNode<T> *tmp;
				LeftRotate(root, parent);
				tmp = parent;
				parent = node;
				node = tmp;
			}

			// Case 3条件：叔叔是黑色，且当前节点是左孩子。
			rb_set_black(parent);
			rb_set_red(gparent);
			RightRotate(root, gparent);
		}
		else//若“z的父节点”是“z的祖父节点的右孩子”
		{
			// Case 1条件：叔叔节点是红色
			{
				RBTNode<T> *uncle = gparent->left;
				if (uncle && rb_is_red(uncle))
				{
					rb_set_black(uncle);
					rb_set_black(parent);
					rb_set_red(gparent);
					node = gparent;
					continue;
				}
			}

			// Case 2条件：叔叔是黑色，且当前节点是左孩子
			if (parent->left == node)
			{
				RBTNode<T> *tmp;
				RightRotate(root, parent);
				tmp = parent;
				parent = node;
				node = tmp;
			}

			// Case 3条件：叔叔是黑色，且当前节点是右孩子。
			rb_set_black(parent);
			rb_set_red(gparent);
			LeftRotate(root, gparent);
		}
	}

	// 将根节点设为黑色
	rb_set_black(root);
}

/*
 * 将结点插入到红黑树中
 *
 * 参数说明：
 *     root 红黑树的根结点
 *     node 插入的结点        // 对应《算法导论》中的node
 */
template <class T>
void RBTree<T>::Insert(RBTNode<T>* &root, RBTNode<T>* node) noexcept
{
	RBTNode<T> *y = NULL;
	RBTNode<T> *x = root;

	// 1. 将红黑树当作一颗二叉查找树，将节点添加到二叉查找树中。
	while (x != NULL)
	{
		y = x;
		if (node->key < x->key)
			x = x->left;
		else
			x = x->right;
	}

	node->parent = y;
	if (y != NULL)
	{
		if (node->key < y->key)
			y->left = node;
		else
			y->right = node;
	}
	else
		root = node;

	// 2. 设置节点的颜色为红色
	node->color = RED;

	// 3. 将它重新修正为一颗二叉查找树
	InsertFixUp(root, node);
}

/*
 * 将结点(key为节点键值)插入到红黑树中
 *
 * 参数说明：
 *     tree 红黑树的根结点
 *     key 插入结点的键值
 */
template <class T>
void RBTree<T>::Insert(T key) noexcept
{
	RBTNode<T> *z = NULL;

	// 如果新建结点失败，则返回。
	if ((z = new RBTNode<T>(key, BLACK, NULL, NULL, NULL)) == NULL)
		return;

	Insert(_rootNode, z);
}

/*
 * 红黑树删除修正函数
 *
 * 在从红黑树中删除插入节点之后(红黑树失去平衡)，再调用该函数；
 * 目的是将它重新塑造成一颗红黑树。
 *
 * 参数说明：
 *     root 红黑树的根
 *     node 待修正的节点
 */
template <class T>
void RBTree<T>::RemoveFixUp(RBTNode<T>* &root, RBTNode<T> *node, RBTNode<T> *parent) noexcept
{
	RBTNode<T> *other;

	while ((!node || rb_is_black(node)) && node != root)
	{
		if (parent->left == node)
		{
			other = parent->right;
			if (rb_is_red(other))
			{
				// Case 1: x的兄弟w是红色的
				rb_set_black(other);
				rb_set_red(parent);
				LeftRotate(root, parent);
				other = parent->right;
			}
			if ((!other->left || rb_is_black(other->left)) &&
				(!other->right || rb_is_black(other->right)))
			{
				// Case 2: x的兄弟w是黑色，且w的俩个孩子也都是黑色的
				rb_set_red(other);
				node = parent;
				parent = rb_parent(node);
			}
			else
			{
				if (!other->right || rb_is_black(other->right))
				{
					// Case 3: x的兄弟w是黑色的，并且w的左孩子是红色，右孩子为黑色。
					rb_set_black(other->left);
					rb_set_red(other);
					RightRotate(root, other);
					other = parent->right;
				}
				// Case 4: x的兄弟w是黑色的；并且w的右孩子是红色的，左孩子任意颜色。
				rb_set_color(other, rb_color(parent));
				rb_set_black(parent);
				rb_set_black(other->right);
				LeftRotate(root, parent);
				node = root;
				break;
			}
		}
		else
		{
			other = parent->left;
			if (rb_is_red(other))
			{
				// Case 1: x的兄弟w是红色的
				rb_set_black(other);
				rb_set_red(parent);
				RightRotate(root, parent);
				other = parent->left;
			}
			if ((!other->left || rb_is_black(other->left)) &&
				(!other->right || rb_is_black(other->right)))
			{
				// Case 2: x的兄弟w是黑色，且w的俩个孩子也都是黑色的
				rb_set_red(other);
				node = parent;
				parent = rb_parent(node);
			}
			else
			{
				if (!other->left || rb_is_black(other->left))
				{
					// Case 3: x的兄弟w是黑色的，并且w的左孩子是红色，右孩子为黑色。
					rb_set_black(other->right);
					rb_set_red(other);
					LeftRotate(root, other);
					other = parent->left;
				}
				// Case 4: x的兄弟w是黑色的；并且w的右孩子是红色的，左孩子任意颜色。
				rb_set_color(other, rb_color(parent));
				rb_set_black(parent);
				rb_set_black(other->left);
				RightRotate(root, parent);
				node = root;
				break;
			}
		}
	}
	if (node)
		rb_set_black(node);
}

/*
 * 删除结点(node)，并返回被删除的结点
 *
 * 参数说明：
 *     root 红黑树的根结点
 *     node 删除的结点
 */
template <class T>
void RBTree<T>::Remove(RBTNode<T>* &root, RBTNode<T> *node) noexcept
{
	RBTNode<T> *child, *parent;
	RBTColor color;

	// 被删除节点的"左右孩子都不为空"的情况。
	if ((node->left != NULL) && (node->right != NULL))
	{
		// 被删节点的后继节点。(称为"取代节点")
		// 用它来取代"被删节点"的位置，然后再将"被删节点"去掉。
		RBTNode<T> *replace = node;

		// 获取后继节点
		replace = replace->right;
		while (replace->left != NULL)
			replace = replace->left;

		// "node节点"不是根节点(只有根节点不存在父节点)
		if (rb_parent(node))
		{
			if (rb_parent(node)->left == node)
				rb_parent(node)->left = replace;
			else
				rb_parent(node)->right = replace;
		}
		else
			// "node节点"是根节点，更新根节点。
			root = replace;

		// child是"取代节点"的右孩子，也是需要"调整的节点"。
		// "取代节点"肯定不存在左孩子！因为它是一个后继节点。
		child = replace->right;
		parent = rb_parent(replace);
		// 保存"取代节点"的颜色
		color = rb_color(replace);

		// "被删除节点"是"它的后继节点的父节点"
		if (parent == node)
		{
			parent = replace;
		}
		else
		{
			// child不为空
			if (child)
				rb_set_parent(child, parent);
			parent->left = child;

			replace->right = node->right;
			rb_set_parent(node->right, replace);
		}

		replace->parent = node->parent;
		replace->color = node->color;
		replace->left = node->left;
		node->left->parent = replace;

		if (color == BLACK)
			RemoveFixUp(root, child, parent);

		delete node;
		return;
	}

	if (node->left != NULL)
		child = node->left;
	else
		child = node->right;

	parent = node->parent;
	// 保存"取代节点"的颜色
	color = node->color;

	if (child)
		child->parent = parent;

	// "node节点"不是根节点
	if (parent)
	{
		if (parent->left == node)
			parent->left = child;
		else
			parent->right = child;
	}
	else
		root = child;

	if (color == BLACK)
		RemoveFixUp(root, child, parent);
	delete node;
}

/*
 * 删除红黑树中键值为key的节点
 *
 * 参数说明：
 *     tree 红黑树的根结点
 */
template <class T>
void RBTree<T>::Remove(T key) noexcept
{
	RBTNode<T> *node;

	// 查找key对应的节点(node)，找到的话就删除该节点
	if ((node = Search(_rootNode, key)) != NULL)
		Remove(_rootNode, node);
}

template <class T>
void RBTree<T>::Destroy(RBTNode<T>* &tree) noexcept
{
	if (tree == NULL)
		return;

	if (tree->left != NULL)
		return Destroy(tree->left);
	if (tree->right != NULL)
		return Destroy(tree->right);

	delete tree;
	tree = NULL;
}

/*
 * 打印"二叉查找树"
 *
 * key        -- 节点的键值
 * direction  --  0，表示该节点是根节点;
 *               -1，表示该节点是它的父结点的左孩子;
 *                1，表示该节点是它的父结点的右孩子。
 */
template <class T>
void RBTree<T>::Print(RBTNode<T>* tree, T key, int direction) noexcept
{
	if (tree != NULL)
	{
		if (direction == 0)    // tree是根节点
			std::cout << std::setw(2) << tree->key << "(B) is root" << std::endl;
		else                // tree是分支节点
			std::cout << std::setw(2) << tree->key << (rb_is_red(tree) ? "(R)" : "(B)") << " is " << std::setw(2) << key << "'s " << std::setw(12) << (direction == 1 ? "right child" : "left child") << std::endl;

		Print(tree->left, tree->key, -1);
		Print(tree->right, tree->key, 1);
	}
}

template <class T>
void RBTree<T>::Print() noexcept
{
	if (_rootNode != NULL)
		Print(_rootNode, _rootNode->key, 0);
}
DC_END_NAMESPACE