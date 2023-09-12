#include "Quadtree.h"
#include "runtime/scene/Camera.h"
#include "runtime/scene/SceneManager.h"
#include "runtime/scene/LayerMask.h"
#include "runtime/component/Transform.h"
#include "runtime/Application.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(Quadtree, Scenetree);
Quadtree::Quadtree(Quadtree *tree)
	: _parentTree(tree)
{
	for ( int i = 0; i < 2; i++ )
	{
		for ( int j = 0; j < 2; j++ )
		{
			_childrens[i][j] = nullptr;
		}
	}
}
Quadtree::Quadtree(const Aabb& aabb)
	: _parentTree(nullptr), _box(aabb)
{
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			_childrens[i][j] = nullptr;
		}
	}
}
Quadtree::~Quadtree()
{
	for ( int i = 0; i < 2; i++ )
	{
		for ( int j = 0; j < 2; j++ )
		{
			if (_childrens[i][j] != 0)
			{
				SAFE_DELETE(_childrens[i][j]);
			}
		}
	}

	_nodes.Clear();
	_parentTree = 0;
}
void Quadtree::AddObject(GameObject *node, int depth)
{
	if (node->HasFlag(GameObjectFlag::AlwaysFindInScene))
	{
		this->AddNode(node);
		return;
	}

	Aabb box = node->GetTransform()->GetBoundingBox();
	//先判断是否已经超出最大包围盒
	if (depth == 0)
	{
		if (box.IsNull())
		{
			return;
		}
		//测试是否相交，如果不相交就在box里面，原因是_octree->_box已经初始化为足够大
		if (!node->GetTransform()->IsIntersects(_box))
		{
			this->AddNode(node);
			return;
		}
	}
	if ((depth < 8) && this->Container(box))
	{
		int x, y;
		this->GetChildIndex(box, &x, &y);
		if (this->_childrens[x][y] == nullptr)
		{
			this->_childrens[x][y] = Quadtree::Create(this);
			const Vector3& octantMin = this->_box.GetMinimum();
			const Vector3& octantMax = this->_box.GetMaximum();
			Vector3 min, max;
			if (x == 0)
			{
				min.x = octantMin.x;
				max.x = (octantMin.x + octantMax.x) / 2;
			}
			else
			{
				min.x = (octantMin.x + octantMax.x) / 2;
				max.x = octantMax.x;
			}

			if (y == 0)
			{
				min.y = octantMin.y;
				max.y = (octantMin.y + octantMax.y) / 2;
			}
			else
			{
				min.y = (octantMin.y + octantMax.y) / 2;
				max.y = octantMax.y;
			}

			this->_childrens[x][y]->_box.SetExtents(min, max);
		}
		this->_childrens[x][y]->AddObject(node, ++depth);
	}
	else
	{//如果当前节点容不下，理论上需要放到父节点(父节点已经判断可以容下)
		if (_parentTree != nullptr)
			_parentTree->AddNode(node);
		else
			this->AddNode(node);
	}
}
void Quadtree::RemoveAllObjects()
{
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			if (_childrens[i][j] != 0)
			{
				_childrens[i][j]->RemoveAllObjects();
			}
		}
	}

	_nodes.Clear();
	m_iNumNode = 0;
}
void Quadtree::FindVisibleObjects(Camera *camera, bool found_visible)
{
	if (camera == nullptr || this->NumOfNode() == 0)
		return;

	FrustumVisible v = FrustumVisible::None;
	if (found_visible)
	{
		v = FrustumVisible::Full;
	}
	else if (_parentTree == nullptr)
	{
		v = FrustumVisible::Partial;
	}
	else
	{
		v = camera->GetVisibility(_box);
	}
	if (v != FrustumVisible::None)
	{
		//这个树节点关联的节点
		FrustumVisible vis = FrustumVisible::None;
		for (const auto& node : _nodes)
		{
			Renderer* renderable = node->GetComponent<Renderer>();
			if (!renderable)continue;

			vis = FrustumVisible::None;
			if (node->HasFlag(GameObjectFlag::AlwaysFindInScene))
			{
				if (!(Application::IsEditor() && (node->HasFlag(GameObjectFlag::VisibleEditorCamera) && !camera->GetGameObject()->HasFlag(GameObjectFlag::EditorCamera))))
					vis = FrustumVisible::Full;
			}
			else
			{
				if (Application::IsEditor() && (node->HasFlag(GameObjectFlag::VisibleEditorCamera) && !camera->GetGameObject()->HasFlag(GameObjectFlag::EditorCamera)))
				{
					vis = FrustumVisible::None;
				}
				else if (v == FrustumVisible::Partial)
				{
					vis = camera->GetVisibility(node->GetTransform()->GetBoundingBox());
				}
				else if (v == FrustumVisible::Full)
				{
					vis = FrustumVisible::Full;
				}
			}

			if ((uint)vis > 0)
			{
				uint obj_mask = LayerMask::GetMask(node->GetLayer());
				if ((obj_mask & camera->GetCullMask()) != 0)
				{
					Renderer* renderable = node->GetComponent<Renderer>();
					if (renderable != nullptr)camera->AddRenderable(renderable);
				}
			}
		}

		Quadtree *child;
		bool bChildFoundVisible = (v == FrustumVisible::Full);

		if ((child = _childrens[0][0]) != nullptr)
		{
			child->FindVisibleObjects(camera, bChildFoundVisible);
		}

		if ((child = _childrens[1][0]) != nullptr)
		{
			child->FindVisibleObjects(camera, bChildFoundVisible);
		}

		if ((child = _childrens[0][1]) != nullptr)
		{
			child->FindVisibleObjects(camera, bChildFoundVisible);
		}

		if ((child = _childrens[1][1]) != nullptr)
		{
			child->FindVisibleObjects(camera, bChildFoundVisible);
		}

		if ((child = _childrens[0][0]) != nullptr)
		{
			child->FindVisibleObjects(camera, bChildFoundVisible);
		}

		if ((child = _childrens[1][0]) != nullptr)
		{
			child->FindVisibleObjects(camera, bChildFoundVisible);
		}

		if ((child = _childrens[0][1]) != nullptr)
		{
			child->FindVisibleObjects(camera, bChildFoundVisible);
		}

		if ((child = _childrens[1][1]) != nullptr)
		{
			child->FindVisibleObjects(camera, bChildFoundVisible);
		}
	}
}
void Quadtree::FindObjects(List<GameObject*>& list, const Ray& ray)
{
	if (this->NumOfNode() == 0)
		return;

	bool is_intersect = false;
	if (_parentTree == nullptr)
	{//根
		is_intersect = true;
	}
	else
	{
		is_intersect = ray.Intersects(_box).first;
	}

	if (is_intersect)
	{
		for (const auto& node : _nodes)
		{
			if (ray.Intersects(node->GetTransform()->GetBoundingBox()).first)
			{
				list.Add(node);
			}
		}

		Quadtree *child;
		if ((child = _childrens[0][0]) != nullptr)
		{
			child->FindObjects(list, ray);
		}

		if ((child = _childrens[1][0]) != nullptr)
		{
			child->FindObjects(list, ray);
		}

		if ((child = _childrens[0][1]) != nullptr)
		{
			child->FindObjects(list, ray);
		}

		if ((child = _childrens[1][1]) != nullptr)
		{
			child->FindObjects(list, ray);
		}

		if ((child = _childrens[0][0]) != nullptr)
		{
			child->FindObjects(list, ray);
		}

		if ((child = _childrens[1][0]) != nullptr)
		{
			child->FindObjects(list, ray);
		}

		if ((child = _childrens[0][1]) != nullptr)
		{
			child->FindObjects(list, ray);
		}

		if ((child = _childrens[1][1]) != nullptr)
		{
			child->FindObjects(list, ray);
		}
	}
}
void Quadtree::AddNode(GameObject *node)
{
	_nodes.Add(node);
	Ref();
}
void Quadtree::RemoveNode(GameObject *node)
{
	_nodes.Remove(node);
	Unref();
}
bool Quadtree::Container(const Aabb &box) const
{
	if (box.IsInfinite())
		return false;

	Rect rect1(_box.GetCenter().x, _box.GetCenter().y, _box.GetSize().x, _box.GetSize().y);
	Rect rect2(box.GetCenter().x, box.GetCenter().y, box.GetSize().x, box.GetSize().y);
	return rect1.Contains(rect2);
}
void Quadtree::GetChildIndex(const Aabb &box, int *x, int *y) const
{
	Vector3 center = _box.GetMaximum().MidPoint(_box.GetMinimum());
	Vector3 center_box = box.GetMaximum().MidPoint(box.GetMinimum());
	if (center_box.x > center.x)
	{
		*x = 1;
	}
	else
	{
		*x = 0;
	}

	if (center_box.y > center.y)
	{
		*y = 1;
	}
	else
	{
		*y = 0;
	}
}
void Quadtree::Ref()
{
	m_iNumNode++;
	if (_parentTree != nullptr)
	{
		_parentTree->Ref();
	}
}
void Quadtree::Unref()
{
	m_iNumNode--;
	if (_parentTree != nullptr)
	{
		_parentTree->Unref();
	}
}
DC_END_NAMESPACE
