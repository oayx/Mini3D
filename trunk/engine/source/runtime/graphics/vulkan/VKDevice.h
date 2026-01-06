 
/*****************************************************************************
* Author： hannibal
* Date：2020/8/18
* Description：设备
*****************************************************************************/
#pragma once

#include "runtime/graphics/null/GraphicsDevice.h"
#include "VKDefine.h"
#include "VKSwapChain.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class VKDevice final : public GraphicsDevice
{
	friend class Application;
	DEFAULT_CREATE(VKDevice);
	FRIEND_CONSTRUCT_DESTRUCT(VKDevice);
	BEGIN_DERIVED_REFECTION_TYPE(VKDevice, GraphicsDevice)
	END_REFECTION_TYPE;

protected:
	VKDevice();
	~VKDevice();

public:
	virtual bool			CreateDevice(RenderWindow* window)override;
	virtual bool			CreateSwapChain(RenderWindow* window)override;
	virtual void			CreateRenderContent()override;
	virtual void			Resize(const WindowResizeDesc& desc)override;

	virtual CGProgram*			CreateShader()override;
	virtual Texture*		CreateTexture(const TextureDesc& desc)override;
	virtual RenderTexture*	CreateRenderTexture(const TextureDesc& desc)override;
	virtual FinalProcess*	CreateFinalProcess()override;
	virtual HardwareVertexBuffer* CreateVertexBuffer()override;
	virtual HardwareIndexBuffer* CreateIndexBuffer()override;

public:
	VkAllocationCallbacks*	GetAllocator()const { return _allocator; }
	VkInstance				GetInstance()const { return _instance; }
	VkSurfaceKHR			GetSurface()const { return _surface; }
	VkDevice                GetDevice()const { return _device; }
	VkDescriptorPool        GetDescriptorPool()const { return _descriptorPool; }
	VkPhysicalDevice		GetPhysicalDevice()const { return _physicalDevice; }
	VkQueue                 GetQueue()const { return _queue; }
	uint					GetQueueFamily()const { return _queueFamily; }
	VkCommandBuffer			GetCommandBuffer()const;

private:
	VkAllocationCallbacks*	_allocator = nullptr;
	VkInstance				_instance = VK_NULL_HANDLE;
	VkSurfaceKHR			_surface = VK_NULL_HANDLE;	//surface就是Vulkan与窗体系统的连接桥梁
	VkDevice                _device = VK_NULL_HANDLE;
	VkDescriptorPool        _descriptorPool = VK_NULL_HANDLE;
	VkPhysicalDevice		_physicalDevice = VK_NULL_HANDLE;
	VkQueue                 _queue = VK_NULL_HANDLE;
	uint					_queueFamily = INVALID_INDEX;

	VkDebugReportCallbackEXT _debugReport = VK_NULL_HANDLE;

};
DC_END_NAMESPACE