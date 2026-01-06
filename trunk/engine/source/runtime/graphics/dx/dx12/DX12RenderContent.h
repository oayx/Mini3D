 
/*****************************************************************************
* Author： hannibal
* Date：2020/2/20
* Description：d3d渲染
*****************************************************************************/
#pragma once

#include "runtime/graphics/null/RenderContent.h"
#include "DX12Define.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class DX12RenderContent final : public RenderContent
{
	friend class DX12Device;
	DEFAULT_CREATE(DX12RenderContent);
	FRIEND_CONSTRUCT_DESTRUCT(DX12RenderContent);
	DISALLOW_COPY_ASSIGN(DX12RenderContent);
	BEGIN_DERIVED_REFECTION_TYPE(DX12RenderContent, RenderContent)
	END_REFECTION_TYPE;

	DX12RenderContent();
	virtual ~DX12RenderContent();

public:
	virtual void Initialize()override;

	virtual void PreRender(RenderWindow* window)override { base::PreRender(window); }
	virtual void PostRender()override { base::PostRender(); }
	virtual void BeginFrame(RenderFrameDesc& desc)override;
	virtual void RenderOnePrimitive(Camera* camera, Pass* pass, Primitive* primitive, RenderMode mode)override;
	virtual void Present(uint sync)override {}

	virtual void SetViewport(const ViewPortDesc& viewPort)override;
	virtual void SetViewportScissor(const iRect& clip)override;
	virtual void ClearBackbuffer(void* targetBuffer, void* depthStencilBuffer, ClearFlag flag, const Color& color)override;

	virtual void Resize(const WindowResizeDesc& desc)override { base::Resize(desc); }

private:
	void UpdataRenderState(Camera* camera, Pass* pass, HardwareVertexBuffer* vertex_buffer);
	void SetRasterizerState(D3D12_GRAPHICS_PIPELINE_STATE_DESC& pso_desc, Camera* camera, Pass* pass);
	void SetBlendState(D3D12_GRAPHICS_PIPELINE_STATE_DESC& pso_desc, Pass* pass);
	void SetDepthStencilTest(D3D12_GRAPHICS_PIPELINE_STATE_DESC& pso_desc, Pass* pass);
	void SetShader(D3D12_GRAPHICS_PIPELINE_STATE_DESC& pso_desc, Pass* pass);
	void SetSubPrimitiveState(const SubPrimitive* sub_prim, Pass* pass);
	void SetVertexDeclaration(Vector<D3D12_INPUT_ELEMENT_DESC>& element_desc, HardwareVertexBuffer* vertex_buffer, VertexSemantic input_semantic);

private:
	std::unordered_map<uint64, ID3D12PipelineState*> _pipelineStates;
};
DC_END_NAMESPACE

