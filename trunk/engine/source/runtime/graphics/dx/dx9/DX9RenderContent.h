
/*****************************************************************************
* Author： hannibal
* Date：2020年2月20日
* Description：d3d渲染
*****************************************************************************/
#pragma once

#include "runtime/graphics/null/RenderContent.h"
#include "DX9Define.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class DX9RenderContent Final : public RenderContent
{
	friend class DX9Device;
	DEFAULT_CREATE(DX9RenderContent);
	FRIEND_CONSTRUCT_DESTRUCT(DX9RenderContent);
	BEGIN_DERIVED_REFECTION_TYPE(DX9RenderContent, RenderContent)
	END_DERIVED_REFECTION_TYPE;

	DX9RenderContent();
	~DX9RenderContent();

public:
	virtual void Initialize()override;

	virtual void SetFillMode(FillMode mode)override;
	virtual void SetShadeMode(ShadeMode mode)override;

	virtual void PreRender(RenderWindow* window)override;
	virtual void PostRender()override;
	virtual void BeginFrame(RenderFrameDesc& desc)override;
	virtual void EndFrame()override;
	virtual void RenderOnePrimitive(Camera* camera, Pass* pass, Primitive* primitive, RenderMode mode)override;
	virtual void Present(uint sync)override {}

	virtual void SetViewport(const ViewPortDesc& view_port)override;
	virtual void SetViewportScissor(const iRect& clip)override;
	virtual void ClearBackbuffer(void* target_buffer, void* depth_stencil_buffer, ClearFlag flag, const Color& color)override;

	virtual void Resize(const WindowResizeDesc& desc)override;

private:
	void ResetRenderState();
	void UpdataRenderState(Camera* camera, Pass* pass, Primitive* primitive);
	void SetCullMode(Pass* pass);
	void SetScissor(Pass* pass);
	void SetDepth(Pass* pass);
	void SetStencil(Pass* pass);
	void SetColorMask(Pass* pass);
	void SetBlendState(Pass* pass);
	void SetSubPrimitiveState(const SubPrimitive* sub_prim, Pass* pass);

	void HandleLostDevice();
	void HandleResetDevice();

private:
	std::unordered_map<uint, IDirect3DVertexDeclaration9*> _vertexDeclarations;
};
DC_END_NAMESPACE