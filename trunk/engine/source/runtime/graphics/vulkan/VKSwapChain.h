
/*****************************************************************************
* Author： hannibal
* Description：
*****************************************************************************/
#pragma once

#include "runtime/graphics/null/SwapChain.h"
#include "VKDefine.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class VKSwapChain final : public SwapChain
{
	friend class VKDevice;
	DEFAULT_CREATE(VKSwapChain);
	FRIEND_CONSTRUCT_DESTRUCT(VKSwapChain);
	DISALLOW_COPY_ASSIGN(VKSwapChain);
	BEGIN_DERIVED_REFECTION_TYPE(VKSwapChain, SwapChain)
	END_REFECTION_TYPE;

	VKSwapChain();
	~VKSwapChain();

public:
	virtual bool CreateSwapChain(RenderWindow* window)override;
	virtual void Resize(const WindowResizeDesc& desc)override;
	virtual void BeginFrame(RenderFrameDesc& desc)override;
	virtual void Present(uint sync)override;

	virtual bool Copy(Texture* texture)const override { return false; }

public:
	VkSwapchainKHR		GetSwapchain()const { return _swapchain; }
	VkRenderPass        GetRenderPass()const { return _renderPass; }
	VkImage				GetBackbuffer(int index)const { return _backbuffer[index]; }
	VkImageView         GetBackbufferView(int index)const { return _backbufferView[index]; }
	VkFramebuffer       GetFramebuffer(int index)const { return _framebuffer[index]; }
	uint				GetBufferCount()const { return _backBufferCount; }

private:
	VkSurfaceFormatKHR	SelectSurfaceFormat(VkPhysicalDevice physical_device, VkSurfaceKHR surface, const VkFormat* request_formats, int request_formats_count, VkColorSpaceKHR request_color_space);
	VkPresentModeKHR	SelectPresentMode(VkPhysicalDevice physical_device, VkSurfaceKHR surface, const Vector<VkPresentModeKHR>&& present_modes);
	int					GetMinImageCountFromPresentMode(VkPresentModeKHR present_mode);

private:
	VkSwapchainKHR		_swapchain = VK_NULL_HANDLE;
	VkRenderPass        _renderPass = VK_NULL_HANDLE;

	uint				_backBufferCount = 0;
	VkImage             _backbuffer[VKBackBufferCount] = { VK_NULL_HANDLE };
	VkImageView         _backbufferView[VKBackBufferCount] = { VK_NULL_HANDLE };
	VkFramebuffer       _framebuffer[VKBackBufferCount] = { VK_NULL_HANDLE };

	VkSurfaceFormatKHR	_surfaceFormat;
	VkPresentModeKHR	_presentMode = VK_PRESENT_MODE_FIFO_KHR;
};
DC_END_NAMESPACE