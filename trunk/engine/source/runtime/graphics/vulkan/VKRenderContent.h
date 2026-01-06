 
/*****************************************************************************
* Author： hannibal
* Date：2020/2/20
* Description：渲染
*****************************************************************************/
#pragma once

#include "runtime/graphics/null/RenderContent.h"
#include "VKDefine.h"

DC_BEGIN_NAMESPACE
class Primitive;
class VKPipelineState;
/********************************************************************/
class VKRenderContent final : public RenderContent
{
	friend class VKDevice;
	DEFAULT_CREATE(VKRenderContent);
	FRIEND_CONSTRUCT_DESTRUCT(VKRenderContent);
	DISALLOW_COPY_ASSIGN(VKRenderContent);
	BEGIN_DERIVED_REFECTION_TYPE(VKRenderContent, RenderContent)
	END_REFECTION_TYPE;

	VKRenderContent();
	virtual ~VKRenderContent();

public:
	virtual void Initialize()override;

	virtual void SetFillMode(FillMode mode)override;
	virtual void SetShadeMode(ShadeMode mode)override;

	virtual void PreRender(RenderWindow* window)override;
	virtual void PostRender()override;
	virtual void BeginFrame(RenderFrameDesc& desc)override;
	virtual void RenderOnePrimitive(Camera* camera, Pass* pass, Primitive* primitive, RenderMode mode)override;
	virtual void Present(uint sync)override;

	virtual void SetViewport(const ViewPortDesc& viewPort)override;
	virtual void SetViewportScissor(const iRect& clip)override;
	virtual void ClearBackbuffer(void* target_buffer, void* depth_stencil_buffer, ClearFlag flag, const Color& color)override;

	virtual void Resize(const WindowResizeDesc& desc)override;

public:
	VkCommandBuffer GetCommandBuffer()const { return _commandBuffer[_frameIndex]; }

private:
	void CreateCommandObjects();

	void UpdateRenderState(Camera* camera, Pass* pass, HardwareVertexBuffer* vertex_buffer);
	void SetRasterizerState(VkPipelineRasterizationStateCreateInfo& desc, Camera* camera, Pass* pass);
	void SetBlendState(VkPipelineColorBlendAttachmentState& desc, Pass* pass);
	void SetDepthStencilTest(VkPipelineDepthStencilStateCreateInfo& desc, Pass* pass);
	void SetShader(Vector<VkPipelineShaderStageCreateInfo>& shader_desc, Pass* pass);
	void SetSubPrimitiveState(const SubPrimitive* sub_prim, Pass* pass);
	void SetVertexDeclaration(VkVertexInputBindingDescription& binding_desc, Vector<VkVertexInputAttributeDescription>& attribute_descs, VkPipelineInputAssemblyStateCreateInfo& ia_desc, HardwareVertexBuffer* vertex_buffer, VertexSemantic input_semantic);


private:
	VkCommandPool		_commandPool[VKBackBufferCount] = { VK_NULL_HANDLE };
	VkCommandBuffer		_commandBuffer[VKBackBufferCount] = { VK_NULL_HANDLE };
	VkFence				_fence[VKBackBufferCount] = { VK_NULL_HANDLE };
	VkSemaphore			_imageAcquiredSemaphore[VKBackBufferCount] = { VK_NULL_HANDLE };
	VkSemaphore			_renderCompleteSemaphore[VKBackBufferCount] = { VK_NULL_HANDLE };

	uint				_frameIndex = 0;
	uint				_semaphoreIndex = 0;

	std::unordered_map<uint64, VkPipeline> _pipelineStates;
};
DC_END_NAMESPACE