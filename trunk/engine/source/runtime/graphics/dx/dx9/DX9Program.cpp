#include "DX9Program.h"
#include "DX9Device.h"
#include "DX9Caps.h"
#include "DX9RenderContent.h"
#include "runtime/graphics/TextureUnit.h"
#include "runtime/graphics/null/Texture.h"
#include "runtime/graphics/null/RenderTexture.h"
#include "runtime/scene/Camera.h"
#include "runtime/input/Input.h"
#include "runtime/Application.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
bool DX9CompileShaderFromFile(const String& file, const VecString& defines, const String& enter_point, const String& compile_target, ID3DXConstantTable* &constTable, ID3DXBuffer* &codeBuffer)
{
	//没有就重新编译
	Debuger::Log("DX9CompileShaderFromFile:%s, enter_point:%s, target:%s", file.c_str(), enter_point.c_str(), compile_target.c_str());
	String file_content = File::ReadAllText(file);
	return DX9CompileShaderFromMemory(file_content, defines, enter_point, compile_target, constTable, codeBuffer);
}
bool DX9CompileShaderFromMemory(const String& code, const VecString& defines, const String& enter_point, const String& compile_target, ID3DXConstantTable* &constTable, ID3DXBuffer* &codeBuffer)
{
	DWORD dwShaderFlags = 0;
#ifdef DC_DEBUG
	// 设置 D3DCOMPILE_DEBUG 标志用于获取着色器调试信息。该标志可以提升调试体验，
	// 但仍然允许着色器进行优化操作
	dwShaderFlags |= D3DXSHADER_DEBUG;
#endif

	//NOTE:HLSL中#if需要Definition为1，#ifdef只需要定义了Name就会成立
	VecString shader_defineds;
	shader_defineds.Add(ShaderVersion[int(ShaderModel::SM_3_0)]);
	shader_defineds.Add(HLSLDefineVersion[int(ShaderModel::SM_3_0)]);
	shader_defineds.Add("SHADER_API_DIRECTX");
	shader_defineds.Add("UV_STARTS_AT_TOP");
#if defined(DC_COLORSPACE_LINEAR)
	shader_defineds.Add("COLORSPACE_LINEAR");
#endif
	std::unique_ptr<D3DXMACRO[]> d3d_defines(DBG_NEW D3DXMACRO[shader_defineds.Size() + 1]);
	for (int i = 0; i < shader_defineds.Size(); ++i)
	{
		d3d_defines[i].Name = shader_defineds[i].c_str();
		d3d_defines[i].Definition = "1";
	}
	d3d_defines[shader_defineds.Size()].Name = NULL;
	d3d_defines[shader_defineds.Size()].Definition = NULL;

	ID3DXBuffer* errorBuffer = NULL;
	HRESULT hr = D3DXCompileShader(code.c_str(), code.Size(), d3d_defines.get(), NULL, enter_point.c_str(), compile_target.c_str(), dwShaderFlags, &codeBuffer, &errorBuffer, &constTable);
	if (errorBuffer != NULL)
	{
		Debuger::Error("DX9CompileShaderFromMemory: %s", (char*)errorBuffer->GetBufferPointer());
		errorBuffer->Release();
		return false;
	}
	if (FAILED(hr))
	{
		Debuger::Error("DX9CompileShaderFromMemory  Failed");
		return false;
	}
	SAFE_RELEASE(errorBuffer);
	return true;
}
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(DX9Program, CGProgram);
DX9Program::~DX9Program()
{
	SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_pPixelShader);
	for (int i = 0; i < (int)ShaderType::Max; ++i)
	{
		SAFE_RELEASE(_constantTable[i]);
	}
}
bool DX9Program::LoadFromFile(const String& file)
{
	ShaderDesc info;
	info.ShaderFile[int(ShaderType::Vertex)] = file;
	info.ShaderFile[int(ShaderType::Pixel)] = file;
	return LoadFromFile(info);
}
bool DX9Program::LoadFromFile(const ShaderDesc& shader_info)
{
	base::LoadFromFile(shader_info);

	bool result = true;
	const String& shader_version = Application::GetGraphics()->GetShaderVersion();
	for (int i = 0; i < int(ShaderType::Max); ++i)
	{
		if (shader_info.ShaderFile[i].IsEmpty())continue;

		ShaderType shader_type = ShaderType(i);
		if (shader_type != ShaderType::Vertex && shader_type != ShaderType::Pixel)continue;

		String enter_point = shader_info.Enter[i];
		if (enter_point.IsEmpty())enter_point = ShaderEnterEnum[i];
		String target = shader_info.Target[i];
		if (target.IsEmpty())target = DXGetShaderTarget(shader_type, shader_version);
		String file_path = Resource::GetFullDataPath(shader_info.ShaderFile[i]);

		ID3DXBuffer* codeBuffer = NULL;
		if (!DX9CompileShaderFromFile(file_path, shader_info.ShaderDefines, enter_point, target, _constantTable[i], codeBuffer))
			break;
		switch (shader_type)
		{
		case ShaderType::Vertex:
			DX9HR(GetDX9Device()->GetDevice()->CreateVertexShader((DWORD*)codeBuffer->GetBufferPointer(), &m_pVertexShader));
			break;
		case ShaderType::Pixel:
			DX9HR(GetDX9Device()->GetDevice()->CreatePixelShader((DWORD*)codeBuffer->GetBufferPointer(), &m_pPixelShader));
			break;
		}
		SAFE_RELEASE(codeBuffer);
	}

	return result;
}
bool DX9Program::LoadFromMemory(const String& name, const VecString& codes, const VecString& defines)
{
	base::LoadFromMemory(name, codes, defines);
	bool result = true;
	for (int i = 0; i < codes.Size(); ++i)
	{
		const String& code = codes[i];
		if (code.IsEmpty())continue;

		ShaderType shader_type = ShaderType(i);
		if (shader_type != ShaderType::Vertex && shader_type != ShaderType::Pixel)continue;

		const String& shader_version = Application::GetGraphics()->GetShaderVersion();
		String enter_point = ShaderEnterEnum[i];
		String target = DXGetShaderTarget(shader_type, shader_version);

		ID3DXBuffer* codeBuffer = NULL;
		if (!DX9CompileShaderFromMemory(code, defines, enter_point, target, _constantTable[i], codeBuffer))
			break;
		switch (shader_type)
		{
		case ShaderType::Vertex:
			DX9HR(GetDX9Device()->GetDevice()->CreateVertexShader((DWORD*)codeBuffer->GetBufferPointer(), &m_pVertexShader));
			break;
		case ShaderType::Pixel:
			DX9HR(GetDX9Device()->GetDevice()->CreatePixelShader((DWORD*)codeBuffer->GetBufferPointer(), &m_pPixelShader));
			break;
		}
		SAFE_RELEASE(codeBuffer);
	}

	return result;
}
void DX9Program::PreRender()
{

}
void DX9Program::PostRender()
{
	DX9HR(GetDX9Device()->GetDevice()->SetVertexShader(m_pVertexShader));
	DX9HR(GetDX9Device()->GetDevice()->SetPixelShader(m_pPixelShader));
}
bool DX9Program::SetVariable(const String& name, const Matrix4& mat)
{
	const sShaderReflectVariable* cb_var = GetCBVariable(name);
	if (cb_var == nullptr)return false;
	
	for (uint type = (uint)ShaderType::Vertex; type < (uint)ShaderType::Max; ++type)
	{
		if (cb_var->stage & ShaderStageEnum[type] && _constantTable[type])
		{
			D3DXHANDLE hndl = _constantTable[type]->GetConstantByName(NULL, name.c_str());
			if (hndl)
			{
				UINT ucount = 1;
				D3DXCONSTANT_DESC Description;
				_constantTable[type]->GetConstantDesc(hndl, &Description, &ucount);
				if (Description.RegisterSet != D3DXRS_SAMPLER)
				{
					Matrix4 t_mat = mat.Transpose();
					DX9HR(_constantTable[type]->SetFloatArray(GetDX9Device()->GetDevice(), hndl, t_mat.p, 16));
				}
			}
		}
	}
	return true;
}
bool DX9Program::SetVariable(const String& name, const Matrix4* mats, int count)
{
	const sShaderReflectVariable* cb_var = GetCBVariable(name);
	if (cb_var == nullptr)return false;

	for (uint type = (uint)ShaderType::Vertex; type < (uint)ShaderType::Max; ++type)
	{
		if (cb_var->stage & ShaderStageEnum[type] && _constantTable[type])
		{
			D3DXHANDLE hndl = _constantTable[type]->GetConstantByName(NULL, name.c_str());
			if (hndl)
			{
				UINT ucount = 1;
				D3DXCONSTANT_DESC Description;
				_constantTable[type]->GetConstantDesc(hndl, &Description, &ucount);
				if (Description.RegisterSet != D3DXRS_SAMPLER)
				{
					Vector<Matrix4> t_mat(count);
					for (int i = 0; i < count; ++i)
					{
						t_mat[i] = mats->Transpose();
						mats += 1;
					}

					DX9HR(_constantTable[type]->SetFloatArray(GetDX9Device()->GetDevice(), hndl, (const float*)t_mat.Data(), 16 * count));
				}
			}
		}
	}
	return true;
}
bool DX9Program::SetVariable(const String& name, const float* floats, int count)
{
	const sShaderReflectVariable* cb_var = GetCBVariable(name);
	if (cb_var == nullptr)return false;

	for (uint type = (uint)ShaderType::Vertex; type < (uint)ShaderType::Max; ++type)
	{
		if (cb_var->stage & ShaderStageEnum[type] && _constantTable[type])
		{
			D3DXHANDLE hndl = _constantTable[type]->GetConstantByName(NULL, name.c_str());
			if (hndl)
			{
				UINT ucount = 1;
				D3DXCONSTANT_DESC Description;
				_constantTable[type]->GetConstantDesc(hndl, &Description, &ucount);
				if (Description.RegisterSet != D3DXRS_SAMPLER)
				{
					DX9HR(_constantTable[type]->SetFloatArray(GetDX9Device()->GetDevice(), hndl, floats, count));
				}
			}
		}
	}
	return true;
}
bool DX9Program::SetVariable(const String& name, const Vector4* vecs, int count)
{
	const sShaderReflectVariable* cb_var = GetCBVariable(name);
	if (cb_var == nullptr)return false;

	for (uint type = (uint)ShaderType::Vertex; type < (uint)ShaderType::Max; ++type)
	{
		if (cb_var->stage & ShaderStageEnum[type] && _constantTable[type])
		{
			D3DXHANDLE hndl = _constantTable[type]->GetConstantByName(NULL, name.c_str());
			if (hndl)
			{
				UINT ucount = 1;
				D3DXCONSTANT_DESC Description;
				_constantTable[type]->GetConstantDesc(hndl, &Description, &ucount);
				if (Description.RegisterSet != D3DXRS_SAMPLER)
				{
					DX9HR(_constantTable[type]->SetFloatArray(GetDX9Device()->GetDevice(), hndl, (const float*)vecs, 4 * count));
				}
			}
		}
	}
	return true;
}
bool DX9Program::SetVariable(const String& name, const int* ints, int count)
{
	const sShaderReflectVariable* cb_var = GetCBVariable(name);
	if (cb_var == nullptr)return false;

	for (uint type = (uint)ShaderType::Vertex; type < (uint)ShaderType::Max; ++type)
	{
		if (cb_var->stage & ShaderStageEnum[type] && _constantTable[type])
		{
			D3DXHANDLE hndl = _constantTable[type]->GetConstantByName(NULL, name.c_str());
			if (hndl)
			{
				UINT ucount = 1;
				D3DXCONSTANT_DESC Description;
				_constantTable[type]->GetConstantDesc(hndl, &Description, &ucount);
				if (Description.RegisterSet != D3DXRS_SAMPLER)
				{
					DX9HR(_constantTable[type]->SetIntArray(GetDX9Device()->GetDevice(), hndl, ints, count));
				}
			}
		}
	}
	return true;
}
bool DX9Program::SetVariable(const String& name, const bool bools)
{
	const sShaderReflectVariable* cb_var = GetCBVariable(name);
	if (cb_var == nullptr)return false;

	for (uint type = (uint)ShaderType::Vertex; type < (uint)ShaderType::Max; ++type)
	{
		if (cb_var->stage & ShaderStageEnum[type] && _constantTable[type])
		{
			D3DXHANDLE hndl = _constantTable[type]->GetConstantByName(NULL, name.c_str());
			if (hndl)
			{
				UINT ucount = 1;
				D3DXCONSTANT_DESC Description;
				_constantTable[type]->GetConstantDesc(hndl, &Description, &ucount);
				if (Description.RegisterSet != D3DXRS_SAMPLER)
				{
					DX9HR(_constantTable[type]->SetBoolArray(GetDX9Device()->GetDevice(), hndl, (BOOL*)bools, 1));
				}
			}
		}
	}
	return true;
}
bool DX9Program::SetVariable(const String& name, const ShaderLight& light, int index)
{
	const sShaderReflectVariable* cb_var = GetCBVariable(name);
	if (cb_var == nullptr)return false;

	String shader_name = name + "[" + String::ToString(index) + "]";
	for (uint type = (uint)ShaderType::Vertex; type < (uint)ShaderType::Max; ++type)
	{
		if (cb_var->stage & ShaderStageEnum[type] && _constantTable[type])
		{
			D3DXHANDLE hndl = _constantTable[type]->GetConstantByName(NULL, shader_name.c_str());
			if (hndl)
			{
				UINT ucount = 1;
				D3DXCONSTANT_DESC Description;
				_constantTable[type]->GetConstantDesc(hndl, &Description, &ucount);
				if (Description.RegisterSet != D3DXRS_SAMPLER)
				{
					DX9HR(_constantTable[type]->SetValue(GetDX9Device()->GetDevice(), hndl, &light, sizeof(ShaderLight)));
				}
			}
		}
	}
	return true;
}
bool DX9Program::SetVariable(byte index, TextureUnit* tex_unit)
{
	const String& name = tex_unit->Name;
	if (name.IsEmpty())return false;

	//设置纹理
	Texture* texture = tex_unit->GetTexture();
	if (texture == nullptr)return false;
	int aniso = Math::Min<int>(GetGraphicsCaps()->maxAnisoLevel, tex_unit->Anisotropy);

	if (texture->GetTextureView() == nullptr)
	{
		GetDX9Device()->GetDevice()->SetTexture(index, 0);
		return false;
	}
	DX9HR(GetDX9Device()->GetDevice()->SetTexture(index, (IDirect3DTexture9*)texture->GetTextureView()));
	{
		//UV缩放和偏移
		Vector4 tex_uv(tex_unit->TextureScale.x, tex_unit->TextureScale.y, tex_unit->TextureOffset.x, tex_unit->TextureOffset.y);
		SetVariable(name + "_ST", tex_uv.ptr(), 4);

		//贴图大小
		Vector4 tex_size(1.0f / texture->GetWidth(), 1.0f / texture->GetHeight(), texture->GetWidth(), texture->GetHeight());
		SetVariable(name + "_TexelSize", tex_size.ptr(), 4);
	}

	{//渲染状态
		//纹理过滤
		GetDX9Device()->SetD3DSamplerState(index, D3DSAMP_MAGFILTER, DX9GetTextureFilter(tex_unit->FilterType, aniso));//放大
		GetDX9Device()->SetD3DSamplerState(index, D3DSAMP_MINFILTER, DX9GetTextureFilter(tex_unit->FilterType, aniso));//缩小
		//mipmap
		if (tex_unit->FilterType == TextureFilter::Trilinear)
		{
			GetDX9Device()->SetD3DSamplerState(index, D3DSAMP_MIPFILTER, DX9GetTextureMipFilter(tex_unit->FilterType));
			GetDX9Device()->SetD3DSamplerState(index, D3DSAMP_MAXMIPLEVEL, 0);
		}
		else
		{
			GetDX9Device()->SetD3DSamplerState(index, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
		}
		//各向异性
		GetDX9Device()->SetD3DSamplerState(index, D3DSAMP_MAXANISOTROPY, aniso);

		//纹理寻址模式
		GetDX9Device()->SetD3DSamplerState(index, D3DSAMP_ADDRESSU, DX9GetTextureAddress(tex_unit->AddressMode));
		GetDX9Device()->SetD3DSamplerState(index, D3DSAMP_ADDRESSV, DX9GetTextureAddress(tex_unit->AddressMode));
		GetDX9Device()->SetD3DSamplerState(index, D3DSAMP_ADDRESSW, DX9GetTextureAddress(tex_unit->AddressMode));
		if (tex_unit->AddressMode == TextureAddress::Border)
		{
			GetDX9Device()->SetD3DSamplerState(index, D3DSAMP_BORDERCOLOR, DX9ToColor(tex_unit->BorderColor));
		}

		//sRGB
#if defined(DC_COLORSPACE_LINEAR)
		GetDX9Device()->SetD3DSamplerState(index, D3DSAMP_SRGBTEXTURE, tex_unit->IsSRGB());
#else
		GetDX9Device()->SetD3DSamplerState(index, D3DSAMP_SRGBTEXTURE, false);
#endif
	}
	return true;
}
bool DX9Program::SetVariable(byte index, const String& name, Texture* texture)
{
	if (texture == nullptr || texture->GetTextureView() == nullptr)
	{
		GetDX9Device()->GetDevice()->SetTexture(index, 0);
		return false;
	}
	DX9HR(GetDX9Device()->GetDevice()->SetTexture(index, (IDirect3DTexture9*)texture->GetTextureView()));
	{
		//UV缩放和偏移
		Vector4 tex_uv(1, 1, 0, 0);
		SetVariable(name + "_ST", tex_uv.ptr(), 4);

		//贴图大小
		Vector4 tex_size(1.0f / texture->GetWidth(), 1.0f / texture->GetHeight(), texture->GetWidth(), texture->GetHeight());
		SetVariable(name + "_TexelSize", tex_size.ptr(), 4);
	}
	{//渲染状态
		//纹理过滤
		GetDX9Device()->SetD3DSamplerState(index, D3DSAMP_MAGFILTER, DX9GetTextureFilter(texture->GetFilterType(), texture->GetAnisotropy()));//放大
		GetDX9Device()->SetD3DSamplerState(index, D3DSAMP_MINFILTER, DX9GetTextureFilter(texture->GetFilterType(), texture->GetAnisotropy()));//缩小

		//mipmap
		if (texture->GetFilterType() == TextureFilter::Trilinear)
		{
			GetDX9Device()->SetD3DSamplerState(index, D3DSAMP_MIPFILTER, DX9GetTextureMipFilter(texture->GetFilterType()));
			GetDX9Device()->SetD3DSamplerState(index, D3DSAMP_MAXMIPLEVEL, 0);
		}
		else
		{
			GetDX9Device()->SetD3DSamplerState(index, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
		}

		//各向异性
		GetDX9Device()->SetD3DSamplerState(index, D3DSAMP_MAXANISOTROPY, texture->GetAnisotropy());

		//纹理寻址模式
		GetDX9Device()->SetD3DSamplerState(index, D3DSAMP_ADDRESSU, DX9GetTextureAddress(texture->GetAddressMode()));
		GetDX9Device()->SetD3DSamplerState(index, D3DSAMP_ADDRESSV, DX9GetTextureAddress(texture->GetAddressMode()));
		GetDX9Device()->SetD3DSamplerState(index, D3DSAMP_ADDRESSW, DX9GetTextureAddress(texture->GetAddressMode()));

		//sRGB
#if defined(DC_COLORSPACE_LINEAR)
		GetDX9Device()->SetD3DSamplerState(index, D3DSAMP_SRGBTEXTURE, texture->IsSRGB());
#else
		GetDX9Device()->SetD3DSamplerState(index, D3DSAMP_SRGBTEXTURE, false);
#endif
	}
	return true;
}
bool DX9Program::SetPassVariable(byte index, const String& name, Texture* texture)
{
	if (texture == nullptr || texture->GetTextureView() == nullptr)
	{
		GetDX9Device()->GetDevice()->SetTexture(index, 0);
		return false;
	}
	DX9HR(GetDX9Device()->GetDevice()->SetTexture(index, (IDirect3DTexture9*)texture->GetTextureView()));
	{
		//UV缩放和偏移
		Vector4 tex_uv(1, 1, 0, 0);
		SetVariable(name + "_ST", tex_uv.ptr(), 4);

		//贴图大小
		Vector4 tex_size(1.0f / texture->GetWidth(), 1.0f / texture->GetHeight(), texture->GetWidth(), texture->GetHeight());
		SetVariable(name + "_TexelSize", tex_size.ptr(), 4);
	}
	{//渲染状态
		//sRGB
#if defined(DC_COLORSPACE_LINEAR)
		GetDX9Device()->SetD3DSamplerState(index, D3DSAMP_SRGBTEXTURE, texture->IsSRGB());
#else
		GetDX9Device()->SetD3DSamplerState(index, D3DSAMP_SRGBTEXTURE, false);
#endif
	}
	return true;
}
DC_END_NAMESPACE