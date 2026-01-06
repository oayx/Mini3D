#include "VKProgram.h"
#include "VKDevice.h"
#include "VKRenderContent.h"
#include "core/image/Image.h"
#include "core/stream/DataStream.h"
#include "runtime/graphics/Pass.h"
#include "runtime/graphics/TextureUnit.h"
#include "runtime/graphics/null/Texture.h"
#include "runtime/graphics/null/RenderTexture.h"
#include "runtime/Application.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
VKCBuffer::VKCBuffer(const sShaderReflectCBuffer& cbuffer)
	: info(cbuffer)
{
	VkDevice device = GetVKDevice()->GetDevice();
	VkAllocationCallbacks* allocator = GetVKDevice()->GetAllocator();

	VkBufferCreateInfo buffer_info = {};
	buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer_info.size = info.size;
	buffer_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	VK_ERROR(vkCreateBuffer(device, &buffer_info, allocator, &uniformBuffer));

	VkMemoryRequirements req;
	vkGetBufferMemoryRequirements(device, uniformBuffer, &req);

	VkMemoryAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	alloc_info.allocationSize = req.size;
	alloc_info.memoryTypeIndex = VK_GetMemoryType(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, req.memoryTypeBits);
	VK_ERROR(vkAllocateMemory(device, &alloc_info, allocator, &uniformBufferMemory));
	VK_ERROR(vkBindBufferMemory(device, uniformBuffer, uniformBufferMemory, 0));
}
VKCBuffer::~VKCBuffer()
{
	VkDevice device = GetVKDevice()->GetDevice();
	VkAllocationCallbacks* allocator = GetVKDevice()->GetAllocator();

	vkDestroyBuffer(device, uniformBuffer, allocator);
	vkFreeMemory(device, uniformBufferMemory, allocator);
}
void VKCBuffer::PreUpdate()
{
	vkMapMemory(GetVKDevice()->GetDevice(), uniformBufferMemory, 0, info.size, 0, (void**)(&buffer));
}
void VKCBuffer::PostUpdate()
{
	vkUnmapMemory(GetVKDevice()->GetDevice(), uniformBufferMemory);
}
void VKCBuffer::Copy(uint offset, const void* src_data, uint size)
{
	::memcpy(buffer + offset, src_data, size);
	dirty = true;
}
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(VKProgram, CGProgram);
VKProgram::~VKProgram()
{
	VkDevice device = GetVKDevice()->GetDevice();
	VkAllocationCallbacks* allocator = GetVKDevice()->GetAllocator();

	for (auto obj : _cbBuffers)
	{
		SAFE_DELETE(obj.second);
	}
	_cbBuffers.Clear();

	for(auto layout : _descriptorSetLayout)
	{
		vkDestroyDescriptorSetLayout(device, layout, nullptr);
	}
	_descriptorSetLayout.Clear();
	_descriptorSet.Clear();
}
bool VKProgram::LoadFromDesc(const ShaderDesc& info)
{
	base::LoadFromDesc(info);

	VkDevice device = GetVKDevice()->GetDevice();
	VkAllocationCallbacks* allocator = GetVKDevice()->GetAllocator();

	//获得编译宏
	String defines = "";
	for (const auto& define : info.ShaderDefines)
	{
		defines += "_" + define;
	}

	String shader_version = Application::GetGraphics()->GetShaderVersion();
	if (!shader_version.IsEmpty())shader_version = "_" + shader_version;
	const String& language_name = ShadingLanguageFileName();
	for (int i = 0; i < int(ShaderType::Max); ++i)
	{
		if (info.ShaderFile[i].IsEmpty())continue;

		String dir_path = Path::GetDirectoryName(info.ShaderFile[i]);
		String file_name = Path::GetFileNameWithoutExtension(info.ShaderFile[i]);
		String file_path = Resource::GetFullDataPath(dir_path + "/" + file_name + "_" + language_name + shader_version + defines + ShaderFileExtEnum[i]);

		MemoryDataStream stream;
		if(!File::ReadAllBytes(file_path, stream))continue;

		VkShaderModuleCreateInfo vert_info = {};
		vert_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		vert_info.codeSize = stream.Size();
		vert_info.pCode = (uint*)stream.Buffer();
		VK_ERROR(vkCreateShaderModule(device, &vert_info, allocator, &_shaderModule[i]));
	}

	this->CreateCBuffer();

	return true;
}
bool VKProgram::LoadFromMemory(const String& name, const VecString& codes, const VecString& defines)
{
	base::LoadFromMemory(name, codes, defines);
	AssertEx(false, "");
	return false;
}
void VKProgram::PreRender()
{
	for (const auto& cb : _cbBuffers)
	{
		auto cbuffer = cb.second;
		cbuffer->PreUpdate();
	}
}
void VKProgram::Render(Camera* camera, Pass* pass, const Matrix4& obj_mat)
{
	if (_descriptorSetLayout.IsEmpty())
	{
		this->CreateDescriptorSetLayout(pass);
	}
	if (_pipelineLayout == VK_NULL_HANDLE)
	{
		this->CreatePipelineLayout();
	}
	if (_descriptorSet.IsEmpty())
	{
		this->CreateDescriptorSet(pass);
	}
	base::Render(camera, pass, obj_mat);
}
void VKProgram::PostRender()
{
	for (const auto& cb : _cbBuffers)
	{
		auto cbuffer = cb.second;
		cbuffer->PostUpdate();
	}
	if (!_descriptorSet.IsEmpty())
	{
		vkCmdBindDescriptorSets(GetVKDevice()->GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelineLayout, 0, _descriptorSet.Size(), _descriptorSet.CData(), 0, NULL);
	}
}
bool VKProgram::SetVariable(const String& name, const Matrix4& mat)
{
	const sShaderReflectVariable* cb_var = GetCBVariable(name);
	if (cb_var != nullptr)
	{
		cbuffers_t::iterator it = _cbBuffers.Find(cb_var->cb_name);
		if (it != _cbBuffers.end())
		{
			it->second->Copy(cb_var->offset, mat.Transpose().p, cb_var->size);
		}
		return true;
	}
	return false;
}
bool VKProgram::SetVariable(const String& name, const Matrix4* mats, int count)
{
	const sShaderReflectVariable* cb_var = GetCBVariable(name);
	if (cb_var != nullptr)
	{
		cbuffers_t::iterator it = _cbBuffers.Find(cb_var->cb_name);
		if (it != _cbBuffers.end())
		{
			Vector<Matrix4> t_mat(count);
			for (int i = 0; i < count; ++i)
			{
				t_mat[i] = mats->Transpose();
				mats += 1;
			}
			it->second->Copy(cb_var->offset, t_mat[0].p, sizeof(Matrix4) * count);
		}
		return true;
	}
	return false;
}
bool VKProgram::SetVariable(const String& name, const float* values, int count)
{
	const sShaderReflectVariable* cb_var = GetCBVariable(name);
	if (cb_var != nullptr)
	{
		cbuffers_t::iterator it = _cbBuffers.Find(cb_var->cb_name);
		if (it != _cbBuffers.end())
		{
			it->second->Copy(cb_var->offset, values, sizeof(float) * count);
		}
		return true;
	}
	return false;
}
bool VKProgram::SetVariable(const String& name, const Vector4* values, int count)
{
	const sShaderReflectVariable* cb_var = GetCBVariable(name);
	if (cb_var != nullptr)
	{
		cbuffers_t::iterator it = _cbBuffers.Find(cb_var->cb_name);
		if (it != _cbBuffers.end())
		{
			it->second->Copy(cb_var->offset, values, sizeof(Vector4) * count);
		}
		return true;
	}
	return false;
}
bool VKProgram::SetVariable(const String& name, const int* ints, int count)
{
	const sShaderReflectVariable* cb_var = GetCBVariable(name);
	if (cb_var != nullptr)
	{
		cbuffers_t::iterator it = _cbBuffers.Find(cb_var->cb_name);
		if (it != _cbBuffers.end())
		{
			it->second->Copy(cb_var->offset, ints, sizeof(int) * count);
		}
		return true;
	}
	return false;
}
bool VKProgram::SetVariable(const String& name, const bool bools)
{
	const sShaderReflectVariable* cb_var = GetCBVariable(name);
	if (cb_var != nullptr)
	{
		cbuffers_t::iterator it = _cbBuffers.Find(cb_var->cb_name);
		if (it != _cbBuffers.end())
		{
			it->second->Copy(cb_var->offset, &bools, cb_var->size);
		}
		return true;
	}
	return false;
}
bool VKProgram::SetVariable(const String& name, const ShaderLight& light, int index)
{
	const sShaderReflectVariable* cb_var = GetCBVariable(name);
	if (cb_var != nullptr)
	{
		cbuffers_t::iterator it = _cbBuffers.Find(cb_var->cb_name);
		if (it != _cbBuffers.end())
		{
			int offset = index * sizeof(ShaderLight);
			it->second->Copy(cb_var->offset + offset, &light, sizeof(ShaderLight));
		}
		return true;
	}
	return false;
}
bool VKProgram::SetVariable(byte index, TextureUnit* tex_unit)
{
	const String& name = tex_unit->Name;
	if (name.IsEmpty())return false;

	//设置纹理
	Texture* texture = tex_unit->GetTexture();
	if (texture == nullptr)return false;

	//UV缩放和偏移
	Vector4 tex_uv(tex_unit->TextureScale.x, tex_unit->TextureScale.y, tex_unit->TextureOffset.x, tex_unit->TextureOffset.y);
	SetVariable(name + "_ST", tex_uv.ptr(), 4);

	//贴图大小
	Vector4 tex_size(1.0f / texture->GetWidth(), 1.0f / texture->GetHeight(), (float)texture->GetWidth(), (float)texture->GetHeight());
	SetVariable(name + "_TexelSize", tex_size.ptr(), 4);

	return true;
}
bool VKProgram::SetVariable(byte index, const String& name, Texture* texture)
{
	//UV缩放和偏移
	Vector4 tex_uv(1, 1, 0, 0);
	SetVariable(name + "_ST", tex_uv.ptr(), 4);

	//贴图大小
	Vector4 tex_size(1.0f / texture->GetWidth(), 1.0f / texture->GetHeight(), (float)texture->GetWidth(), (float)texture->GetHeight());
	SetVariable(name + "_TexelSize", tex_size.ptr(), 4);

	return true;
}
bool VKProgram::SetPassVariable(byte index, const String& name, Texture* texture)
{
	//UV缩放和偏移
	Vector4 tex_uv(1, 1, 0, 0);
	SetVariable(name + "_ST", tex_uv.ptr(), 4);

	//贴图大小
	Vector4 tex_size(1.0f / texture->GetWidth(), 1.0f / texture->GetHeight(), (float)texture->GetWidth(), (float)texture->GetHeight());
	SetVariable(name + "_TexelSize", tex_size.ptr(), 4);

	return true;
}
void VKProgram::CreateCBuffer()
{
	for (const auto& obj : _reflectCBuffers)
	{
		const sShaderReflectCBuffer& cbuffer = obj.second;
		VKCBuffer* vk_cbuffer = VKCBuffer::Create(cbuffer);
		AssertEx(_cbBuffers.Add(cbuffer.name, vk_cbuffer), "");
	}
}
void VKProgram::CreateDescriptorSetLayout(Pass* pass)
{
	VkDevice device = GetVKDevice()->GetDevice();
	VkAllocationCallbacks* allocator = GetVKDevice()->GetAllocator();

	{//cbuffer
		Vector<VkDescriptorSetLayoutBinding> binding;
		for (const auto& obj : _reflectCBuffers)
		{
			const sShaderReflectCBuffer& cbuffer = obj.second;
			VkDescriptorSetLayoutBinding bind = {};
			bind.binding = cbuffer.slot;
			bind.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			bind.descriptorCount = 1;
			bind.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
			bind.pImmutableSamplers = nullptr;
			binding.Add(bind);
		}
		if (!binding.IsEmpty())
		{
			VkDescriptorSetLayoutCreateInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			info.bindingCount = binding.Size();
			info.pBindings = binding.Data();
			VkDescriptorSetLayout layout = VK_NULL_HANDLE;
			VK_ERROR(vkCreateDescriptorSetLayout(device, &info, allocator, &layout));
			_descriptorSetLayout.Add(layout);
		}
	}

	{//image
		Vector<VkDescriptorSetLayoutBinding> binding;
		for (const auto& obj : _reflectTextures)
		{
			const sShaderReflectTexture& rtexture = obj.second;

			VkDescriptorSetLayoutBinding bind = {};
			bind.binding = rtexture.slot;
			bind.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			bind.descriptorCount = 1;
			bind.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
			bind.pImmutableSamplers = nullptr;
			binding.Add(bind);
		}
		if (!binding.IsEmpty())
		{
			VkDescriptorSetLayoutCreateInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			info.bindingCount = binding.Size();
			info.pBindings = binding.Data();
			VkDescriptorSetLayout layout = VK_NULL_HANDLE;
			VK_ERROR(vkCreateDescriptorSetLayout(device, &info, allocator, &layout));
			_descriptorSetLayout.Add(layout);
		}
	}

	{//sample
		Vector<VkDescriptorSetLayoutBinding> binding;
		for (const auto& obj : _reflectTextures)
		{
			const sShaderReflectTexture& rtexture = obj.second;

			VkDescriptorSetLayoutBinding bind = {};
			bind.binding = rtexture.slot;
			bind.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
			bind.descriptorCount = 1;
			bind.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
			bind.pImmutableSamplers = nullptr;
			binding.Add(bind);
		}
		if (!binding.IsEmpty())
		{
			VkDescriptorSetLayoutCreateInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			info.bindingCount = binding.Size();
			info.pBindings = binding.Data();
			VkDescriptorSetLayout layout = VK_NULL_HANDLE;
			VK_ERROR(vkCreateDescriptorSetLayout(device, &info, allocator, &layout));
			_descriptorSetLayout.Add(layout);
		}
	}
}
void VKProgram::CreatePipelineLayout()
{
	VkDevice device = GetVKDevice()->GetDevice();
	VkAllocationCallbacks* allocator = GetVKDevice()->GetAllocator();

	VkPipelineLayoutCreateInfo layout_info = {};
	layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	layout_info.setLayoutCount = _descriptorSetLayout.Size();
	layout_info.pSetLayouts = _descriptorSetLayout.IsEmpty() ? nullptr : _descriptorSetLayout.Data();
	layout_info.pushConstantRangeCount = 0;
	layout_info.pPushConstantRanges = nullptr;
	VK_ERROR(vkCreatePipelineLayout(device, &layout_info, allocator, &_pipelineLayout));
}
void VKProgram::CreateDescriptorSet(Pass* pass)
{
	if (_descriptorSetLayout.IsEmpty())return;

	VkDevice device = GetVKDevice()->GetDevice();
	VkAllocationCallbacks* allocator = GetVKDevice()->GetAllocator();

	VkDescriptorSetAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	alloc_info.descriptorPool = GetVKDevice()->GetDescriptorPool();
	alloc_info.descriptorSetCount = _descriptorSetLayout.Size();
	alloc_info.pSetLayouts = _descriptorSetLayout.Data();
	_descriptorSet.Resize(_descriptorSetLayout.Size());
	VK_ERROR(vkAllocateDescriptorSets(device, &alloc_info, _descriptorSet.Data()));

	int descriptor_set_index = -1;
	Vector<VkWriteDescriptorSet> writes;
	Vector<VkDescriptorBufferInfo> desc_bufs(_cbBuffers.Size());
	if (!_cbBuffers.IsEmpty())
	{//cbuffer
		int index = 0;
		++descriptor_set_index;
		for (const auto& obj : _cbBuffers)
		{
			VKCBuffer* cbuffer = obj.second;

			desc_bufs[index] = {};
			desc_bufs[index].buffer = cbuffer->uniformBuffer;
			desc_bufs[index].offset = 0;
			desc_bufs[index].range = cbuffer->info.size;

			VkWriteDescriptorSet write = {};
			write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write.dstSet = _descriptorSet[descriptor_set_index];
			write.dstBinding = cbuffer->info.slot;
			write.dstArrayElement = 0;
			write.descriptorCount = 1;
			write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			write.pBufferInfo = &desc_bufs[index++];
			writes.Add(write);
		}
	}

	Vector<VkDescriptorImageInfo> desc_images(_reflectTextures.Size());
	if (!_reflectTextures.IsEmpty())
	{//image
		int index = 0;
		++descriptor_set_index;
		for (const auto& obj : _reflectTextures)
		{
			const sShaderReflectTexture& rtexture = obj.second;
			Texture* texture = pass->GetTexUnit(index)->GetTexture();

			desc_images[index] = {};
			desc_images[index].sampler = VK_NULL_HANDLE;
			desc_images[index].imageView = (VkImageView)texture->GetTextureView();
			desc_images[index].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			VkWriteDescriptorSet write = {};
			write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write.dstSet = _descriptorSet[descriptor_set_index];
			write.dstBinding = rtexture.slot;
			write.descriptorCount = 1;
			write.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			write.pImageInfo = &desc_images[index++];
			writes.Add(write);
		}
	}

	Vector<VkDescriptorImageInfo> desc_samples(_reflectTextures.Size());
	if (!_reflectTextures.IsEmpty())
	{//sample
		int index = 0;
		++descriptor_set_index;
		for (const auto& obj : _reflectTextures)
		{
			const sShaderReflectTexture& rtexture = obj.second;
			Texture* texture = pass->GetTexUnit(index)->GetTexture();

			desc_samples[index] = {};
			desc_samples[index].sampler = (VkSampler)texture->GetTextureSample();

			VkWriteDescriptorSet write = {};
			write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write.dstSet = _descriptorSet[descriptor_set_index];
			write.dstBinding = rtexture.slot;
			write.descriptorCount = 1;
			write.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
			write.pImageInfo = &desc_samples[index++];
			writes.Add(write);
		}
	}
	vkUpdateDescriptorSets(device, writes.Size(), writes.Data(), 0, NULL);
}
DC_END_NAMESPACE