#include "VKDefine.h"
#include "VKDevice.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
uint VK_GetMemoryType(VkMemoryPropertyFlags properties, uint type_bits)
{
	VkPhysicalDevice physical_device = GetVKDevice()->GetPhysicalDevice();
	VkPhysicalDeviceMemoryProperties prop;
	vkGetPhysicalDeviceMemoryProperties(physical_device, &prop);
	for (uint i = 0; i < prop.memoryTypeCount; i++)
		if ((prop.memoryTypes[i].propertyFlags & properties) == properties && type_bits & (1 << i))
			return i;
	return 0xFFFFFFFF;
}
DC_END_NAMESPACE