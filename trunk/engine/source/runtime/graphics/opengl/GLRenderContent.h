 
/*****************************************************************************
* Author： hannibal
* Date：2020/2/20
* Description：d3d渲染
*****************************************************************************/
#pragma once

#include "runtime/graphics/null/RenderContent.h"
#include "GLDefine.h"

DC_BEGIN_NAMESPACE
class Primitive;
/********************************************************************/
class GLRenderContent Final : public RenderContent
{
	friend class GLDevice;
	DEFAULT_CREATE(GLRenderContent);
	FRIEND_CONSTRUCT_DESTRUCT(GLRenderContent);
	DISALLOW_COPY_ASSIGN(GLRenderContent);
	BEGIN_DERIVED_REFECTION_TYPE(GLRenderContent, RenderContent)
	END_DERIVED_REFECTION_TYPE;

	GLRenderContent() {}
	virtual ~GLRenderContent();

public:
	virtual void Initialize()override;

	virtual void SetFillMode(FillMode mode)override;
	virtual void SetShadeMode(ShadeMode mode)override;

	virtual void PreRender(RenderWindow* window)override;
	virtual void PostRender()override;
	virtual void BeginFrame(RenderFrameDesc& desc)override;
	virtual void RenderOnePrimitive(Camera* camera, Pass* pass, Primitive* primitive, RenderMode mode)override;
	virtual void Present(uint sync)override {}

	virtual void SetViewport(const ViewPortDesc& view_port)override;
	virtual void SetViewportScissor(const iRect& clip)override;
	virtual void ClearBackbuffer(void* target_buffer, void* depth_stencil_buffer, ClearFlag flag, const Color& color)override;

	virtual void Resize(const WindowResizeDesc& desc)override;

private:
	void UpdataRenderState(Camera* camera, Pass* pass, Primitive* primitive);
	void SetCullMode(Pass* pass);
	void SetScissor(Pass* pass);
	void SetDepth(Pass* pass);
	void SetStencil(Pass* pass);
	void SetColorMask(Pass* pass);
	void SetBlendState(Pass* pass);
	void SetDepthOffset(Pass* pass);
	void SetSubPrimitiveState(const SubPrimitive* sub_prim, Pass* pass);
};
DC_END_NAMESPACE