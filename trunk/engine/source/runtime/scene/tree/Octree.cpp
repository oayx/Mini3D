#include "Octree.h"
#include "runtime/scene/Camera.h"
#include "runtime/scene/SceneManager.h"
#include "runtime/scene/LayerMask.h"
#include "runtime/component/Transform.h"
#include "runtime/Application.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(Octree, Scenetree);
Octree::Octree(Octree *octree)
	: _parentTree(octree)
{
	for ( int i = 0; i < 2; i++ )
	{
		for ( int j = 0; j < 2; j++ )
		{
			for ( int k = 0; k < 2; k++ )
			{
				_childrens[i][j][k] = nullptr;
			}
		}
	}
}
Octree::Octree(const Aabb& aabb)
	: _box(aabb), _parentTree(nullptr)
{
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				_childrens[i][j][k] = nullptr;
			}
		}
	}
}
Octree::~Octree()
{
	for ( int i = 0; i < 2; i++ )
	{
		for ( int j = 0; j < 2; j++ )
		{
			for ( int k = 0; k < 2; k++ )
			{
				if ( _childrens[ i ][ j ][ k ] != 0 )
				{
					SAFE_DELETE(_childrens[ i ][ j ][ k ]);
				}
			}
		}
	}

	_nodes.Clear();
	_parentTree = 0;
}
void Octree::AddObject(GameObject *node, int depth)
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
		int x, y, z;
		this->GetChildIndex(box, &x, &y, &z);
		if (this->_childrens[x][y][z] == nullptr)
		{
			this->_childrens[x][y][z] = Octree::Create(this);
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

			if (z == 0)
			{
				min.z = octantMin.z;
				max.z = (octantMin.z + octantMax.z) / 2;
			}
			else
			{
				min.z = (octantMin.z + octantMax.z) / 2;
				max.z = octantMax.z;
			}

			this->_childrens[x][y][z]->_box.SetExtents(min, max);
		}
		this->_childrens[x][y][z]->AddObject(node, ++depth);
	}
	else
	{//如果当前节点容不下，理论上需要放到父节点(父节点已经判断可以容下)
		if (_parentTree != nullptr)
			_parentTree->AddNode(node);
		else
			this->AddNode(node);
	}
}
void Octree::RemoveAllObjects()
{
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				if (_childrens[i][j][k] != 0)
				{
					_childrens[i][j][k]->RemoveAllObjects();
				}
			}
		}
	}

	_nodes.Clear();
	_numNode = 0;
}
void Octree::FindVisibleObjects(Camera *camera, bool foundVisible)
{
	if (camera == nullptr || this->NumOfNode() == 0)
		return;

	FrustumVisible v = FrustumVisible::None;
	if (foundVisible)
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
			List<Renderer*> renderables = node->GetComponents<Renderer>();
			for (auto renderable : renderables)
			{
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
					uint objMask = LayerMask::GetMask(node->GetLayer());
					if ((objMask & camera->GetCullMask()))
					{
						camera->AddRenderable(renderable);
					}
				}
			}
		}

		Octree *child;
		bool bChildFoundVisible = (v == FrustumVisible::Full);

		if ((child = _childrens[0][0][0]) != nullptr)
		{
			child->FindVisibleObjects(camera, bChildFoundVisible);
		}

		if ((child = _childrens[1][0][0]) != nullptr)
		{
			child->FindVisibleObjects(camera, bChildFoundVisible);
		}

		if ((child = _childrens[0][1][0]) != nullptr)
		{
			child->FindVisibleObjects(camera, bChildFoundVisible);
		}

		if ((child = _childrens[1][1][0]) != nullptr)
		{
			child->FindVisibleObjects(camera, bChildFoundVisible);
		}

		if ((child = _childrens[0][0][1]) != nullptr)
		{
			child->FindVisibleObjects(camera, bChildFoundVisible);
		}

		if ((child = _childrens[1][0][1]) != nullptr)
		{
			child->FindVisibleObjects(camera, bChildFoundVisible);
		}

		if ((child = _childrens[0][1][1]) != nullptr)
		{
			child->FindVisibleObjects(camera, bChildFoundVisible);
		}

		if ((child = _childrens[1][1][1]) != nullptr)
		{
			child->FindVisibleObjects(camera, bChildFoundVisible);
		}
	}
}
void Octree::FindObjects(List<GameObject*>& list, const Ray& ray)
{
	if (this->NumOfNode() == 0)
		return;

	bool isIntersect = false;
	if (_parentTree == nullptr)
	{//根
		isIntersect = true;
	}
	else
	{
		isIntersect = ray.Intersects(_box).first;
	}

	if (isIntersect)
	{
		for (const auto& node : _nodes)
		{
			if (ray.Intersects(node->GetTransform()->GetBoundingBox()).first)
			{
				list.Add(node);
			}
		}

		Octree *child;
		if ((child = _childrens[0][0][0]) != nullptr)
		{
			child->FindObjects(list, ray);
		}

		if ((child = _childrens[1][0][0]) != nullptr)
		{
			child->FindObjects(list, ray);
		}

		if ((child = _childrens[0][1][0]) != nullptr)
		{
			child->FindObjects(list, ray);
		}

		if ((child = _childrens[1][1][0]) != nullptr)
		{
			child->FindObjects(list, ray);
		}

		if ((child = _childrens[0][0][1]) != nullptr)
		{
			child->FindObjects(list, ray);
		}

		if ((child = _childrens[1][0][1]) != nullptr)
		{
			child->FindObjects(list, ray);
		}

		if ((child = _childrens[0][1][1]) != nullptr)
		{
			child->FindObjects(list, ray);
		}

		if ((child = _childrens[1][1][1]) != nullptr)
		{
			child->FindObjects(list, ray);
		}
	}
}
void Octree::AddNode(GameObject *node)
{
	_nodes.Add(node);
	Ref();
}
void Octree::RemoveNode(GameObject *node)
{
	_nodes.Remove(node);
	Unref();
}
bool Octree::Container(const Aabb &box) const
{
	if (box.IsInfinite())
		return false;
	return _box.Contains(box);
}
void Octree::GetChildIndex(const Aabb &box, int *x, int *y, int *z) const
{
	Vector3 center = _box.GetMaximum().MidPoint(_box.GetMinimum());
	Vector3 centerBox = box.GetMaximum().MidPoint(box.GetMinimum());
	if (centerBox.x > center.x)
		*x = 1;
	else
		*x = 0;

	if (centerBox.y > center.y)
		*y = 1;
	else
		*y = 0;

	if (centerBox.z > center.z)
		*z = 1;
	else
		*z = 0;
}
void Octree::Ref()
{
	_numNode++;
	if (_parentTree != nullptr)
	{
		_parentTree->Ref();
	}
}
void Octree::Unref()
{
	_numNode--;
	if (_parentTree != nullptr)
	{
		_parentTree->Unref();
	}
}
DC_END_NAMESPACE
