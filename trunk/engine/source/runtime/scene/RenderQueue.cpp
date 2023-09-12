#include "RenderQueue.h"
#include "Camera.h"
#include "Light.h"
#include "runtime/graphics/Material.h"
#include "runtime/graphics/ShadowMap.h"
#include "runtime/graphics/null/RenderContent.h"
#include "runtime/graphics/RenderDepthMap.h"
#include "runtime/Application.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(RenderQueue, Object);
RenderQueue::RenderQueue()
{
}
RenderQueue::~RenderQueue()
{
	for (auto obj : _renderableGroups)
	{
		SAFE_DELETE(obj.second);
	}
	_renderableGroups.Clear();
	for (auto obj : _transparentRenderableGroups)
	{
		SAFE_DELETE(obj.second);
	}
	_transparentRenderableGroups.Clear();
}
void RenderQueue::AddRenderable(Camera* camera, Renderer *renderable)
{
	if (renderable == nullptr)return;

	RenderQueueGroup *render_group = nullptr;
	int64 sort_layer = renderable->GetSortLayer();
	int64 order = renderable->GetOrderInLayer();
	int64 max_value = UINT_MAX;
	int64 group_id = (max_value*(sort_layer < 0 ? -1 : (sort_layer > 0 ? 1 : 0)) + sort_layer) + (order);

	//区分透明和非透明组
	GroupCollection& group = renderable->GetRenderQueue() < int(eRenderQueue::AlphaTest) ? _renderableGroups : _transparentRenderableGroups;

	GroupCollection::iterator it = group.begin();
	while (it != group.end())
	{
		auto obj = (*it);
		if (obj.first == group_id)
		{//已经有了
			render_group = obj.second;
			break;
		}
		else if(group_id < obj.first)
		{//插入到指定位置
			render_group = RenderQueueGroup::Create(group_id);
			group.Insert(it, std::make_pair(group_id, render_group));
			break;
		}
		++it;
	}
	if (render_group == nullptr)
	{//原有队列为空或是最后一个
		render_group = RenderQueueGroup::Create(group_id);
		group.Add(std::make_pair(group_id, render_group));
	}

	render_group->AddRenderable(camera, renderable);
}
void RenderQueue::Clear()
{
	for (auto obj : _renderableGroups)
	{
		obj.second->Clear();
	}
	for (auto obj : _transparentRenderableGroups)
	{
		obj.second->Clear();
	}
}
void RenderQueue::Render(Camera* camera)
{
	for (const auto& obj : _renderableGroups)
	{
		obj.second->Render(camera);
	}
	for (const auto& obj : _transparentRenderableGroups)
	{
		obj.second->Render(camera);
	}
}
void RenderQueue::RenderDepthTexture(Camera* camera, RenderDepthMap* depth_map)
{
	for (const auto& obj : _renderableGroups)
	{
		obj.second->RenderDepthTexture(camera, depth_map);
	}
	//NOTE:深度图不渲染透明物体
}
void RenderQueue::RenderShadowTexture(Camera* camera, Light* light)
{
	for (const auto& obj : _renderableGroups)
	{
		obj.second->RenderShadowTexture(camera, light);
	}
	for (const auto& obj : _transparentRenderableGroups)
	{
		obj.second->RenderShadowTexture(camera, light);
	}
}
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(RenderQueueGroup, Object);
RenderQueueGroup::RenderQueueGroup(int64 group_id)
: _groupId(group_id)
{
}
void RenderQueueGroup::AddRenderable(Camera* camera, Renderer *renderable)
{
	if (std::find(_renderables.begin(), _renderables.end(), renderable) != _renderables.end())
	{
		Debuger::Warning("RenderQueueGroup::AddRenderable - The same object has been added");
		return;
	}
	bool is_insert = false;
	int render_queue = renderable->GetRenderQueue();
	float distance_new = renderable->GetTransform()->GetPosition().Distance(camera->GetTransform()->GetPosition());
	for (auto it = _renderables.begin(); it != _renderables.end(); ++it)
	{
		if (render_queue < (*it)->GetRenderQueue())
		{
			_renderables.Insert(it, renderable);
			is_insert = true;
			break;
		}
		else if (render_queue == (*it)->GetRenderQueue())
		{
			float distance = (*it)->GetTransform()->GetPosition().Distance(camera->GetTransform()->GetPosition());
			if (distance_new > distance)
			{
				if (IsTransparentQueue(render_queue))
				{//透明物体先远后近渲染
					_renderables.Insert(it, renderable);
					is_insert = true;
					break;
				}
			}
			else if (distance_new < distance)
			{
				if (!IsTransparentQueue(render_queue))
				{//非透明物体先近后远渲染
					_renderables.Insert(it, renderable);
					is_insert = true;
					break;
				}
			}
		}
	}
	if (!is_insert)
	{
		_renderables.Add(renderable);
	}
}
void RenderQueueGroup::Render(Camera* camera)
{
	for (const auto& readerable : _renderables)
	{
		if (!readerable->IsEnable())
			continue;

		readerable->CalLightes();
		readerable->GetGameObject()->PostComponentAction(ComponentAction::PreRender, false, false, camera);

		Application::GetGraphics()->GetRenderContent()->RenderOneObject(camera, readerable->GetMaterials(), readerable, RenderMode::Normal);

		readerable->GetGameObject()->PostComponentAction(ComponentAction::PostRender, false, false, camera);

		readerable->GetGameObject()->PostComponentAction(ComponentAction::DrawGizmos, false, false, camera);
	}
}
void RenderQueueGroup::RenderDepthTexture(Camera* camera, RenderDepthMap* depth_map)
{
	for (const auto& obj : _renderables)
	{
		depth_map->RenderOneObject(camera, obj);
	}
}
void RenderQueueGroup::RenderShadowTexture(Camera* camera, Light* light)
{
	if (light->GetShadowMap() != nullptr)
	{
		for (const auto& obj : _renderables)
		{
			if (obj->GetMaterial() != nullptr)
			{
				if (!obj->GetMaterial()->IsCastShadow())continue;
			}
			light->GetShadowMap()->RenderOneObject(camera, light, obj);
		}
	}
}
DC_END_NAMESPACE