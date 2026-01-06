 
/*****************************************************************************
* Author： hannibal
* Date：2020/3/18
* Description：
*****************************************************************************/
#pragma once

#include "runtime/graphics/null/CGProgram.h"
#include "DX12Define.h"

DC_BEGIN_NAMESPACE
class Texture;
class DX12ShaderReflect;
/********************************************************************/
//cbuffer
class DX12CBuffer final : public object
{
	friend class DX12Program;
	DEFAULT_CREATE(DX12CBuffer);
	FRIEND_CONSTRUCT_DESTRUCT(DX12CBuffer);

private:
	bool dirty = true;
	sShaderReflectCBuffer info;
	byte* buffer = nullptr;
	ID3D12Resource* d3d_buffer = nullptr;
	ID3D12DescriptorHeap* cbv_heap = nullptr;

private:
	DX12CBuffer(const sShaderReflectCBuffer& cbuffer);
	~DX12CBuffer();

	void Update();
	void Copy(uint offset, const void* src_data, uint size);
};
/********************************************************************/
// shader程序
class DX12Program final : public CGProgram
{
	friend class DX12Device;
	typedef Map<String, DX12CBuffer*> cbuffers_t;
	typedef Vector<cbuffers_t> cbuffers;
	DEFAULT_CREATE(DX12Program);
	FRIEND_CONSTRUCT_DESTRUCT(DX12Program);
	DISALLOW_COPY_ASSIGN(DX12Program);
	BEGIN_DERIVED_REFECTION_TYPE(DX12Program, CGProgram)
	END_REFECTION_TYPE;

	DX12Program();
	~DX12Program();

public:
	virtual bool LoadFromFile(const String& file)override;
	virtual bool LoadFromDesc(const ShaderDesc& info)override;
	virtual bool LoadFromMemory(const String& name, const VecString& codes, const VecString& defines)override;

	virtual void PreRender()override;
	virtual void PostRender()override;

	ID3DBlob*	 GetCodeBuffer(ShaderType type)const { return _codeBuffer[(int)type]; }

public:
	virtual bool SetVariable(const String& name, const Matrix4& mat)override;
	virtual bool SetVariable(const String& name, const Matrix4* mats, int count)override;
	virtual bool SetVariable(const String& name, const float* floats, int count)override;
	virtual bool SetVariable(const String& name, const Vector4* vecs, int count)override;
	virtual bool SetVariable(const String& name, const int* ints, int count)override;
	virtual bool SetVariable(const String& name, const bool bools)override;
	virtual bool SetVariable(const String& name, const ShaderLight& light, int index)override;
	virtual bool SetVariable(byte index, TextureUnit* tex_unit)override;
	virtual bool SetVariable(byte index, const String& name, Texture* texture)override;
	virtual bool SetPassVariable(byte index, const String& name, Texture* texture)override;

private:
	void CreateShader(ShaderType type);
	void CreateCBuffer();
	void OnPreCommandEvt();

	const sShaderReflectVariable* GetCBVariable(const String& name)const;
	DX12CBuffer* GetCBuffer(const String& name)const;

private:
	ID3DBlob*			_codeBuffer[(int)ShaderType::Max] = {nullptr};

	int					_currentCBIndex = -1;		//每次渲染，需要使用独立的cb
	cbuffers			_cBBuffers;
#if DC_DEBUG
	DX12ShaderReflect*	_shaderReflect = nullptr;
#endif
};
DC_END_NAMESPACE

