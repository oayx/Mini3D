#include "VKTempCommand.h"
#include "VKDevice.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
List<VkCommandPool> VKTempCommand::_commandPools;
VKTempCommand::VKTempCommand()
{
	VkDevice device = GetVKDevice()->GetDevice();

	_commandPool = SpawnCommandPool();

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = _commandPool;
	allocInfo.commandBufferCount = 1;
	vkAllocateCommandBuffers(device, &allocInfo, &_commandBuffer);

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	vkBeginCommandBuffer(_commandBuffer, &beginInfo);
}
VKTempCommand::~VKTempCommand()
{
	VkDevice device = GetVKDevice()->GetDevice();

	VkSubmitInfo end_info = {};
	end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	end_info.commandBufferCount = 1;
	end_info.pCommandBuffers = &_commandBuffer;
	VK_ERROR(vkEndCommandBuffer(_commandBuffer));
	VK_ERROR(vkQueueSubmit(GetVKDevice()->GetQueue(), 1, &end_info, VK_NULL_HANDLE));

	vkFreeCommandBuffers(device, _commandPool, 1, &_commandBuffer);
	DespawnCommandPool(_commandPool);
}
VkCommandPool VKTempCommand::SpawnCommandPool()
{
	if (_commandPools.IsEmpty())
	{
		VkDevice device = GetVKDevice()->GetDevice();
		VkAllocationCallbacks* allocator = GetVKDevice()->GetAllocator();
		uint queue_family = GetVKDevice()->GetQueueFamily();

		VkCommandPoolCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		info.queueFamilyIndex = queue_family;
		VkCommandPool commandPool = VK_NULL_HANDLE;
		VK_ERROR(vkCreateCommandPool(device, &info, allocator, &commandPool));
		return commandPool;
	}
	else
	{
		VkCommandPool commandPool = _commandPools.First();
		_commandPools.RemoveFirst();
		return commandPool;
	}
}
void VKTempCommand::DespawnCommandPool(VkCommandPool pool)
{
	_commandPools.AddLast(pool);
}
void VKTempCommand::Destroy()
{
	VkDevice device = GetVKDevice()->GetDevice();
	VkAllocationCallbacks* allocator = GetVKDevice()->GetAllocator();
	for (auto pool : _commandPools)
	{
		vkDestroyCommandPool(device, pool, allocator);
	}
	_commandPools.Clear();
}
DC_END_NAMESPACE