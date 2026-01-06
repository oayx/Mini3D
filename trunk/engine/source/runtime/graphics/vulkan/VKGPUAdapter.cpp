#include "VKGPUAdapter.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(VKGPUAdapterInfo, GPUAdapterInfo);
IMPL_DERIVED_REFECTION_TYPE(VKGPUAdapter, GPUAdapter);
GPUAdapterInfo* VKGPUAdapter::SelectAdapters(void* instance)
{
	VkInstance vk_instance = (VkInstance)instance;

	if (_gpuAdapters.IsEmpty())
	{
		this->EnumAdapters(instance);
	}

	if (!_gpuAdapters.IsEmpty())
	{
		//先看看是否有独立显卡
		for (const auto& adapter : _gpuAdapters)
		{
			VKGPUAdapterInfo* vk_adapter = dynamic_cast<VKGPUAdapterInfo*>(adapter);
			if (IsDeviceSuitable(vk_adapter))
			{
				return adapter;
			}
		}

		//没有就用集成显卡
		for (const auto& adapter : _gpuAdapters)
		{
			VKGPUAdapterInfo* vk_adapter = dynamic_cast<VKGPUAdapterInfo*>(adapter);
			if (IsDeviceSuitable(vk_adapter, false))
			{
				return adapter;
			}
		}
	}

	return nullptr;
}
void VKGPUAdapter::EnumAdapters(void* instance)
{
	VkInstance vk_instance = (VkInstance)instance;

	uint32_t gpu_count;
	VK_ERROR(vkEnumeratePhysicalDevices(vk_instance, &gpu_count, NULL));
	AssertEx(gpu_count > 0, "");

	Vector<VkPhysicalDevice> devices(gpu_count);
	VK_ERROR(vkEnumeratePhysicalDevices(vk_instance, &gpu_count, devices.Data()));

	for (uint index = 0; index < devices.Size(); ++index)
	{
		VKGPUAdapterInfo* adapter = VKGPUAdapterInfo::Create(index);
		adapter->SetDevice(devices[index]);
		_gpuAdapters.Add(adapter);
	}
}
bool VKGPUAdapter::IsDeviceSuitable(VKGPUAdapterInfo* adapter, bool need_discrete_gpu)
{
	VkPhysicalDevice device = adapter->GetDevice();

	//设备属性像name, type以及Vulkan版本
	VkPhysicalDeviceProperties properties;
	vkGetPhysicalDeviceProperties(device, &properties);

	//查询对纹理压缩，64位浮点数和多视图渲染(VR非常有用)等可选功能的支持
	VkPhysicalDeviceFeatures features;
	vkGetPhysicalDeviceFeatures(device, &features);

	if (need_discrete_gpu && properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		return false;

	if (!adapter->IsValid())
		return false;

	if (!this->CheckDeviceExtensionSupport(device))
		return false;

	return true;
}
bool VKGPUAdapter::CheckDeviceExtensionSupport(VkPhysicalDevice device) 
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	//所有支持的扩展
	Vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.Data());

	//补充需要验证的扩展
	const Vector<String> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	Vector<String> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
	for (const auto& extension : availableExtensions) 
	{
		requiredExtensions.Remove(extension.extensionName);
	}

	return requiredExtensions.IsEmpty();
}
DC_END_NAMESPACE