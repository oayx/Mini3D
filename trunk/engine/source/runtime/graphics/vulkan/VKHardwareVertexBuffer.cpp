#include "VKHardwareVertexBuffer.h"
#include "VKDevice.h"
#include "VKRenderContent.h"
#include "runtime/Application.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(VKHardwareVertexBuffer, HardwareVertexBuffer);
VKHardwareVertexBuffer::VKHardwareVertexBuffer()
{
}
VKHardwareVertexBuffer::~VKHardwareVertexBuffer()
{
	this->Release();
}
void* VKHardwareVertexBuffer::Lock(const VertexBufferDesc& desc)
{
	bool rebuild = this->NeedRebuild(desc);

	base::Lock(desc);
	if (desc.num_vertex == 0 || desc.stream > 0)
		return nullptr;

	if (_vertexBuffer == VK_NULL_HANDLE || rebuild)
	{
		this->Release();

		VkDevice device = GetVKDevice()->GetDevice();
		VkAllocationCallbacks* allocator = GetVKDevice()->GetAllocator();

		VkBufferCreateInfo buffer_info = {};
		buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		buffer_info.size = this->GetBufferSize(desc.stream);
		buffer_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		VK_ERROR(vkCreateBuffer(device, &buffer_info, allocator, &_vertexBuffer));

		VkMemoryRequirements req;
		vkGetBufferMemoryRequirements(device, _vertexBuffer, &req);
		VkMemoryAllocateInfo alloc_info = {};
		alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		alloc_info.allocationSize = req.size;
		alloc_info.memoryTypeIndex = VK_GetMemoryType(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, req.memoryTypeBits);
		VK_ERROR(vkAllocateMemory(device, &alloc_info, allocator, &_bufferMemory));

		VK_ERROR(vkBindBufferMemory(device, _vertexBuffer, _bufferMemory, 0));
	}

	void* buffer;
	VK_ERROR(vkMapMemory(GetVKDevice()->GetDevice(), _bufferMemory, 0, this->GetBufferSize(desc.stream), 0, &buffer));
	return buffer;
}
void  VKHardwareVertexBuffer::Unlock(const VertexBufferDesc& desc)
{
	base::Unlock(desc);
	if (desc.num_vertex == 0 || desc.stream > 0)
		return;
	
	vkUnmapMemory(GetVKDevice()->GetDevice(), _bufferMemory);
}
void VKHardwareVertexBuffer::Render(CGProgram* shader)
{
	DC_PROFILE_FUNCTION;
	VkDeviceSize vertex_offset[1] = { 0 };
	vkCmdBindVertexBuffers(GetVKDevice()->GetCommandBuffer(), 0, 1, &_vertexBuffer, vertex_offset);
}
void VKHardwareVertexBuffer::Release()
{
	VkDevice device = GetVKDevice()->GetDevice();
	VkAllocationCallbacks* allocator = GetVKDevice()->GetAllocator();

	if (_vertexBuffer != VK_NULL_HANDLE)
	{
		vkDestroyBuffer(device, _vertexBuffer, allocator);
		_vertexBuffer = VK_NULL_HANDLE;
	}
	if (_bufferMemory != VK_NULL_HANDLE)
	{
		vkFreeMemory(device, _bufferMemory, allocator);
		_bufferMemory = VK_NULL_HANDLE;
	}
}
DC_END_NAMESPACE