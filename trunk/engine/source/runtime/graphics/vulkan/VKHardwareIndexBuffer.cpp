#include "VKHardwareIndexBuffer.h"
#include "VKDevice.h"
#include "VKRenderContent.h"
#include "runtime/Application.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(VKHardwareIndexBuffer, HardwareIndexBuffer);
VKHardwareIndexBuffer::VKHardwareIndexBuffer()
{
}
VKHardwareIndexBuffer::~VKHardwareIndexBuffer()
{
	this->Release();
}
void* VKHardwareIndexBuffer::Lock(const IndexBufferDesc& desc)
{
	bool rebuild = this->NeedRebuild(desc);

	base::Lock(desc);
	if (_indexCount == 0)
		return nullptr;

	if (_indexBuffer == VK_NULL_HANDLE || rebuild)
	{
		this->Release();

		VkDevice device = GetVKDevice()->GetDevice();
		VkAllocationCallbacks* allocator = GetVKDevice()->GetAllocator();

		VkBufferCreateInfo buffer_info = {};
		buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		buffer_info.size = this->GetBufferSize();
		buffer_info.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		VK_ERROR(vkCreateBuffer(device, &buffer_info, allocator, &_indexBuffer));

		VkMemoryRequirements req;
		vkGetBufferMemoryRequirements(device, _indexBuffer, &req);
		VkMemoryAllocateInfo alloc_info = {};
		alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		alloc_info.allocationSize = req.size;
		alloc_info.memoryTypeIndex = VK_GetMemoryType(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, req.memoryTypeBits);
		VK_ERROR(vkAllocateMemory(device, &alloc_info, allocator, &_bufferMemory));

		VK_ERROR(vkBindBufferMemory(device, _indexBuffer, _bufferMemory, 0));
	}

	void* buffer;
	VK_ERROR(vkMapMemory(GetVKDevice()->GetDevice(), _bufferMemory, 0, this->GetBufferSize(), 0, &buffer));
	return buffer;
}
void  VKHardwareIndexBuffer::Unlock()
{
	base::Unlock();
	if (_indexCount == 0)
		return;
	vkUnmapMemory(GetVKDevice()->GetDevice(), _bufferMemory);
}
void VKHardwareIndexBuffer::Render()
{
	vkCmdBindIndexBuffer(GetVKDevice()->GetCommandBuffer(), _indexBuffer, 0, VK_GetIndexType(_indexType));
}
void VKHardwareIndexBuffer::Release()
{
	VkDevice device = GetVKDevice()->GetDevice();
	VkAllocationCallbacks* allocator = GetVKDevice()->GetAllocator();

	if (_indexBuffer != VK_NULL_HANDLE)
	{
		vkDestroyBuffer(device, _indexBuffer, allocator);
		_indexBuffer = VK_NULL_HANDLE;
	}
	if (_bufferMemory != VK_NULL_HANDLE)
	{
		vkFreeMemory(device, _bufferMemory, allocator);
		_bufferMemory = VK_NULL_HANDLE;
	}
}
DC_END_NAMESPACE