#include "Nonetree.h"
#include "runtime/scene/Camera.h"
#include "runtime/scene/SceneManager.h"
#include "runtime/scene/LayerMask.h"
#include "runtime/component/Transform.h"
#include "runtime/Application.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(Nonetree, Scenetree);
void Nonetree::AddObject(GameObject *node, int depth)
{
	_nodes.Add(node);
}
void Nonetree::RemoveAllObjects()
{
	_nodes.Clear();
}
void Nonetree::FindVisibleObjects(Camera *camera, bool foundVisible)
{
	if (camera == nullptr)
		return;

	for (const auto& node : _nodes)
	{
		uint objMask = LayerMask::GetMask(node->GetLayer());
		if ((objMask & camera->GetCullMask()) != 0)
		{
			Renderer* renderable = node->GetComponent<Renderer>();
			if (!renderable)continue;

			if (Application::IsEditor() && (node->HasFlag(GameObjectFlag::VisibleEditorCamera) && !camera->GetGameObject()->HasFlag(GameObjectFlag::EditorCamera)))
				continue;

			camera->AddRenderable(renderable);
		}
	}
}
void Nonetree::FindObjects(List<GameObject*>& list, const Ray& ray)
{
	for (const auto& node : _nodes)
	{
		if (ray.Intersects(node->GetTransform()->GetBoundingBox()).first)
		{
			list.Add(node);
		}
	}
}
DC_END_NAMESPACE