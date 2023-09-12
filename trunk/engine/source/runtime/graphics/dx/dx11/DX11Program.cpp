#include "DX11Program.h"
#include "DX11Device.h"
#include "DX11RenderContent.h"
#include "DX11ShaderReflect.h"
#include "runtime/graphics/TextureUnit.h"
#include "runtime/graphics/null/Texture.h"
#include "runtime/graphics/null/RenderTexture.h"
#include "runtime/scene/Camera.h"
#include "runtime/input/Input.h"
#include "runtime/Application.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
DX11CBuffer::DX11CBuffer(const sShaderReflectCBuffer& cbuffer)
{
	this->info = cbuffer;
	this->buffer = NewArray<byte>(cbuffer.size);
	Memory::Clear(this->buffer, cbuffer.size);

	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = cbuffer.size;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	HR(GetDX11Device()->GetDevice()->CreateBuffer(&bd, NULL, &this->d3d_buffer));
}
DX11CBuffer::~DX11CBuffer()
{
	if (buffer)
	{
		DeleteArray(buffer);
		buffer = nullptr;
	}
	SAFE_RELEASE(d3d_buffer);
}
void DX11CBuffer::Update()
{
	if (dirty)
	{
		GetDX11Device()->GetContent()->UpdateSubresource(d3d_buffer, 0, NULL, buffer, 0, 0);
		dirty = false;
	}

	for (int i = 0; i < int(ShaderType::Max); ++i)
	{
		ShaderType type = ShaderType(i);
		switch (type)
		{
		case ShaderType::Vertex:
			GetDX11Device()->GetContent()->VSSetConstantBuffers(info.slot, 1, &d3d_buffer);
			break;
		case ShaderType::Geometry:
			GetDX11Device()->GetContent()->GSSetConstantBuffers(info.slot, 1, &d3d_buffer);
			break;
		case ShaderType::Pixel:
			GetDX11Device()->GetContent()->PSSetConstantBuffers(info.slot, 1, &d3d_buffer);
			break;
		case ShaderType::Compute:
			GetDX11Device()->GetContent()->CSSetConstantBuffers(info.slot, 1, &d3d_buffer);
			break;
		default:
			break;
		}
	}
}
void DX11CBuffer::Copy(uint offset, const void* src_data, uint size)
{
	::memcpy(buffer + offset, src_data, size);
	dirty = true;
}
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(DX11Program, CGProgram);
DX11Program::DX11Program()
{
#if DC_DEBUG
	_shaderReflect = DX11ShaderReflect::Create();
#endif
	for (int i = 0; i < (int)ShaderType::Max; ++i)
	{
		_codeBuffer[i] = nullptr;
		_shaderInstance[i] = nullptr;
	}
}
DX11Program::~DX11Program()
{
#if DC_DEBUG
	SAFE_DELETE(_shaderReflect);
#endif
	for (int i = 0; i < (int)ShaderType::Max; ++i)
	{
		SAFE_RELEASE(_codeBuffer[i]);
		SAFE_RELEASE(_shaderInstance[i]);
	}
	for (auto obj : _cBBuffers)
	{
		SAFE_DELETE(obj.second);
	}
	_cBBuffers.Clear();
}
bool DX11Program::LoadFromFile(const String& file)
{
	ShaderDesc info;
	info.ShaderFile[int(ShaderType::Vertex)] = file;
	info.ShaderFile[int(ShaderType::Pixel)] = file;
	return LoadFromFile(info);
}
bool DX11Program::LoadFromFile(const ShaderDesc& shader_info)
{
	base::LoadFromFile(shader_info);

	bool result = true;
	const String& shader_version = Application::GetGraphics()->GetShaderVersion();
	for (int i = 0; i < int(ShaderType::Max); ++i)
	{
		if (shader_info.ShaderFile[i].IsEmpty())continue;

		ShaderType shader_type = ShaderType(i);
		String enter_point = shader_info.Enter[i];
		if (enter_point.IsEmpty())enter_point = ShaderEnterEnum[i];
		String target = shader_info.Target[i];
		if (target.IsEmpty())target = DXGetShaderTarget(shader_type, shader_version);
		String file_path = Resource::GetFullDataPath(shader_info.ShaderFile[i]);
		if (!DX11CompileShaderFromFile(file_path, shader_info.ShaderDefines, enter_point, target, &_codeBuffer[i]))
		{
			result = false;
			continue;
		}
		CreateShader(shader_type);
	}
	CreateCBuffer();

	return result;
}
bool DX11Program::LoadFromMemory(const String& name, const VecString& codes, const VecString& defines)
{
	base::LoadFromMemory(name, codes, defines);
	bool result = true;
	for(int i = 0; i < codes.Size(); ++i)
	{
		const String& code = codes[i];
		if (code.IsEmpty())continue;

		ShaderType shader_type = ShaderType(i);
		const String& shader_version = Application::GetGraphics()->GetShaderVersion();
		String enter_point = ShaderEnterEnum[i];
		String target = DXGetShaderTarget(shader_type, shader_version);
		if (!DX11CompileShader(name, code, defines, enter_point, target, &_codeBuffer[i]))
		{
			result = false;
			break;
		}
		CreateShader(shader_type);
	}
	CreateCBuffer();

	return result;
}
void DX11Program::PreRender()
{
}
void DX11Program::PostRender()
{
	for (const auto& cb : _cBBuffers)
	{
		auto cbuffer = cb.second;
		cbuffer->Update();
	}
	SetShader();
}
void DX11Program::CreateShader(ShaderType type)
{
	ID3DBlob* shader_code = _codeBuffer[(int)type];
	switch (type)
	{
		case ShaderType::Vertex:
		{
			ID3D11VertexShader*	vertexShader = nullptr;
			HR(GetDX11Device()->GetDevice()->CreateVertexShader(shader_code->GetBufferPointer(), shader_code->GetBufferSize(), NULL, &vertexShader));
			_shaderInstance[int(ShaderType::Vertex)] = vertexShader;
		}
		break;
		case ShaderType::Geometry:
		{
			ID3D11GeometryShader* geometryShader = nullptr;
			HR(GetDX11Device()->GetDevice()->CreateGeometryShader(shader_code->GetBufferPointer(), shader_code->GetBufferSize(), NULL, &geometryShader));
			_shaderInstance[int(ShaderType::Geometry)] = geometryShader;
		}
		break;
		case ShaderType::Pixel:
		{
			ID3D11PixelShader* pixelShader = nullptr;
			HR(GetDX11Device()->GetDevice()->CreatePixelShader(shader_code->GetBufferPointer(), shader_code->GetBufferSize(), NULL, &pixelShader));
			_shaderInstance[int(ShaderType::Pixel)] = pixelShader;
		}
		break;
		case ShaderType::Compute:
		{
			ID3D11ComputeShader* computeShader = nullptr;
			HR(GetDX11Device()->GetDevice()->CreateComputeShader(shader_code->GetBufferPointer(), shader_code->GetBufferSize(), NULL, &computeShader));
			_shaderInstance[int(ShaderType::Compute)] = computeShader;
		}
		break;
		default:
			AssertEx(false, "");
		break;
	}
#if DC_DEBUG
	_shaderReflect->Reflect(type, shader_code);
#endif
}
void DX11Program::CreateCBuffer()
{
#if DC_DEBUG
	for (const auto& obj : _shaderReflect->mCBBuffers)
#else
	for (const auto& obj : _reflectCBuffers)
#endif
	{
		const sShaderReflectCBuffer& cbuffer = obj.second;
		DX11CBuffer* d3d_cbuffer = DX11CBuffer::Create(cbuffer);
		AssertEx(_cBBuffers.Add(cbuffer.name, d3d_cbuffer),"");
	}
}
void DX11Program::SetShader()
{
	for (int i = 0; i < int(ShaderType::Max); ++i)
	{
		ShaderType type = ShaderType(i);
		switch (type)
		{
		case ShaderType::Vertex:
			GetDX11Device()->GetContent()->VSSetShader((ID3D11VertexShader*)(_shaderInstance[i]), NULL, 0);
			break;
		case ShaderType::Hull:
			GetDX11Device()->GetContent()->HSSetShader((ID3D11HullShader*)(_shaderInstance[i]), NULL, 0);
			break;
		case ShaderType::Domain:
			GetDX11Device()->GetContent()->DSSetShader((ID3D11DomainShader*)(_shaderInstance[i]), NULL, 0);
			break;
		case ShaderType::Geometry:
			GetDX11Device()->GetContent()->GSSetShader((ID3D11GeometryShader*)(_shaderInstance[i]), NULL, 0);
			break;
		case ShaderType::Pixel:
			GetDX11Device()->GetContent()->PSSetShader((ID3D11PixelShader*)(_shaderInstance[i]), NULL, 0);
			break;
		case ShaderType::Compute:
			GetDX11Device()->GetContent()->CSSetShader((ID3D11ComputeShader*)(_shaderInstance[i]), NULL, 0);
			break;
		default:
			break;
		}
	}
}
const sShaderReflectVariable* DX11Program::GetCBVariable(const String& name)const
{
#if DC_DEBUG
	ShaderReflectVariables::const_iterator it = _shaderReflect->mCBVariables.Find(name);
	if (it != _shaderReflect->mCBVariables.end())
		return &(it->second);
	return nullptr;
#else
	return base::GetCBVariable(name);
#endif
}
bool DX11Program::SetVariable(const String& name, const Matrix4& mat)
{
	const sShaderReflectVariable* cb_var = GetCBVariable(name);
	if (cb_var != nullptr)
	{
		cbuffers_t::iterator& it = _cBBuffers.Find(cb_var->cb_name);
		if (it != _cBBuffers.end())
		{
			it->second->Copy(cb_var->offset, mat.p, cb_var->size);
		}
		return true;
	}
	return false;
}
bool DX11Program::SetVariable(const String& name, const Matrix4* mats, int count)
{
	const sShaderReflectVariable* cb_var = GetCBVariable(name);
	if (cb_var != nullptr)
	{
		cbuffers_t::iterator& it = _cBBuffers.Find(cb_var->cb_name);
		if (it != _cBBuffers.end())
		{
			it->second->Copy(cb_var->offset, mats, sizeof(Matrix4) * count);
		}
		return true;
	}
	return false;
}
bool DX11Program::SetVariable(const String& name, const float* values, int count)
{
	const sShaderReflectVariable* cb_var = GetCBVariable(name);
	if (cb_var != nullptr)
	{
		cbuffers_t::iterator it = _cBBuffers.Find(cb_var->cb_name);
		if (it != _cBBuffers.end())
		{
			it->second->Copy(cb_var->offset, values, sizeof(float) * count);
		}
		return true;
	}
	return false;
}
bool DX11Program::SetVariable(const String& name, const Vector4* values, int count)
{
	const sShaderReflectVariable* cb_var = GetCBVariable(name);
	if (cb_var != nullptr)
	{
		cbuffers_t::iterator it = _cBBuffers.Find(cb_var->cb_name);
		if (it != _cBBuffers.end())
		{
			it->second->Copy(cb_var->offset, values, sizeof(Vector4) * count);
		}
		return true;
	}
	return false;
}
bool DX11Program::SetVariable(const String& name, const int* ints, int count)
{
	const sShaderReflectVariable* cb_var = GetCBVariable(name);
	if (cb_var != nullptr)
	{
		cbuffers_t::iterator it = _cBBuffers.Find(cb_var->cb_name);
		if (it != _cBBuffers.end())
		{
			it->second->Copy(cb_var->offset, ints, sizeof(int) * count);
		}
		return true;
	}
	return false;
}
bool DX11Program::SetVariable(const String& name, const bool bools)
{
	const sShaderReflectVariable* cb_var = GetCBVariable(name);
	if (cb_var != nullptr)
	{
		cbuffers_t::iterator it = _cBBuffers.Find(cb_var->cb_name);
		if (it != _cBBuffers.end())
		{
			it->second->Copy(cb_var->offset, &bools, cb_var->size);
		}
		return true;
	}
	return false;
}
bool DX11Program::SetVariable(const String& name, const ShaderLight& light, int index)
{
	const sShaderReflectVariable* cb_var = GetCBVariable(name);
	if (cb_var != nullptr)
	{
		cbuffers_t::iterator it = _cBBuffers.Find(cb_var->cb_name);
		if (it != _cBBuffers.end())
		{
			int offset = index * sizeof(ShaderLight);
			it->second->Copy(cb_var->offset + offset, &light, sizeof(ShaderLight));
		}
		return true;
	}
	return false;
}
bool DX11Program::SetVariable(byte index, TextureUnit* tex_unit)
{
	const String& name = tex_unit->Name;
	if (name.IsEmpty())return false;

	//设置纹理
	Texture* texture = tex_unit->GetTexture();
	if (texture == nullptr)return false;
	ID3D11ShaderResourceView* texture_view = (ID3D11ShaderResourceView*)texture->GetTextureView();
	GetDX11Device()->GetContent()->PSSetShaderResources(index, 1, &texture_view);

	//纹理及采样状态
	SamplerStateKey state;
	memset(&state, 0, sizeof(SamplerStateKey));
	state.AddressMode = tex_unit->AddressMode;
	state.BorderColor = tex_unit->BorderColor;
	state.FilterType = tex_unit->FilterType;
	state.Anisotropy = tex_unit->Anisotropy;
	ID3D11SamplerState* sampler = dynamic_cast<DX11RenderContent*>(Application::GetGraphics()->GetRenderContent())->GetSamplerState(state);
	GetDX11Device()->GetContent()->PSSetSamplers(index, 1, &sampler);

	//UV缩放和偏移
	Vector4 tex_uv(tex_unit->TextureScale.x, tex_unit->TextureScale.y, tex_unit->TextureOffset.x, tex_unit->TextureOffset.y);
	SetVariable(name + "_ST", tex_uv.ptr(), 4);

	//贴图大小
	Vector4 tex_size(1.0f / texture->GetWidth(), 1.0f / texture->GetHeight(), (float)texture->GetWidth(), (float)texture->GetHeight());
	SetVariable(name + "_TexelSize", tex_size.ptr(), 4);

	return true;
}
bool DX11Program::SetVariable(byte index, const String& name, Texture* texture)
{
	//设置纹理
	ID3D11ShaderResourceView* texture_view = (ID3D11ShaderResourceView*)texture->GetTextureView();
	GetDX11Device()->GetContent()->PSSetShaderResources(index, 1, &texture_view);

	//纹理及采样状态
	SamplerStateKey state;
	memset(&state, 0, sizeof(SamplerStateKey));
	state.AddressMode = texture->GetAddressMode();
	state.BorderColor = texture->GetBorderColor();
	state.FilterType = texture->GetFilterType();
	ID3D11SamplerState* sampler = dynamic_cast<DX11RenderContent*>(Application::GetGraphics()->GetRenderContent())->GetSamplerState(state);
	GetDX11Device()->GetContent()->PSSetSamplers(index, 1, &sampler);

	//UV缩放和偏移
	Vector4 tex_uv(1, 1, 0, 0);
	SetVariable(name + "_ST", tex_uv.ptr(), 4);

	//贴图大小
	Vector4 tex_size(1.0f / texture->GetWidth(), 1.0f / texture->GetHeight(), (float)texture->GetWidth(), (float)texture->GetHeight());
	SetVariable(name + "_TexelSize", tex_size.ptr(), 4);

	return true;
}
bool DX11Program::SetPassVariable(byte index, const String& name, Texture* texture)
{
	//设置纹理
	ID3D11ShaderResourceView* texture_view = (ID3D11ShaderResourceView*)texture->GetTextureView();
	GetDX11Device()->GetContent()->PSSetShaderResources(index, 1, &texture_view);

	//UV缩放和偏移
	Vector4 tex_uv(1, 1, 0, 0);
	SetVariable(name + "_ST", tex_uv.ptr(), 4);

	//贴图大小
	Vector4 tex_size(1.0f / texture->GetWidth(), 1.0f / texture->GetHeight(), (float)texture->GetWidth(), (float)texture->GetHeight());
	SetVariable(name + "_TexelSize", tex_size.ptr(), 4);

	return true;
}
DC_END_NAMESPACE


/*
渲染管线概述(渲染流水线)
输入装配阶段(Iniiput Assembler(IA)Stage)
		|
		|
顶点着色阶段(Vertex Shader(VS)Stage)
		|
		|
外壳着色阶段(Hull Shader(HS)Stage)
		|
		|
曲面细分阶段(Tessellator Stage)
		|
		|
域着色阶段(Domain Shader(DS)Stage)
		|
		|
几何着色阶段(Geometry Shader(GS)Stage)
		|
		|
光栅化阶段(Rasterizer Stage)
		|
		|
像素着色阶段(Pixel Shader(PS)Stage)
		|
		|
输出合并阶段
*/

/*
从D3D设备可以创建出6种着色器：
方法								着色器类型				描述
ID3D11Device::CreateVertexShader	ID3D11VertexShader		顶点着色器
ID3D11Device::CreateHullShader		ID3D11HullShader		外壳着色器
ID3D11Device::CreateDomainShader	ID3D11DomainShader		域着色器
ID3D11Device::CreateComputeShader	ID3D11ComputeShader		计算着色器
ID3D11Device::CreateGeometryShader	ID3D11GeometryShader	几何着色器
ID3D11Device::CreatePixelShader		ID3D11PixelShader		像素着色器
*/