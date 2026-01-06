#include "VKDevice.h"
#include "VKHardwareVertexBuffer.h"
#include "VKHardwareIndexBuffer.h"
#include "VKPostProcess.h"
#include "VKRenderContent.h"
#include "VKTexture.h"
#include "VKRenderTexture.h"
#include "VKProgram.h"
#include "VKCaps.h"
#include "VKGPUAdapter.h"
#include "runtime/Application.h"
#include "platform/common/glfw/GLFWRenderWindow.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(VKDevice, GraphicsDevice);
static VKAPI_ATTR VkBool32 VKAPI_CALL OnDebugReport(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData)
{
	(void)flags; (void)object; (void)location; (void)messageCode; (void)pUserData; (void)pLayerPrefix; // Unused arguments
	Debuger::Error("[vulkan] Debug report from ObjectType: %i\nMessage: %s\n\n", objectType, pMessage);
	return VK_FALSE;
}
VKDevice* GetVKDevice()
{
	return (VKDevice*)Application::GetGraphics();
}
VKDevice::VKDevice()
{
	_gpuAdapter = VKGPUAdapter::Create();
	_graphicsCaps = VKCaps::Create();
}
VKDevice::~VKDevice()
{
#ifdef DC_DEBUG
	if (_debugReport != VK_NULL_HANDLE)
	{
		auto vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(_instance, "vkDestroyDebugReportCallbackEXT");
		if (vkDestroyDebugReportCallbackEXT != nullptr)
		{
			vkDestroyDebugReportCallbackEXT(_instance, _debugReport, _allocator);
		}
		_debugReport = VK_NULL_HANDLE;
	}
#endif

	vkDestroySurfaceKHR(_instance, _surface, _allocator);
	vkDestroyDescriptorPool(_device, _descriptorPool, _allocator);
	vkDestroyDevice(_device, _allocator);
	vkDestroyInstance(_instance, _allocator);

	_allocator = nullptr;
	_instance = VK_NULL_HANDLE;
	_surface = VK_NULL_HANDLE;
	_device = VK_NULL_HANDLE;
	_descriptorPool = VK_NULL_HANDLE;
	_physicalDevice = VK_NULL_HANDLE;
	_queue = VK_NULL_HANDLE;
}
bool VKDevice::CreateDevice(RenderWindow* window)
{
	_graphicsCaps->Initialize();
	{// 创建instance
		VkApplicationInfo app_info = {};
		app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		app_info.pNext = NULL;
		app_info.pApplicationName = "";
		app_info.applicationVersion = 1;
		app_info.pEngineName = "";
		app_info.engineVersion = 1;
		app_info.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		create_info.pApplicationInfo = &app_info;

		Vector<const char *> extensions;
		extensions.Add(VK_KHR_SURFACE_EXTENSION_NAME);
#ifdef DC_PLATFORM_ANDROID
		extensions.Add(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#elif defined(DC_PLATFORM_WIN32)
		extensions.Add(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(DC_PLATFORM_MAC) || defined(DC_PLATFORM_IOS)
		extensions.Add(VK_EXT_METAL_SURFACE_EXTENSION_NAME);
#else
		extensions.Add(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#endif
		create_info.enabledExtensionCount = extensions.Size();
		create_info.ppEnabledExtensionNames = extensions.Data();

#ifdef DC_DEBUG1
		// 开启调试层
		const char* layers[] = { "VK_LAYER_LUNARG_standard_validation" };
		create_info.enabledLayerCount = 1;
		create_info.ppEnabledLayerNames = layers;

		extensions.Add("VK_EXT_debug_report");
		create_info.enabledExtensionCount = extensions.Size();
		create_info.ppEnabledExtensionNames = extensions.Data();

		// 创建
		VK_ERROR(vkCreateInstance(&create_info, _allocator, &_instance));

		auto vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(_instance, "vkCreateDebugReportCallbackEXT");
		CHECK_RETURN_PTR_FALSE(vkCreateDebugReportCallbackEXT);

		// 调试回调函数
		VkDebugReportCallbackCreateInfoEXT debug_report_ci = {};
		debug_report_ci.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
		debug_report_ci.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
		debug_report_ci.pfnCallback = OnDebugReport;
		debug_report_ci.pUserData = NULL;
		VK_ERROR(vkCreateDebugReportCallbackEXT(_instance, &debug_report_ci, _allocator, &_debugReport));
#else
		VK_ERROR(vkCreateInstance(&create_info, _allocator, &_instance));
#endif
	}

	{//选择物理设备
		GPUAdapterInfo* adapter = _gpuAdapter->SelectAdapters(_instance);
		CHECK_RETURN_PTR_FALSE(adapter);
		adapter->SetSelect();
		VKGPUAdapterInfo* vk_adapter = dynamic_cast<VKGPUAdapterInfo*>(adapter);
		_physicalDevice = vk_adapter->GetDevice();
		_queueFamily = vk_adapter->GetQueueFamily();
	}

	{//创建逻辑设备
		int device_extension_count = 1; 
		const char* device_extensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };//启用交换链扩展
		const float queue_priority[] = { 1.0f };
		VkDeviceQueueCreateInfo queue_info[1] = {};
		queue_info[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_info[0].queueFamilyIndex = _queueFamily;
		queue_info[0].queueCount = 1;
		queue_info[0].pQueuePriorities = queue_priority;
		VkPhysicalDeviceFeatures deviceFeatures = {};
		VkDeviceCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		create_info.queueCreateInfoCount = sizeof(queue_info) / sizeof(queue_info[0]);
		create_info.pQueueCreateInfos = queue_info;
		create_info.enabledExtensionCount = device_extension_count;
		create_info.ppEnabledExtensionNames = device_extensions;
		create_info.pEnabledFeatures = &deviceFeatures;
		VK_ERROR(vkCreateDevice(_physicalDevice, &create_info, _allocator, &_device));
		vkGetDeviceQueue(_device, _queueFamily, 0, &_queue);
	}

	{// 描述符池:用于分配描写集
		VkDescriptorPoolSize pool_sizes[] =
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};
		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000 * ARRAY_SIZE(pool_sizes);
		pool_info.poolSizeCount = (uint)ARRAY_SIZE(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;
		VK_ERROR(vkCreateDescriptorPool(_device, &pool_info, _allocator, &_descriptorPool));
	}

	{//surface
		GLFWRenderWindow* glfw_win = dynamic_cast<GLFWRenderWindow*>(window);
		VK_ERROR(glfwCreateWindowSurface(_instance, glfw_win->GetGLFWwindow(), _allocator, &_surface));
	}

	return true;
}
bool VKDevice::CreateSwapChain(RenderWindow* window)
{
	CHECK_RETURN_PTR_FALSE(window);
	VKSwapChain* swap_chain = VKSwapChain::Create();
	swap_chain->CreateSwapChain(window);
	_swapChains[int(window->GetTargetDisplay())] = swap_chain;
	return true;
}
void VKDevice::CreateRenderContent()
{
	RenderContent* content = VKRenderContent::Create();
	content->Initialize();
	_renderContent = content;
}
void VKDevice::Resize(const WindowResizeDesc& desc)
{
	base::Resize(desc);
}
HardwareVertexBuffer* VKDevice::CreateVertexBuffer()
{
	return VKHardwareVertexBuffer::Create();
}
HardwareIndexBuffer* VKDevice::CreateIndexBuffer()
{
	return VKHardwareIndexBuffer::Create();
}
CGProgram* VKDevice::CreateShader()
{
	return VKProgram::Create();
}
Texture* VKDevice::CreateTexture(const TextureDesc& desc)
{
	return VKTexture::Create(desc);
}
RenderTexture* VKDevice::CreateRenderTexture(const TextureDesc& desc)
{
	return VKRenderTexture::Create(desc);
}
FinalProcess* VKDevice::CreateFinalProcess()
{
	return VKFinalProcess::Create();
}
VkCommandBuffer VKDevice::GetCommandBuffer()const
{
	VKRenderContent* content = dynamic_cast<VKRenderContent*>(_renderContent);
	return content->GetCommandBuffer();
}
DC_END_NAMESPACE