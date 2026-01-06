#include "VKTexture.h"
#include "VKDevice.h"
#include "VKTempCommand.h"
#include "core/image/Image.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(VKTexture, Texture);
VKTexture::VKTexture(const TextureDesc& desc)
	: base(desc)
{
	AssertEx(_textureType == TextureType::D2 || _textureType == TextureType::Cube, "");
	if (this->IsStaging())
	{
		AssertEx(!_enableMips, "");
		AssertEx(_textureType != TextureType::Cube, "");
	}

	CreateTexture();
	if (!this->IsStaging())
	{
		this->CreateTextureView();
		this->CreateTextureSampler();
	}
}
VKTexture::~VKTexture()
{
	VkDevice device = GetVKDevice()->GetDevice();
	VkAllocationCallbacks* allocator = GetVKDevice()->GetAllocator();
	if (_colorTexture)
	{
		vkDestroyImage(device, _colorTexture, allocator);
		_colorTexture = VK_NULL_HANDLE;
	}
	if (_colorTextureView)
	{
		vkDestroyImageView(device, _colorTextureView, allocator);
		_colorTextureView = VK_NULL_HANDLE;
	}
	if (_textureMemory)
	{
		vkFreeMemory(device, _textureMemory, allocator);
		_textureMemory = VK_NULL_HANDLE;
	}
}
bool VKTexture::Fill(Image* image)
{
	VkDevice device = GetVKDevice()->GetDevice();
	VkAllocationCallbacks* allocator = GetVKDevice()->GetAllocator();

	ImageMipData* mip_data = image->GetMipData(0);
	_totalBytes = mip_data->Size;
	const byte* pixels = mip_data->Data;
	
	VkBuffer upload_buffer = VK_NULL_HANDLE;
	VkDeviceMemory upload_buffer_memory = VK_NULL_HANDLE;
	// Create the Upload Buffer:
	{
		VkBufferCreateInfo buffer_info = {};
		buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		buffer_info.size = this->GetBytes();
		buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		VK_ERROR(vkCreateBuffer(device, &buffer_info, allocator, &upload_buffer));
		VkMemoryRequirements req;
		vkGetBufferMemoryRequirements(device, upload_buffer, &req);
		VkMemoryAllocateInfo alloc_info = {};
		alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		alloc_info.allocationSize = req.size;
		alloc_info.memoryTypeIndex = VK_GetMemoryType(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, req.memoryTypeBits);
		VK_ERROR(vkAllocateMemory(device, &alloc_info, allocator, &upload_buffer_memory));
		VK_ERROR(vkBindBufferMemory(device, upload_buffer, upload_buffer_memory, 0));
	}
	// Upload to Buffer:
	{
		char* map = NULL;
		VK_ERROR(vkMapMemory(device, upload_buffer_memory, 0, this->GetBytes(), 0, (void**)(&map)));
		Memory::Copy(map, pixels, this->GetBytes());
		VkMappedMemoryRange range[1] = {};
		range[0].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		range[0].memory = upload_buffer_memory;
		range[0].size = this->GetBytes();
		VK_ERROR(vkFlushMappedMemoryRanges(device, 1, range));
		vkUnmapMemory(device, upload_buffer_memory);
	}

	// Copy to Image:
	VKTempCommand command;
	{
		VkImageMemoryBarrier copy_barrier[1] = {};
		copy_barrier[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		copy_barrier[0].dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		copy_barrier[0].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		copy_barrier[0].newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		copy_barrier[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		copy_barrier[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		copy_barrier[0].image = _colorTexture;
		copy_barrier[0].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		copy_barrier[0].subresourceRange.levelCount = 1;
		copy_barrier[0].subresourceRange.layerCount = 1;
		vkCmdPipelineBarrier(command.GetCommandBuffer(), VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, copy_barrier);

		VkBufferImageCopy region = {};
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.layerCount = 1;
		region.imageExtent.width = _imageWidth;
		region.imageExtent.height = _imageHeight;
		region.imageExtent.depth = 1;
		vkCmdCopyBufferToImage(command.GetCommandBuffer(), upload_buffer, _colorTexture, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

		VkImageMemoryBarrier use_barrier[1] = {};
		use_barrier[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		use_barrier[0].srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		use_barrier[0].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		use_barrier[0].oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		use_barrier[0].newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		use_barrier[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		use_barrier[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		use_barrier[0].image = _colorTexture;
		use_barrier[0].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		use_barrier[0].subresourceRange.levelCount = 1;
		use_barrier[0].subresourceRange.layerCount = 1;
		vkCmdPipelineBarrier(command.GetCommandBuffer(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, use_barrier);
	}

	vkDestroyBuffer(device, upload_buffer, allocator);
	vkFreeMemory(device, upload_buffer_memory, allocator);
	return true;
}
byte* VKTexture::Lock(TextureLockDesc& lock_desc)
{
	if (_textureType != TextureType::D2 || !this->IsDynamic())
		return nullptr;

	lock_desc.pitch = this->GetPitch();
	if (_imageData == nullptr)
	{
		uint size = this->GetBytes();
		_imageData = Memory::NewArray<byte>(size);
		Memory::Clear(_imageData, size);
	}

	return _imageData;
}
void  VKTexture::Unlock(const TextureLockDesc& lock_desc)
{
	if (_textureType != TextureType::D2 || !this->IsDynamic())
		return;

	VkDevice device = GetVKDevice()->GetDevice();
	VkAllocationCallbacks* allocator = GetVKDevice()->GetAllocator();

	VkBuffer upload_buffer = VK_NULL_HANDLE;
	VkDeviceMemory upload_buffer_memory = VK_NULL_HANDLE;
	// Create the Upload Buffer:
	{
		VkBufferCreateInfo buffer_info = {};
		buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		buffer_info.size = this->GetBytes();
		buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		VK_ERROR(vkCreateBuffer(device, &buffer_info, allocator, &upload_buffer));
		VkMemoryRequirements req;
		vkGetBufferMemoryRequirements(device, upload_buffer, &req);
		VkMemoryAllocateInfo alloc_info = {};
		alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		alloc_info.allocationSize = req.size;
		alloc_info.memoryTypeIndex = VK_GetMemoryType(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, req.memoryTypeBits);
		VK_ERROR(vkAllocateMemory(device, &alloc_info, allocator, &upload_buffer_memory));
		VK_ERROR(vkBindBufferMemory(device, upload_buffer, upload_buffer_memory, 0));
	}
	// Upload to Buffer:
	{
		char* map = NULL;
		VK_ERROR(vkMapMemory(device, upload_buffer_memory, 0, this->GetBytes(), 0, (void**)(&map)));
		Memory::Copy(map, _imageData, this->GetBytes());
		VkMappedMemoryRange range[1] = {};
		range[0].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		range[0].memory = upload_buffer_memory;
		range[0].size = this->GetBytes();
		VK_ERROR(vkFlushMappedMemoryRanges(device, 1, range));
		vkUnmapMemory(device, upload_buffer_memory);
	}

	// Copy to Image:
	VKTempCommand command;
	{
		VkImageMemoryBarrier copy_barrier[1] = {};
		copy_barrier[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		copy_barrier[0].dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		copy_barrier[0].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		copy_barrier[0].newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		copy_barrier[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		copy_barrier[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		copy_barrier[0].image = _colorTexture;
		copy_barrier[0].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		copy_barrier[0].subresourceRange.levelCount = 1;
		copy_barrier[0].subresourceRange.layerCount = 1;
		vkCmdPipelineBarrier(command.GetCommandBuffer(), VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, copy_barrier);

		VkBufferImageCopy region = {};
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.layerCount = 1;
		region.imageExtent.width = _imageWidth;
		region.imageExtent.height = _imageHeight;
		region.imageExtent.depth = 1;
		vkCmdCopyBufferToImage(command.GetCommandBuffer(), upload_buffer, _colorTexture, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

		VkImageMemoryBarrier use_barrier[1] = {};
		use_barrier[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		use_barrier[0].srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		use_barrier[0].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		use_barrier[0].oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		use_barrier[0].newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		use_barrier[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		use_barrier[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		use_barrier[0].image = _colorTexture;
		use_barrier[0].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		use_barrier[0].subresourceRange.levelCount = 1;
		use_barrier[0].subresourceRange.layerCount = 1;
		vkCmdPipelineBarrier(command.GetCommandBuffer(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, use_barrier);
	}

	vkDestroyBuffer(device, upload_buffer, allocator);
	vkFreeMemory(device, upload_buffer_memory, allocator);
}
bool VKTexture::Copy(Texture* dst)
{
	AssertEx(dst->GetWidth() == this->GetWidth(), "");
	AssertEx(dst->GetHeight() == this->GetHeight(), "");
	AssertEx(dst->GetFormat() == this->GetFormat(), "");

	TextureLockDesc lock_desc;
	MemoryDataStream stream;
	if (this->GetData(stream))
	{
		void* bits = dst->Lock(lock_desc);
		if(bits)
		{
			Memory::Copy(bits, stream.Buffer(), stream.Size());
		}
		dst->Unlock(lock_desc);
	}

	return true;
}
void VKTexture::SaveToFile(const String& name, ImageType type)
{
	Image* image = Image::Create(_imageFormat, iSize(_imageWidth, _imageHeight));
	MemoryDataStream stream;
	if (this->GetData(stream) && image->GetSize() == stream.Size())
	{
		image->Fill(stream.Buffer(), stream.Size());
		image->SaveToFile(name);
	}
}
void VKTexture::CreateTexture()
{
	VkDevice device = GetVKDevice()->GetDevice();
	VkAllocationCallbacks* allocator = GetVKDevice()->GetAllocator();

	VkImageCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	info.imageType = VK_IMAGE_TYPE_2D;
	info.format = VK_FORMAT_R8G8B8A8_UNORM;
	info.extent.width = _imageWidth;
	info.extent.height = _imageHeight;
	info.extent.depth = 1;
	info.mipLevels = 1;
	info.arrayLayers = 1;
	info.samples = VK_SAMPLE_COUNT_1_BIT;
	info.tiling = VK_IMAGE_TILING_OPTIMAL;
	info.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	VK_ERROR(vkCreateImage(device, &info, allocator, &_colorTexture));

	VkMemoryRequirements req;
	vkGetImageMemoryRequirements(device, _colorTexture, &req);
	VkMemoryAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	alloc_info.allocationSize = req.size;
	alloc_info.memoryTypeIndex = VK_GetMemoryType(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, req.memoryTypeBits);
	VK_ERROR(vkAllocateMemory(device, &alloc_info, allocator, &_textureMemory));

	VK_ERROR(vkBindImageMemory(device, _colorTexture, _textureMemory, 0));
}
void VKTexture::CreateTextureView()
{
	VkDevice device = GetVKDevice()->GetDevice();
	VkAllocationCallbacks* allocator = GetVKDevice()->GetAllocator();

	VkImageViewCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	info.image = _colorTexture;
	info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	info.format = VK_FORMAT_R8G8B8A8_UNORM;
	info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	info.subresourceRange.levelCount = 1;
	info.subresourceRange.layerCount = 1;
	VK_ERROR(vkCreateImageView(device, &info, allocator, &_colorTextureView));
}
void VKTexture::CreateTextureSampler()
{
	VkDevice device = GetVKDevice()->GetDevice();
	VkAllocationCallbacks* allocator = GetVKDevice()->GetAllocator();

	VkSamplerCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	info.magFilter = VK_GetTextureFilter(this->GetFilterType());
	info.minFilter = VK_GetTextureFilter(this->GetFilterType());
	info.mipmapMode = VK_GetTextureMipmapFilter(this->GetFilterType());
	info.addressModeU = VK_GetTextureAddress(this->GetAddressMode());
	info.addressModeV = VK_GetTextureAddress(this->GetAddressMode());
	info.addressModeW = VK_GetTextureAddress(this->GetAddressMode());
	info.minLod = -1000;
	info.maxLod = 1000;
	info.maxAnisotropy = 1.0f;
	VK_ERROR(vkCreateSampler(device, &info, allocator, &_textureSampler));
}
DC_END_NAMESPACE