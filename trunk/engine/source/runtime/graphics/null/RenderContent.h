 
/*****************************************************************************
* Author： hannibal
* Date：2009/11/23
* Description： 渲染
*****************************************************************************/
#pragma once

#include "core/color/Color.h"
#include "core/event/EventController.h"
#include "runtime/CommonDefine.h"
#include "runtime/graphics/RenderDefine.h"
#include "runtime/scene/SceneUtils.h"

DC_BEGIN_NAMESPACE
class Pass;
class Camera;
class CGProgram;
class Material;
class Renderer;
class Primitive;
class RenderWindow;
class HardwareVertexBuffer;
struct SubPrimitive;
/********************************************************************/
class RenderContent : public EventController
{
	friend class GraphicsDevice;
	FRIEND_CONSTRUCT_DESTRUCT(RenderContent);
	DISALLOW_COPY_ASSIGN(RenderContent);
	BEGIN_DERIVED_REFECTION_TYPE(RenderContent, EventController)
	END_DERIVED_REFECTION_TYPE;

protected:
	RenderContent();
	virtual ~RenderContent(); 

public:
	virtual void Initialize() {}

	virtual void SetFillMode(FillMode mode) {  }
	virtual void SetShadeMode(ShadeMode mode) {  }

	virtual void PreRender(RenderWindow* window);
	virtual void PostRender();
	virtual void BeginFrame(RenderFrameDesc& desc) { _isInsideFrame = true; }
	virtual void EndFrame() { _isInsideFrame = false; }
	virtual void RenderOneObject(Camera* camera, Material* material, Renderer *renderable, RenderMode mode);
	virtual void RenderOneObject(Camera* camera, Vector<Material*> materials, Renderer *renderable, RenderMode mode);
	virtual void RenderOneObject(Camera* camera, Pass* pass, Renderer *renderable, RenderMode mode);
	virtual void RenderOnePrimitive(Camera* camera, Material* material, Primitive *primitive, RenderMode mode);
	virtual void RenderOnePrimitive(Camera* camera, Pass* pass, Primitive* primitive, RenderMode mode) = 0;
	virtual void Present(uint sync) = 0;

	virtual void SetViewport(const ViewPortDesc& view_port) = 0;//把位置从NDC坐标系转换成屏幕坐标系
	virtual void SetViewportScissor(const iRect& clip) = 0;//裁剪
	virtual void ClearBackbuffer(void* target_buffer, void* depth_stencil_buffer, ClearFlag flag, const Color& color) = 0;

	virtual void Resize(const WindowResizeDesc& desc);

public:
	bool			IsInsideFrame()const { return _isInsideFrame; }
	RenderWindow*	GetRenderWindow()const { return _currRenderWindow; }

protected:
	bool			_isInsideFrame = false;
	RenderWindow*	_currRenderWindow = nullptr;
};//RenderContent
DC_END_NAMESPACE
