 
/*****************************************************************************
* Author： hannibal
* Date：2020/3/18
* Description：
*****************************************************************************/
#pragma once

#include "runtime/graphics/null/CGProgram.h"
#include "DX11Define.h"

DC_BEGIN_NAMESPACE
class Texture;
class DX11ShaderReflect;
/********************************************************************/
//cbuffer
class DX11CBuffer Final : public object
{
	friend class DX11Program;
	DEFAULT_CREATE(DX11CBuffer);
	FRIEND_CONSTRUCT_DESTRUCT(DX11CBuffer);
private:
	bool dirty = true;
	sShaderReflectCBuffer info;
	byte* buffer = nullptr;
	ID3D11Buffer* d3d_buffer = nullptr;

private:
	DX11CBuffer(const sShaderReflectCBuffer& cbuffer);
	~DX11CBuffer();

	void Update();
	void Copy(uint offset, const void* src_data, uint size);
};

/********************************************************************/
// D3D11 shader程序
class DX11Program Final : public CGProgram
{
	friend class DX11Device;
	typedef Map<String, DX11CBuffer*> cbuffers_t;
	DEFAULT_CREATE(DX11Program);
	FRIEND_CONSTRUCT_DESTRUCT(DX11Program);
	DISALLOW_COPY_ASSIGN(DX11Program);
	BEGIN_DERIVED_REFECTION_TYPE(DX11Program, CGProgram)
	END_DERIVED_REFECTION_TYPE;

	DX11Program();

public:
	~DX11Program();
	virtual bool LoadFromFile(const String& file)override;
	virtual bool LoadFromFile(const ShaderDesc& info)override;
	virtual bool LoadFromMemory(const String& name, const VecString& codes, const VecString& defines)override;

	virtual void PreRender()override;
	virtual void PostRender()override;

	ID3DBlob* GetBlob(ShaderType type)const { return _codeBuffer[int(type)]; }

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
	void SetShader();

	const sShaderReflectVariable* GetCBVariable(const String& name)const;

private:
	ID3DBlob*			_codeBuffer[(int)ShaderType::Max] = { nullptr };
	IUnknown*			_shaderInstance[(int)ShaderType::Max] = { nullptr };

	cbuffers_t			_cBBuffers;
#if DC_DEBUG
	DX11ShaderReflect*	_shaderReflect = nullptr;
#endif
};
DC_END_NAMESPACE
