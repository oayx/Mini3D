
/*****************************************************************************
* Author： hannibal
* Description：
*****************************************************************************/
#pragma once

#include "runtime/graphics/null/GPUAdapter.h"
#include "VKDefine.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class VKGPUAdapterInfo final : public GPUAdapterInfo
{
	DEFAULT_CREATE(VKGPUAdapterInfo);
	FRIEND_CONSTRUCT_DESTRUCT(VKGPUAdapterInfo);
	BEGIN_DERIVED_REFECTION_TYPE(VKGPUAdapterInfo, GPUAdapterInfo)
	END_REFECTION_TYPE;

	using base::base;
	
private:
	VkPhysicalDevice _device = nullptr;
	VkPhysicalDeviceProperties _properties;
	uint _queueFamily = INVALID_INDEX;

public:
	void SetDevice(VkPhysicalDevice device)
	{
		_device = device;
		vkGetPhysicalDeviceProperties(device, &_properties);

		//选择队列族
		uint32_t count;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &count, NULL);
		Vector<VkQueueFamilyProperties> queues(count);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &count, queues.Data());
		for (uint32_t i = 0; i < count; i++)
		{
			if (queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				_queueFamily = i;
				break;
			}
		}
	}
	VkPhysicalDevice GetDevice()const { return _device; }

	uint GetQueueFamily()const { return _queueFamily; }

public:
	virtual bool IsValid() const override
	{
		return _index != INVALID_INDEX && _queueFamily != INVALID_INDEX;
	}
	virtual uint GetVendorId() const override
	{
		return _properties.vendorID;
	}
	virtual uint GetDeviceId() const override
	{
		return (uint)_properties.deviceID;
	}
	virtual String GetDescription() const override
	{
		return String(_properties.deviceName);
	}
};

/********************************************************************/
class VKGPUAdapter : public GPUAdapter
{
	DEFAULT_CREATE(VKGPUAdapter);
	FRIEND_CONSTRUCT_DESTRUCT(VKGPUAdapter);
	BEGIN_DERIVED_REFECTION_TYPE(VKGPUAdapter, GPUAdapter)
	END_REFECTION_TYPE;

public:
	virtual GPUAdapterInfo* SelectAdapters(void* instance)override;

private:
	void EnumAdapters(void* instance);
	bool IsDeviceSuitable(VKGPUAdapterInfo* adapter, bool need_discrete_gpu = true);
	bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
};
DC_END_NAMESPACE