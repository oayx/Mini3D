
/*****************************************************************************
* Author： hannibal
* Date：2009/12/8
* Description：  
*****************************************************************************/
#pragma once

#include "runtime/scene/SceneUtils.h"

DC_BEGIN_NAMESPACE
class Light;
class Camera;
class Renderer;
class RenderDepthMap;
class RenderQueueGroup;
/********************************************************************/
// ClassName：RenderQueue
// Description：渲染队列管理
class RenderQueue final : public Object
{
	typedef List<std::pair<int64, RenderQueueGroup*>> GroupCollection;
	DEFAULT_CREATE(RenderQueue);
	FRIEND_CONSTRUCT_DESTRUCT(RenderQueue);
	DISALLOW_COPY_ASSIGN(RenderQueue);
	BEGIN_DERIVED_REFECTION_TYPE(RenderQueue, Object)
	END_REFECTION_TYPE;

public:
	RenderQueue();
	~RenderQueue();

public:
	//添加
	void AddRenderable(Camera* camera, Renderer *renderable);

public:
	void Clear();
	//渲染所有组
	void Render(Camera* camera);
	void RenderDepthTexture(Camera* camera, RenderDepthMap* depth_map);
	void RenderShadowTexture(Camera* camera, Light* light);

private:
	GroupCollection _renderableGroups;
	GroupCollection _transparentRenderableGroups;
};//RenderQueue

/********************************************************************/
// ClassName：RenderQueueGroup
// Description：渲染分组
class ENGINE_DLL RenderQueueGroup final : public Object
{
	typedef List<Renderer*> Renderables;
	DEFAULT_CREATE(RenderQueueGroup);
	FRIEND_CONSTRUCT_DESTRUCT(RenderQueueGroup);
	DISALLOW_COPY_ASSIGN(RenderQueueGroup);
	BEGIN_DERIVED_REFECTION_TYPE(RenderQueueGroup, Object)
	END_REFECTION_TYPE;

public:
	RenderQueueGroup(int64 groupId);

public:
	//添加到当前组
	void AddRenderable(Camera* camera, Renderer *renderable);

public:
	void Clear() { _renderables.Clear(); }
	//渲染当前组
	void Render(Camera* camera);
	void RenderDepthTexture(Camera* camera, RenderDepthMap* depth_map);
	void RenderShadowTexture(Camera* camera, Light* light);

private:
	int64 _groupId = 0;//分组id
	Renderables _renderables;//内部会按照从小到大排序
};//RenderQueueGroup
DC_END_NAMESPACE
