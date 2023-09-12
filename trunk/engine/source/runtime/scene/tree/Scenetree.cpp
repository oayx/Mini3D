#include "Scenetree.h"
#include "Nonetree.h"
#include "Octree.h"
#include "Quadtree.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(Scenetree, Object);
static Scenetree* g_Scenetree = nullptr;
void CreateSceneTree(SceneTreeType type)
{
	if (g_Scenetree != nullptr)
		DestroySceneTree();
	switch (type)
	{
	case dc::SceneTreeType::None:
		g_Scenetree = Nonetree::Create();
		break;
	case dc::SceneTreeType::Octree:
		g_Scenetree = Octree::Create(Aabb(Vector3(-10000.0f, -10000.0f, -10000.0f), Vector3(10000.0f, 10000.0f, 10000.0f)));
		break;
	case dc::SceneTreeType::Quadtree:
		g_Scenetree = Quadtree::Create(Aabb(Vector3(-10000.0f, -10000.0f, -10000.0f), Vector3(10000.0f, 10000.0f, 10000.0f)));
		break;
	}
}
void DestroySceneTree()
{
	SAFE_DELETE(g_Scenetree);
}
Scenetree* GetSceneTree()
{
	return g_Scenetree;
}
DC_END_NAMESPACE
