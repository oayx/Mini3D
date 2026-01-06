#include "VKSwapChain.h"
#include "VKDevice.h"
#include "runtime/Application.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(VKSwapChain, SwapChain);
VKSwapChain::VKSwapChain()
{

}
VKSwapChain::~VKSwapChain()
{
	VkDevice device = GetVKDevice()->GetDevice();
	VkAllocationCallbacks* allocator = GetVKDevice()->GetAllocator();

	vkDestroySwapchainKHR(device, _swapchain, allocator);
	vkDestroyRenderPass(device, _renderPass, allocator);
	_renderPass = VK_NULL_HANDLE;
	_swapchain = VK_NULL_HANDLE;

	for (int i = 0; i < VKBackBufferCount; ++i)
	{
		vkDestroyImageView(device, _backbufferView[i], allocator);
		vkDestroyFramebuffer(device, _framebuffer[i], allocator);
		_backbuffer[i] = VK_NULL_HANDLE;
		_backbufferView[i] = VK_NULL_HANDLE;
		_framebuffer[i] = VK_NULL_HANDLE;
	}
}
bool VKSwapChain::CreateSwapChain(RenderWindow* window)
{
	VkPhysicalDevice physical_device = GetVKDevice()->GetPhysicalDevice();
	VkSurfaceKHR surface = GetVKDevice()->GetSurface();
	uint queue_family = GetVKDevice()->GetQueueFamily();

	// Check for WSI support
	VkBool32 res;
	vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, queue_family, surface, &res);
	CHECK_RETURN_PTR_FALSE(res == VK_TRUE);

	// Select Surface Format
	const VkFormat request_formats[] = { VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM };
	const VkColorSpaceKHR request_color_space = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
	_surfaceFormat = this->SelectSurfaceFormat(physical_device, surface, request_formats, ARRAY_SIZE(request_formats), request_color_space);

	// Select Present Mode
	Vector<VkPresentModeKHR> present_modes;
	if (window->GetSync() == 0)
	{//不开启垂直同步
		present_modes.Add(VK_PRESENT_MODE_MAILBOX_KHR);
		present_modes.Add(VK_PRESENT_MODE_IMMEDIATE_KHR);
	}
	else
	{
		present_modes.Add(VK_PRESENT_MODE_FIFO_KHR);
	}
	_presentMode = this->SelectPresentMode(physical_device, surface, std::move(present_modes));

	//创建
	WindowResizeDesc desc = {};
	desc.hWnd = window->GetHwnd();
	desc.width = (int)window->GetWidth();
	desc.height = (int)window->GetHeight();
	desc.fullscreen = window->IsFullscreen();
	this->Resize(desc);

	return base::CreateSwapChain(window);
}
void VKSwapChain::Resize(const WindowResizeDesc& desc)
{
	VkDevice device = GetVKDevice()->GetDevice();
	VkAllocationCallbacks* allocator = GetVKDevice()->GetAllocator();
	VkPhysicalDevice physical_device = GetVKDevice()->GetPhysicalDevice();
	VkSurfaceKHR surface = GetVKDevice()->GetSurface();
	uint queue_family = GetVKDevice()->GetQueueFamily();

	VkSwapchainKHR old_swapchain = _swapchain;
	_swapchain = VK_NULL_HANDLE;
	VK_ERROR(vkDeviceWaitIdle(device));

	// We don't use ImGui_ImplVulkanH_DestroyWindow() because we want to preserve the old swapchain to create the new one.
	// Destroy old Framebuffer
	for (uint i = 0; i < _backBufferCount; i++)
	{
		vkDestroyImageView(device, _backbufferView[i], allocator);
		vkDestroyFramebuffer(device, _framebuffer[i], allocator);
	}
	if (_renderPass)
	{
		vkDestroyRenderPass(device, _renderPass, allocator);
	}

	// If min image count was not specified, request different count of images dependent on selected present mode
	_backBufferCount = this->GetMinImageCountFromPresentMode(_presentMode);

	// Create Swapchain
	{
		VkSwapchainCreateInfoKHR info = {};
		info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		info.surface = surface;
		info.minImageCount = _backBufferCount;
		info.imageFormat = _surfaceFormat.format;
		info.imageColorSpace = _surfaceFormat.colorSpace;
		info.imageArrayLayers = 1;
		info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;           // Assume that graphics family == present family
		info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		info.presentMode = _presentMode;
		info.clipped = VK_TRUE;
		info.oldSwapchain = old_swapchain;
		VkSurfaceCapabilitiesKHR cap;
		VK_ERROR(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &cap));
		if (info.minImageCount < cap.minImageCount)
			info.minImageCount = cap.minImageCount;
		else if (cap.maxImageCount != 0 && info.minImageCount > cap.maxImageCount)
			info.minImageCount = cap.maxImageCount;

		info.imageExtent.width = desc.width;
		info.imageExtent.height = desc.height;
		VK_ERROR(vkCreateSwapchainKHR(device, &info, allocator, &_swapchain));
		VK_ERROR(vkGetSwapchainImagesKHR(device, _swapchain, &_backBufferCount, NULL));
		VkImage backbuffers[16] = {};
		VK_ERROR(vkGetSwapchainImagesKHR(device, _swapchain, &_backBufferCount, backbuffers));

		for (uint i = 0; i < _backBufferCount; i++)
			_backbuffer[i] = backbuffers[i];
	}
	if (old_swapchain)
		vkDestroySwapchainKHR(device, old_swapchain, allocator);

	// Create the Render Pass
	{
		VkAttachmentDescription attachment = {};
		attachment.format = _surfaceFormat.format;
		attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		VkAttachmentReference color_attachment = {};
		color_attachment.attachment = 0;
		color_attachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &color_attachment;
		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		VkRenderPassCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		info.attachmentCount = 1;
		info.pAttachments = &attachment;
		info.subpassCount = 1;
		info.pSubpasses = &subpass;
		info.dependencyCount = 1;
		info.pDependencies = &dependency;
		VK_ERROR(vkCreateRenderPass(device, &info, allocator, &_renderPass));
	}

	// Create The Image Views
	{
		VkImageViewCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		info.format = _surfaceFormat.format;
		info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		VkImageSubresourceRange image_range = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		info.subresourceRange = image_range;
		for (uint i = 0; i < _backBufferCount; i++)
		{
			info.image = _backbuffer[i];
			VK_ERROR(vkCreateImageView(device, &info, allocator, &_backbufferView[i]));
		}
	}

	// Create Framebuffer
	{
		VkImageView attachment[1];
		VkFramebufferCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		info.renderPass = _renderPass;
		info.attachmentCount = 1;
		info.pAttachments = attachment;
		info.width = desc.width;
		info.height = desc.height;
		info.layers = 1;
		for (uint i = 0; i < _backBufferCount; i++)
		{
			attachment[0] = _backbufferView[i];
			VK_ERROR(vkCreateFramebuffer(device, &info, allocator, &_framebuffer[i]));
		}
	}

	base::Resize(desc);
}
void VKSwapChain::BeginFrame(RenderFrameDesc& desc)
{
	base::BeginFrame(desc);
}
void VKSwapChain::Present(uint sync)
{
}
VkSurfaceFormatKHR VKSwapChain::SelectSurfaceFormat(VkPhysicalDevice physical_device, VkSurfaceKHR surface, const VkFormat* request_formats, int request_formats_count, VkColorSpaceKHR request_color_space)
{
	AssertEx(request_formats != nullptr, "");
	AssertEx(request_formats_count > 0, "");

	uint avail_count;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &avail_count, NULL);
	Vector<VkSurfaceFormatKHR> avail_format(avail_count);
	vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &avail_count, avail_format.Data());

	// First check if only one format, VK_FORMAT_UNDEFINED, is available, which would imply that any format is available
	if (avail_count == 1)
	{
		if (avail_format[0].format == VK_FORMAT_UNDEFINED)
		{
			VkSurfaceFormatKHR ret;
			ret.format = request_formats[0];
			ret.colorSpace = request_color_space;
			return ret;
		}
		else
		{
			// No point in searching another format
			return avail_format[0];
		}
	}
	else
	{
		// Request several formats, the first found will be used
		for (int request_i = 0; request_i < request_formats_count; request_i++)
			for (uint avail_i = 0; avail_i < avail_count; avail_i++)
				if (avail_format[avail_i].format == request_formats[request_i] && avail_format[avail_i].colorSpace == request_color_space)
					return avail_format[avail_i];

		// If none of the requested image formats could be found, use the first available
		return avail_format[0];
	}
}
/*
VK_PRESENT_MODE_IMMEDIATE_KHR: 应用程序提交的图像被立即传输到屏幕呈现，这种模式可能会造成撕裂效果。
VK_PRESENT_MODE_FIFO_KHR: 交换链被看作一个队列，当显示内容需要刷新的时候，显示设备从队列的前面获取图像，并且程序将渲染完成的图像插入队列的后面。如果队列是满的程序会等待。这种规模与视频游戏的垂直同步很类似。显示设备的刷新时刻被成为“垂直中断”。
VK_PRESENT_MODE_FIFO_RELAXED_KHR: 该模式与上一个模式略有不同的地方为，如果应用程序存在延迟，即接受最后一个垂直同步信号时队列空了，将不会等待下一个垂直同步信号，而是将图像直接传送。这样做可能导致可见的撕裂效果。
VK_PRESENT_MODE_MAILBOX_KHR: 这是第二种模式的变种。当交换链队列满的时候，选择新的替换旧的图像，从而替代阻塞应用程序的情形。这种模式通常用来实现三重缓冲区，与标准的垂直同步双缓冲相比，它可以有效避免延迟带来的撕裂效果
*/
VkPresentModeKHR VKSwapChain::SelectPresentMode(VkPhysicalDevice physical_device, VkSurfaceKHR surface, const Vector<VkPresentModeKHR>&& present_modes)
{
	AssertEx(!present_modes.IsEmpty(), "");

	// Request a certain mode and confirm that it is available. If not use VK_PRESENT_MODE_FIFO_KHR which is mandatory
	uint avail_count = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &avail_count, NULL);
	Vector<VkPresentModeKHR> avail_modes(avail_count);
	vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &avail_count, avail_modes.Data());

	for (uint request_i = 0; request_i < present_modes.Size(); request_i++)
		for (uint avail_i = 0; avail_i < avail_count; avail_i++)
			if (present_modes[request_i] == avail_modes[avail_i])
				return present_modes[request_i];

	return VK_PRESENT_MODE_FIFO_KHR; // Always available
}
int VKSwapChain::GetMinImageCountFromPresentMode(VkPresentModeKHR present_mode)
{
	if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
		return 3;
	if (present_mode == VK_PRESENT_MODE_FIFO_KHR || present_mode == VK_PRESENT_MODE_FIFO_RELAXED_KHR)
		return 2;
	if (present_mode == VK_PRESENT_MODE_IMMEDIATE_KHR)
		return 1;
	return 1;
}
DC_END_NAMESPACE