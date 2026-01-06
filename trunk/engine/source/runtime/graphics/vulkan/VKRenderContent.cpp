#include "VKRenderContent.h"
#include "VKDevice.h"
#include "VKProgram.h"
#include "VKRenderContent.h"
#include "VKTempCommand.h"
#include "VKHardwareVertexBuffer.h"
#include "VKHardwareIndexBuffer.h"
#include "runtime/graphics/Material.h"
#include "runtime/graphics/Pass.h"
#include "runtime/graphics/TextureUnit.h"
#include "runtime/graphics/null/Texture.h"
#include "runtime/graphics/null/CGProgram.h"
#include "runtime/renderable/Renderer.h"
#include "runtime/scene/Camera.h"
#include "platform/RenderWindow.h"
#include "runtime/Application.h"
#include "core/image/Image.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(VKRenderContent, RenderContent);
VKRenderContent::VKRenderContent()
{
}
VKRenderContent::~VKRenderContent()
{
	VkDevice device = GetVKDevice()->GetDevice();
	VkAllocationCallbacks* allocator = GetVKDevice()->GetAllocator();

	VK_ERROR(vkDeviceWaitIdle(device));
	VKTempCommand::Destroy();

	for (auto state : _pipelineStates)
	{
		vkDestroyPipeline(device, state.second, allocator);
	}
	_pipelineStates.clear();

	for (int i = 0; i < VKBackBufferCount; ++i)
	{
		vkFreeCommandBuffers(device, _commandPool[i], 1, &_commandBuffer[i]);
		vkDestroyCommandPool(device, _commandPool[i], allocator);

		vkDestroyFence(device, _fence[i], allocator);

		vkDestroySemaphore(device, _imageAcquiredSemaphore[i], allocator);
		vkDestroySemaphore(device, _renderCompleteSemaphore[i], allocator);

		_commandPool[i] = VK_NULL_HANDLE; 
		_commandBuffer[i] = VK_NULL_HANDLE;
		_fence[i] = VK_NULL_HANDLE;
		_imageAcquiredSemaphore[i] = VK_NULL_HANDLE;
		_renderCompleteSemaphore[i] = VK_NULL_HANDLE;
	}
}
void VKRenderContent::Initialize()
{
	base::Initialize();

	this->CreateCommandObjects();
}
void VKRenderContent::SetFillMode(FillMode mode)
{
}
void VKRenderContent::SetShadeMode(ShadeMode mode)
{
}
void VKRenderContent::PreRender(RenderWindow* window)
{
	base::PreRender(window);

	VkDevice device = GetVKDevice()->GetDevice();
	VkAllocationCallbacks* allocator = GetVKDevice()->GetAllocator();
	VkSurfaceKHR surface = GetVKDevice()->GetSurface();
	uint queue_family = GetVKDevice()->GetQueueFamily();
	VKSwapChain* vk_swapchain = dynamic_cast<VKSwapChain*>(GetVKDevice()->GetSwapChain());

	VkSemaphore image_acquired_semaphore = _imageAcquiredSemaphore[_semaphoreIndex];
	VkSemaphore render_complete_semaphore = _renderCompleteSemaphore[_semaphoreIndex];
	VK_ERROR(vkAcquireNextImageKHR(device, vk_swapchain->GetSwapchain(), UINT64_MAX, image_acquired_semaphore, VK_NULL_HANDLE, &_frameIndex));

	{
		VK_ERROR(vkWaitForFences(device, 1, &_fence[_frameIndex], VK_TRUE, UINT64_MAX));
		VK_ERROR(vkResetFences(device, 1, &_fence[_frameIndex]));
	}
	{
		VK_ERROR(vkResetCommandPool(device, _commandPool[_frameIndex], 0));
		VkCommandBufferBeginInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		VK_ERROR(vkBeginCommandBuffer(_commandBuffer[_frameIndex], &info));
	}
}
void VKRenderContent::PostRender()
{
	VkSemaphore image_acquired_semaphore = _imageAcquiredSemaphore[_semaphoreIndex];
	VkSemaphore render_complete_semaphore = _renderCompleteSemaphore[_semaphoreIndex];

	// Submit command buffer
	vkCmdEndRenderPass(_commandBuffer[_frameIndex]);
	{
		VK_ERROR(vkEndCommandBuffer(_commandBuffer[_frameIndex]));

		VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		VkSubmitInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		info.waitSemaphoreCount = 1;
		info.pWaitSemaphores = &image_acquired_semaphore;
		info.pWaitDstStageMask = &wait_stage;
		info.commandBufferCount = 1;
		info.pCommandBuffers = &_commandBuffer[_frameIndex];
		info.signalSemaphoreCount = 1;
		info.pSignalSemaphores = &render_complete_semaphore;
		VK_ERROR(vkQueueSubmit(GetVKDevice()->GetQueue(), 1, &info, _fence[_frameIndex]));
	}
	base::PostRender();
}
void VKRenderContent::BeginFrame(RenderFrameDesc& desc)
{
	VKSwapChain* vk_swapchain = dynamic_cast<VKSwapChain*>(GetVKDevice()->GetSwapChain());

	Vector<VkClearValue> clear_values;
	switch (desc.clearFlag)
	{
	case ClearFlag::Skybox:
	case ClearFlag::SolidColor:
	{
		VkClearValue clear_value1 = {};
		::memcpy(clear_value1.color.float32, desc.clearColor.ptr(), 4 * sizeof(float));
		clear_values.Add(clear_value1);
		VkClearValue clear_value2 = {};
		clear_value2.depthStencil.depth = 1.0f;
		clear_value2.depthStencil.stencil = 0;
		clear_values.Add(clear_value2);
		break;
	}
	case ClearFlag::DepthOnly:
	{
		VkClearValue clear_value = {};
		clear_value.depthStencil.depth = 1.0f;
		clear_value.depthStencil.stencil = 0;
		clear_values.Add(clear_value);
		break;
	}
	case ClearFlag::DonotClear:
		break;
	}

	VkRenderPassBeginInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	info.renderPass = vk_swapchain->GetRenderPass();
	info.framebuffer = vk_swapchain->GetFramebuffer(_frameIndex);
	info.renderArea.offset.x = desc.viewPort.x;
	info.renderArea.offset.y = desc.viewPort.y;
	info.renderArea.extent.width = desc.viewPort.w;
	info.renderArea.extent.height = desc.viewPort.h;
	info.clearValueCount = clear_values.Size();
	info.pClearValues = clear_values.Data();
	vkCmdBeginRenderPass(_commandBuffer[_frameIndex], &info, VK_SUBPASS_CONTENTS_INLINE);

	SetViewport(desc.viewPort);
	SetViewportScissor(iRect((int)desc.viewPort.x, (int)desc.viewPort.y, (int)desc.viewPort.w, (int)desc.viewPort.h));
}
void VKRenderContent::RenderOnePrimitive(Camera* camera, Pass* pass, Primitive* primitive, RenderMode mode)
{
	//在后台缓冲区绘制图形
	HardwareVertexBuffer* vertex_buffer = primitive->GetVertexData();
	if (vertex_buffer == nullptr || vertex_buffer->GetVertexCount() == 0)//如果模型没有顶点数据，则buffer会为null
		return;

	//Gpu
	CGProgram* shader = pass->GetProgram();
	shader->PreRender();
	shader->Render(camera, pass, primitive->GetWorldMatrix());
	shader->PostRender();

	//顶点声明和数据流
	vertex_buffer->Render(shader);

	//渲染状态更新
	this->UpdateRenderState(camera, pass, vertex_buffer);

	//获得渲染类型和渲染三角形个数
	uint prim_type = VK_GetPrimitiveType(primitive->GetPrimitiveType());

	HardwareIndexBuffer *index_buffer = primitive->GetIndexData();
	if (index_buffer)
	{
		index_buffer->Render();
		for (int i = 0; i < primitive->GetSubPrimitiveCount(); ++i)
		{
			const SubPrimitive& prim = primitive->GetSubPrimitive(i);
			this->SetViewportScissor(prim.ScissorRect);
			this->SetSubPrimitiveState(&prim, pass);
			vkCmdDrawIndexed(_commandBuffer[_frameIndex], prim.ElemCount, 1, prim.StartIndex, prim.StartVertex, 0);

			int prim_count = prim.GetTringles(primitive->GetPrimitiveType());
			SceneManager::AddDrawCall(1);
			SceneManager::AddDrawTriangles(prim_count);
		}
	}
	else
	{
		for (int i = 0; i < primitive->GetSubPrimitiveCount(); ++i)
		{
			const SubPrimitive& prim = primitive->GetSubPrimitive(i);
			this->SetViewportScissor(prim.ScissorRect);
			this->SetSubPrimitiveState(&prim, pass);
			vkCmdDraw(_commandBuffer[_frameIndex], prim.ElemCount, 1, prim.StartVertex, 0);

			int prim_count = prim.GetTringles(primitive->GetPrimitiveType());
			SceneManager::AddDrawCall(1);
			SceneManager::AddDrawTriangles(prim_count);
		}
	}

	//统计
	SceneManager::AddRenderBatch(1);
	SceneManager::AddSetPassCall(1);
}
void VKRenderContent::Present(uint sync)
{
	VKSwapChain* swapchain = dynamic_cast<VKSwapChain*>(GetVKDevice()->GetSwapChain());
	VkSwapchainKHR vk_swapchain = swapchain->GetSwapchain();

	VkSemaphore render_complete_semaphore = _renderCompleteSemaphore[_semaphoreIndex];
	VkPresentInfoKHR info = {};
	info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	info.waitSemaphoreCount = 1;
	info.pWaitSemaphores = &render_complete_semaphore;
	info.swapchainCount = 1;
	info.pSwapchains = &vk_swapchain;
	info.pImageIndices = &_frameIndex;
	VkResult result = vkQueuePresentKHR(GetVKDevice()->GetQueue(), &info);
	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		//TODO:重构交换链
		return;
	}
	VK_ERROR(result);
	_semaphoreIndex = (_semaphoreIndex + 1) % swapchain->GetBufferCount(); // Now we can use the next set of semaphores
}
void VKRenderContent::SetViewport(const ViewPortDesc& viewPort)
{
	VkViewport viewport;
	viewport.x = viewPort.x;
	viewport.y = viewPort.y;
	viewport.width = viewPort.w;
	viewport.height = viewPort.h;
	viewport.minDepth = viewPort.z_near;
	viewport.maxDepth = viewPort.z_far;
	vkCmdSetViewport(_commandBuffer[_frameIndex], 0, 1, &viewport);
}
void VKRenderContent::SetViewportScissor(const iRect& clip)
{
	VkRect2D scissor;
	scissor.offset.x = clip.x;
	scissor.offset.y = clip.y;
	scissor.extent.width = clip.width;
	scissor.extent.height = clip.height;
	vkCmdSetScissor(_commandBuffer[_frameIndex], 0, 1, &scissor);
}
void VKRenderContent::ClearBackbuffer(void* target_buffer, void* depth_stencil_buffer, ClearFlag flag, const Color& color)
{
	AssertEx(false, "");
}
void VKRenderContent::Resize(const WindowResizeDesc& desc)
{ 
	base::Resize(desc);
}
void VKRenderContent::CreateCommandObjects()
{
	VkDevice device = GetVKDevice()->GetDevice();
	VkAllocationCallbacks* allocator = GetVKDevice()->GetAllocator();
	uint queue_family = GetVKDevice()->GetQueueFamily();
	for (uint32_t i = 0; i < 2; i++)
	{
		{
			VkCommandPoolCreateInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			info.queueFamilyIndex = queue_family;
			VK_ERROR(vkCreateCommandPool(device, &info, allocator, &_commandPool[i]));
		}
		{
			VkCommandBufferAllocateInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			info.commandPool = _commandPool[i];
			info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			info.commandBufferCount = 1;
			VK_ERROR(vkAllocateCommandBuffers(device, &info, &_commandBuffer[i]));
		}
		{
			VkFenceCreateInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
			VK_ERROR(vkCreateFence(device, &info, allocator, &_fence[i]));
		}
		{
			VkSemaphoreCreateInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			VK_ERROR(vkCreateSemaphore(device, &info, allocator, &_imageAcquiredSemaphore[i]));
			VK_ERROR(vkCreateSemaphore(device, &info, allocator, &_renderCompleteSemaphore[i]));
		}
	}
}
void VKRenderContent::UpdateRenderState(Camera* camera, Pass* pass, HardwareVertexBuffer* vertex_buffer)
{
	VkPipeline pso = VK_NULL_HANDLE;
	VKProgram* shader = dynamic_cast<VKProgram*>(pass->GetProgram());

	uint64 key = (uint64)pass;
	auto it = _pipelineStates.find(key);
	if (it == _pipelineStates.end())
	{
		VkDevice device = GetVKDevice()->GetDevice();
		VkAllocationCallbacks* allocator = GetVKDevice()->GetAllocator();

		//shader
		Vector<VkPipelineShaderStageCreateInfo> shader_desc;
		this->SetShader(shader_desc, pass);

		//顶点输入
		VkVertexInputBindingDescription binding_desc = {};
		Vector<VkVertexInputAttributeDescription> attribute_descs;
		VkPipelineInputAssemblyStateCreateInfo ia_desc = {};
		this->SetVertexDeclaration(binding_desc, attribute_descs, ia_desc, vertex_buffer, pass->GetProgram()->GetInputVertexSemantic());
		VkPipelineVertexInputStateCreateInfo vertex_desc = {};
		vertex_desc.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertex_desc.vertexBindingDescriptionCount = 1;
		vertex_desc.pVertexBindingDescriptions = &binding_desc;
		vertex_desc.vertexAttributeDescriptionCount = attribute_descs.Size();
		vertex_desc.pVertexAttributeDescriptions = attribute_descs.Data();

		//dynamic
		VkDynamicState dynamic_states[2] = {};
		VkPipelineDynamicStateCreateInfo dynamic_state = {};
		dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamic_state.dynamicStateCount = 0;
		dynamic_state.pDynamicStates = dynamic_states;

		//视口
		VkPipelineViewportStateCreateInfo viewport_desc = {};
		viewport_desc.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewport_desc.viewportCount = 1;
		viewport_desc.scissorCount = 1;
#if !defined(DC_PLATFORM_ANDROID)
		dynamic_states[dynamic_state.dynamicStateCount++] = VK_DYNAMIC_STATE_VIEWPORT;
		dynamic_states[dynamic_state.dynamicStateCount++] = VK_DYNAMIC_STATE_SCISSOR;
		viewport_desc.pScissors = NULL;
		viewport_desc.pViewports = NULL;
#else
		const Size& win_size = WindowManager::GetWindow()->GetWinSize();
		VkViewport viewports;
		viewports.minDepth = 0.0f;
		viewports.maxDepth = 1.0f;
		viewports.x = 0;
		viewports.y = 0;
		viewports.width = win_size.width;
		viewports.height = win_size.height;
		VkRect2D scissor;
		scissor.extent.width = win_size.width;
		scissor.extent.height = win_size.height;
		scissor.offset.x = 0;
		scissor.offset.y = 0;
		viewport_desc.pScissors = &scissor;
		viewport_desc.pViewports = &viewports;
#endif

		//光栅化
		VkPipelineRasterizationStateCreateInfo raster_desc = {};
		this->SetRasterizerState(raster_desc, camera, pass);

		//混合
		VkPipelineColorBlendAttachmentState color_attachment_desc = {};
		this->SetBlendState(color_attachment_desc, pass);
		VkPipelineColorBlendStateCreateInfo blend_desc = {};
		blend_desc.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		blend_desc.attachmentCount = 1;
		blend_desc.pAttachments = &color_attachment_desc;

		//深度模板测试
		VkPipelineDepthStencilStateCreateInfo depth_stencil_desc = {};
		this->SetDepthStencilTest(depth_stencil_desc, pass);

		//MSAA
		VkPipelineMultisampleStateCreateInfo ms_info = {};
		ms_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		ms_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		VkGraphicsPipelineCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		info.flags = 0;
		info.stageCount = shader_desc.Size();
		info.pStages = shader_desc.Data();
		info.pVertexInputState = &vertex_desc;
		info.pInputAssemblyState = &ia_desc;
		info.pViewportState = &viewport_desc;
		info.pRasterizationState = &raster_desc;
		info.pMultisampleState = &ms_info;
		info.pDepthStencilState = &depth_stencil_desc;
		info.pColorBlendState = &blend_desc;
		info.pDynamicState = &dynamic_state;
		info.layout = shader->GetPipelineLayout();
		info.renderPass = dynamic_cast<VKSwapChain*>(Application::GetGraphics()->GetSwapChain())->GetRenderPass();
		VkResult r = (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &info, allocator, &pso));
		_pipelineStates.insert({ key, pso });

	}
	else
	{
		pso = it->second;
	}

	vkCmdBindPipeline(GetVKDevice()->GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pso);
}
void VKRenderContent::SetRasterizerState(VkPipelineRasterizationStateCreateInfo& desc, Camera* camera, Pass* pass)
{
	FillMode fill_mode = FillMode::Solid;
	if (camera)fill_mode = camera->GetFillMode();

	desc.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	desc.polygonMode = VK_GetFillMode(fill_mode); // VK_POLYGON_MODE_LINE;
	desc.cullMode = VK_GetCullMode(pass->Cullmode);
	desc.frontFace = VK_FRONT_FACE_CLOCKWISE;
	desc.rasterizerDiscardEnable = VK_FALSE;//设置为VK_TRUE，那么几何图元永远不会传递到光栅化阶段
	desc.depthClampEnable = VK_FALSE;//设置为VK_TRUE，超过远近裁剪面的片元会进行收敛，而不是丢弃它们
	desc.depthBiasEnable = VK_FALSE;//光栅化可以通过添加常量或者基于片元的斜率来更改深度值。一些时候对于阴影贴图是有用的
	desc.depthBiasConstantFactor = 0.0f; // Optional
	desc.depthBiasClamp = 0.0f; // Optional
	desc.depthBiasSlopeFactor = 0.0f; // Optional
	desc.lineWidth = 1.0f;
}
void VKRenderContent::SetBlendState(VkPipelineColorBlendAttachmentState& desc, Pass* pass)
{
	desc.blendEnable = pass->BlendEnable;
	desc.srcColorBlendFactor = VK_GetAlphaBlend(pass->SrcBlend);		//源、目标颜色混合因子
	desc.dstColorBlendFactor = VK_GetAlphaBlend(pass->DstBlend);
	desc.colorBlendOp = VK_BLEND_OP_ADD;									//源、目标颜色的混合操作
	desc.srcAlphaBlendFactor = VK_GetAlphaBlend(pass->SrcAlphaSource);	//源、目标alpha值的混合因子
	desc.dstAlphaBlendFactor = VK_GetAlphaBlend(pass->DstAlphaSource);
	desc.alphaBlendOp = VK_BLEND_OP_ADD;
	//color mask
	DWORD color_mask = 0;
	if (pass->ColorMask.x)color_mask |= VK_COLOR_COMPONENT_R_BIT;
	if (pass->ColorMask.y)color_mask |= VK_COLOR_COMPONENT_G_BIT;
	if (pass->ColorMask.z)color_mask |= VK_COLOR_COMPONENT_B_BIT;
	if (pass->ColorMask.w)color_mask |= VK_COLOR_COMPONENT_A_BIT;
	desc.colorWriteMask = color_mask;
}
void VKRenderContent::SetDepthStencilTest(VkPipelineDepthStencilStateCreateInfo& desc, Pass* pass)
{
	desc.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	desc.depthTestEnable = false;// pass->DepthEnable;
	desc.depthWriteEnable = false;//pass->DepthWriteEnable;
	desc.depthCompareOp = VK_GetStencilCmp(pass->DepthCmpFun);
	desc.stencilTestEnable = pass->StencilEnable;
	desc.minDepthBounds = 0;
	desc.maxDepthBounds = 0;
	if (pass->StencilEnable)
	{
		// 对于front face 像素使用的模版操作操作，三种状况下的stencil操作
		desc.front.failOp = VK_GetStencilOp(pass->StencilFailOp);
		desc.front.depthFailOp = VK_GetStencilOp(pass->StencilDepthFailOp);
		desc.front.passOp = VK_GetStencilOp(pass->StencilPassOp);
		desc.front.compareOp = VK_GetStencilCmp(pass->StencilFunc);
		desc.front.reference = pass->StencilRef;
		desc.front.compareMask = pass->StencilReadMask;
		desc.front.writeMask = pass->StencilWriteMask;

		// 对于back face像素使用的模版操作模式.
		desc.back = desc.front;
	}
}
void VKRenderContent::SetShader(Vector<VkPipelineShaderStageCreateInfo>& shader_desc, Pass* pass)
{
	VKProgram* shader = dynamic_cast<VKProgram*>(pass->GetProgram());
	for (int i = int(ShaderType::Vertex); i < int(ShaderType::Max); ++i)
	{
		ShaderType type = ShaderType(i);
		VkShaderModule shader_code = shader->GetShaderModule(type);
		if (shader_code != VK_NULL_HANDLE)
		{
			VkPipelineShaderStageCreateInfo stage = {};
			stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			stage.module = shader_code;
			stage.stage = VK_ShaderStageEnum[int(type)];
			stage.pName = ShaderEnterEnum[int(type)];
			shader_desc.Add(stage);
		}
	}
}
void VKRenderContent::SetSubPrimitiveState(const SubPrimitive* sub_prim, Pass* pass)
{
	if (!sub_prim->Tex)return;
	CGProgram* shader = pass->GetProgram();
	shader->SetPassVariable(sub_prim->TexIndex, sub_prim->TexName, sub_prim->Tex);

	//TODO
}
void VKRenderContent::SetVertexDeclaration(VkVertexInputBindingDescription& binding_desc, Vector<VkVertexInputAttributeDescription>& attribute_descs, VkPipelineInputAssemblyStateCreateInfo& ia_desc, HardwareVertexBuffer* vertex_buffer, VertexSemantic input_semantic)
{
	binding_desc.binding = 0;
	binding_desc.stride = vertex_buffer->GetVertexSize(0);
	binding_desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	for (byte i = 0, j = 0; i < vertex_buffer->GetElementCount(0); ++i)
	{
		const VertexElement* element = vertex_buffer->GetElement(0, i);
		if (input_semantic & element->GetSemantic())
		{
			VkVertexInputAttributeDescription attribute_desc;
			attribute_desc.location = j;
			attribute_desc.binding = binding_desc.binding;
			attribute_desc.format = VK_GetVertexType(element->GetType());
			attribute_desc.offset = (UINT)element->GetOffset();
			attribute_descs.Add(attribute_desc);
			j++;
		}
	}

	ia_desc.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	ia_desc.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	ia_desc.primitiveRestartEnable = VK_FALSE;
}
DC_END_NAMESPACE