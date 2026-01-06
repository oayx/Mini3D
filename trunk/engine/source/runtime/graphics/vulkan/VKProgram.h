 
/*****************************************************************************
* Author： hannibal
* Date：2020/3/18
* Description：
*****************************************************************************/
#pragma once

#include "runtime/graphics/null/CGProgram.h"
#include "VKDefine.h"

DC_BEGIN_NAMESPACE
class Texture;
class TextureUnit;
/********************************************************************/
//cbuffer
class VKCBuffer final : public object
{
	friend class VKProgram;
	DEFAULT_CREATE(VKCBuffer);
	FRIEND_CONSTRUCT_DESTRUCT(VKCBuffer);
private:
	bool dirty = true;
	sShaderReflectCBuffer info;
	byte* buffer = nullptr;
	VkBuffer uniformBuffer;
	VkDeviceMemory uniformBufferMemory;

private:
	VKCBuffer(const sShaderReflectCBuffer& cbuffer);
	~VKCBuffer();

	void PreUpdate();
	void PostUpdate();
	void Copy(uint offset, const void* src_data, uint size);
};
/********************************************************************/
// shader程序
class VKProgram final : public CGProgram
{
	friend class VKDevice;
	typedef Map<String, VKCBuffer*> cbuffers_t;
	typedef Vector<VkDescriptorSetLayout> VkDescriptorSetLayouts;
	typedef Vector<VkDescriptorSet> VkDescriptorSets;
	DEFAULT_CREATE(VKProgram);
	FRIEND_CONSTRUCT_DESTRUCT(VKProgram);
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(VKProgram);
	BEGIN_DERIVED_REFECTION_TYPE(VKProgram, CGProgram)
	END_REFECTION_TYPE;

public:
	~VKProgram();
	virtual bool LoadFromDesc(const ShaderDesc& info)override;
	virtual bool LoadFromMemory(const String& name, const VecString& codes, const VecString& defines)override;

	virtual void PreRender()override;
	virtual void Render(Camera* camera, Pass* pass, const Matrix4& obj_mat)override;
	virtual void PostRender()override;

public:
	virtual bool SetVariable(const String& name, const Matrix4& mat)override;
	virtual bool SetVariable(const String& name, const Matrix4* mats, int count)override;
	virtual bool SetVariable(const String& name, const float* floats, int count)override;
	virtual bool SetVariable(const String& name, const Vector4* vecs, int count)override;
	virtual bool SetVariable(const String& name, const int* ints, int count)override;
	virtual bool SetVariable(const String& name, const ShaderLight& light, int index)override;
	virtual bool SetVariable(const String& name, const bool bools)override;
	virtual bool SetVariable(byte index, TextureUnit* tex_unit)override;
	virtual bool SetVariable(byte index, const String& name, Texture* texture)override;
	virtual bool SetPassVariable(byte index, const String& name, Texture* texture)override;

public:
	VkShaderModule		GetShaderModule(ShaderType type)const { return _shaderModule[int(type)]; }
	VkPipelineLayout	GetPipelineLayout()const { return _pipelineLayout; }

private:
	void CreateCBuffer();
	void CreateDescriptorSetLayout(Pass* pass);
	void CreatePipelineLayout();
	void CreateDescriptorSet(Pass* pass);

private:
	cbuffers_t				_cbBuffers;

	VkShaderModule			_shaderModule[(int)ShaderType::Max] = { VK_NULL_HANDLE };

	VkDescriptorSetLayouts	_descriptorSetLayout;
	VkPipelineLayout		_pipelineLayout = VK_NULL_HANDLE;
	VkDescriptorSets		_descriptorSet;
};
DC_END_NAMESPACE
