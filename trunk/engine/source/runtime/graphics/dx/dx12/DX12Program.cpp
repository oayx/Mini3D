
#include "DX12Program.h"
#include "DX12Device.h"
#include "DX12RenderContent.h"
#include "DX12ShaderReflect.h"
#include "runtime/graphics/TextureUnit.h"
#include "runtime/graphics/null/Texture.h"
#include "runtime/graphics/null/RenderTexture.h"
#include "runtime/input/Input.h"
#include "runtime/Application.h"
#include "external/tinyxml2/tinyxml2.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
DX12CBuffer::DX12CBuffer(const sShaderReflectCBuffer& cbuffer)
	: info(cbuffer)
{
	D3D12_DESCRIPTOR_HEAP_DESC cbv_desc;
	cbv_desc.NumDescriptors = 1;
	cbv_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbv_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbv_desc.NodeMask = 0;
	DX_ERROR(GetDX12Device()->GetDevice()->CreateDescriptorHeap(&cbv_desc, IID_PPV_ARGS(&cbv_heap)));

	auto properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto resource_desc = CD3DX12_RESOURCE_DESC::Buffer(DX12CalcConstantBufferByteSize(info.size));
	DX_ERROR(GetDX12Device()->GetDevice()->CreateCommittedResource(
		&properties,
		D3D12_HEAP_FLAG_NONE,
		&resource_desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&d3d_buffer)));
	DX_ERROR(d3d_buffer->Map(0, nullptr, reinterpret_cast<void**>(&buffer)));

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_view_desc;
	cbv_view_desc.BufferLocation = d3d_buffer->GetGPUVirtualAddress();
	cbv_view_desc.SizeInBytes = DX12CalcConstantBufferByteSize(info.size);
	GetDX12Device()->GetDevice()->CreateConstantBufferView(&cbv_view_desc, cbv_heap->GetCPUDescriptorHandleForHeapStart());
}
DX12CBuffer::~DX12CBuffer()
{
	if (d3d_buffer != nullptr)
	{
		d3d_buffer->Unmap(0, nullptr);
		SAFE_RELEASE(d3d_buffer);
	}
	SAFE_RELEASE(cbv_heap);
}	
void DX12CBuffer::Update()
{
	ID3D12DescriptorHeap* descriptorHeaps[] = { cbv_heap };
	GetDX12Device()->GetCommandList()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	GetDX12Device()->GetCommandList()->SetGraphicsRootConstantBufferView(info.slot + GetDX12Device()->GetStartCBufferDescriptor(), d3d_buffer->GetGPUVirtualAddress());
}
void DX12CBuffer::Copy(uint offset, const void* src_data, uint size)
{
	::memcpy(buffer + offset, src_data, size);
	dirty = true;
}
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(DX12Program, CGProgram);
DX12Program::DX12Program()
{
#if DC_DEBUG
	_shaderReflect = DX12ShaderReflect::Create();
#endif
	for (int i = 0; i < (int)ShaderType::Max; ++i)
	{
		_codeBuffer[i] = nullptr;
	}

	Application::GetGraphics()->GetRenderContent()->AddEventListener(PreCommandEvt, this, MakeEventHandler(DX12Program::OnPreCommandEvt));
}
DX12Program::~DX12Program()
{
	Application::GetGraphics()->GetRenderContent()->RemoveEventListener(PreCommandEvt, this, MakeEventHandler(DX12Program::OnPreCommandEvt));
#if DC_DEBUG
	SAFE_DELETE(_shaderReflect);
#endif
	for (int i = 0; i < (int)ShaderType::Max; ++i)
	{
		SAFE_RELEASE(_codeBuffer[i]);
	}
	for (auto obj : _cBBuffers)
	{
		for (auto buffer : obj)
		{
			SAFE_DELETE(buffer.second);
		}
	}
	_cBBuffers.Clear();
}
bool DX12Program::LoadFromFile(const String& file)
{
	ShaderDesc info;
	info.ShaderFile[int(ShaderType::Vertex)] = file;
	info.ShaderFile[int(ShaderType::Pixel)] = file;
	return LoadFromDesc(info);
}
bool DX12Program::LoadFromDesc(const ShaderDesc& shader_info)
{
	base::LoadFromDesc(shader_info);

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
		if (!DX10CompileShaderFromFile(file_path, shader_info.ShaderDefines, enter_point, target, &_codeBuffer[i]))
		{
			result = false;
			continue;
		}
		CreateShader(shader_type);
	}
	CreateCBuffer();

	return result;
}
bool DX12Program::LoadFromMemory(const String& name, const VecString& codes, const VecString& defines)
{
	base::LoadFromMemory(name, codes, defines);
	bool result = true;
	for (int i = 0; i < codes.Size(); ++i)
	{
		const String& code = codes[i];
		if (code.IsEmpty())continue;

		ShaderType shader_type = ShaderType(i);
		const String& shader_version = Application::GetGraphics()->GetShaderVersion();
		String enter_point = ShaderEnterEnum[i];
		String target = DXGetShaderTarget(shader_type, shader_version);
		if (!DX10CompileShader(name, code, defines, enter_point, target, &_codeBuffer[i]))
		{
			result = false;
			break;
		}
		CreateShader(shader_type);
	}
	CreateCBuffer();

	return result;
}
void DX12Program::PreRender()
{
	_currentCBIndex++;
	if (_cBBuffers.Size() <= _currentCBIndex)
	{
		this->CreateCBuffer();
	}
}
void DX12Program::PostRender()
{
	auto cbuffer = _cBBuffers[_currentCBIndex];
	for (const auto& cb : cbuffer)
	{
		cb.second->Update();
	}
}
void DX12Program::CreateShader(ShaderType type)
{
	ID3DBlob* shader_code = _codeBuffer[(int)type];

#if DC_DEBUG
	_shaderReflect->Reflect(type, shader_code);
#endif
}
void DX12Program::CreateCBuffer()
{
	cbuffers_t cbuffer;
#if DC_DEBUG
	for (const auto& obj : _shaderReflect->mCBBuffers)
#else
	for (const auto& obj : _reflectCBuffers)
#endif
	{
		const sShaderReflectCBuffer& buffer_info = obj.second;
		DX12CBuffer* d3d_cbuffer = DX12CBuffer::Create(buffer_info);
		AssertEx(cbuffer.Add(buffer_info.name, d3d_cbuffer),"");
	}
	_cBBuffers.Add(cbuffer);
}
void DX12Program::OnPreCommandEvt()
{
	_currentCBIndex = -1;
}
const sShaderReflectVariable* DX12Program::GetCBVariable(const String& name)const
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
DX12CBuffer* DX12Program::GetCBuffer(const String& name)const
{
	auto cbuffer = _cBBuffers[_currentCBIndex];
	auto it = cbuffer.Find(name);
	if (it != cbuffer.end())
	{
		return it->second;
	}
	return nullptr;
}
bool DX12Program::SetVariable(const String& name, const Matrix4& mat)
{
	const sShaderReflectVariable* cb_var = GetCBVariable(name);
	if (cb_var != nullptr)
	{
		DX12CBuffer* buffer = GetCBuffer(cb_var->cb_name);
		if (buffer != nullptr)
		{
			buffer->Copy(cb_var->offset, mat.p, cb_var->size);
			return true;
		}
	}
	return false;
}
bool DX12Program::SetVariable(const String& name, const Matrix4* mats, int count)
{
	const sShaderReflectVariable* cb_var = GetCBVariable(name);
	if (cb_var != nullptr)
	{
		DX12CBuffer* buffer = GetCBuffer(cb_var->cb_name);
		if (buffer != nullptr)
		{
			buffer->Copy(cb_var->offset, mats, sizeof(Matrix4) * count);
			return true;
		}
	}
	return false;
}
bool DX12Program::SetVariable(const String& name, const float* values, int count)
{
	const sShaderReflectVariable* cb_var = GetCBVariable(name);
	if (cb_var != nullptr)
	{
		DX12CBuffer* buffer = GetCBuffer(cb_var->cb_name);
		if (buffer != nullptr)
		{
			buffer->Copy(cb_var->offset, values, sizeof(float) * count);
			return true;
		}
	}
	return false;
}
bool DX12Program::SetVariable(const String& name, const Vector4* values, int count)
{
	const sShaderReflectVariable* cb_var = GetCBVariable(name);
	if (cb_var != nullptr)
	{
		DX12CBuffer* buffer = GetCBuffer(cb_var->cb_name);
		if (buffer != nullptr)
		{
			buffer->Copy(cb_var->offset, values, sizeof(Vector4) * count);
			return true;
		}
	}
	return false;
}
bool DX12Program::SetVariable(const String& name, const int* ints, int count)
{
	const sShaderReflectVariable* cb_var = GetCBVariable(name);
	if (cb_var != nullptr)
	{
		DX12CBuffer* buffer = GetCBuffer(cb_var->cb_name);
		if (buffer != nullptr)
		{
			buffer->Copy(cb_var->offset, ints, sizeof(int) * count);
			return true;
		}
	}
	return false;
}
bool DX12Program::SetVariable(const String& name, const bool bools)
{
	const sShaderReflectVariable* cb_var = GetCBVariable(name);
	if (cb_var != nullptr)
	{
		DX12CBuffer* buffer = GetCBuffer(cb_var->cb_name);
		if (buffer != nullptr)
		{
			buffer->Copy(cb_var->offset, &bools, cb_var->size);
		}
		return true;
	}
	return false;
}
bool DX12Program::SetVariable(const String& name, const ShaderLight& light, int index)
{
	const sShaderReflectVariable* cb_var = GetCBVariable(name);
	if (cb_var != nullptr)
	{
		DX12CBuffer* buffer = GetCBuffer(cb_var->cb_name);
		if (buffer != nullptr)
		{
			int offset = index * sizeof(ShaderLight);
			buffer->Copy(cb_var->offset + offset, &light, sizeof(ShaderLight));
		}
		return true;
	}
	return false;
}
bool DX12Program::SetVariable(byte index, TextureUnit* tex_unit)
{
	const String& name = tex_unit->Name;
	if (name.IsEmpty())return false;

	//设置纹理
	Texture* texture = tex_unit->GetTexture();
	if (texture == nullptr)return false;
	ID3D12DescriptorHeap* texture_view = (ID3D12DescriptorHeap*)texture->GetTextureView();
	ID3D12DescriptorHeap* descriptorHeaps[] = { texture_view };
	GetDX12Device()->GetCommandList()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	CD3DX12_GPU_DESCRIPTOR_HANDLE tex(texture_view->GetGPUDescriptorHandleForHeapStart());
	GetDX12Device()->GetCommandList()->SetGraphicsRootDescriptorTable(index + GetDX12Device()->GetStartTextureDescriptor(), tex);

	//纹理及采样状态

	//UV缩放和偏移
	Vector4 tex_uv(tex_unit->TextureScale.x, tex_unit->TextureScale.y, tex_unit->TextureOffset.x, tex_unit->TextureOffset.y);
	SetVariable(name + "_ST", tex_uv.ptr(), 4);

	//贴图大小
	Vector4 tex_size(1.0f / texture->GetWidth(), 1.0f / texture->GetHeight(), (float)texture->GetWidth(), (float)texture->GetHeight());
	SetVariable(name + "_TexelSize", tex_size.ptr(), 4);

	return true;
}
bool DX12Program::SetVariable(byte index, const String& name, Texture* texture)
{
	//设置纹理
	ID3D12DescriptorHeap* texture_view = (ID3D12DescriptorHeap*)texture->GetTextureView();
	ID3D12DescriptorHeap* descriptorHeaps[] = { texture_view };
	GetDX12Device()->GetCommandList()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	CD3DX12_GPU_DESCRIPTOR_HANDLE tex(texture_view->GetGPUDescriptorHandleForHeapStart());
	GetDX12Device()->GetCommandList()->SetGraphicsRootDescriptorTable(index + GetDX12Device()->GetStartTextureDescriptor(), tex);

	//纹理及采样状态

	//UV缩放和偏移
	Vector4 tex_uv(1, 1, 0, 0);
	SetVariable(name + "_ST", tex_uv.ptr(), 4);

	//贴图大小
	Vector4 tex_size(1.0f / texture->GetWidth(), 1.0f / texture->GetHeight(), (float)texture->GetWidth(), (float)texture->GetHeight());
	SetVariable(name + "_TexelSize", tex_size.ptr(), 4);

	return true;
}
bool DX12Program::SetPassVariable(byte index, const String& name, Texture* texture)
{
	//设置纹理
	ID3D12DescriptorHeap* texture_view = (ID3D12DescriptorHeap*)texture->GetTextureView();
	ID3D12DescriptorHeap* descriptorHeaps[] = { texture_view };
	GetDX12Device()->GetCommandList()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	CD3DX12_GPU_DESCRIPTOR_HANDLE tex(texture_view->GetGPUDescriptorHandleForHeapStart());
	GetDX12Device()->GetCommandList()->SetGraphicsRootDescriptorTable(index + GetDX12Device()->GetStartTextureDescriptor(), tex);

	//UV缩放和偏移
	Vector4 tex_uv(1, 1, 0, 0);
	SetVariable(name + "_ST", tex_uv.ptr(), 4);

	//贴图大小
	Vector4 tex_size(1.0f / texture->GetWidth(), 1.0f / texture->GetHeight(), (float)texture->GetWidth(), (float)texture->GetHeight());
	SetVariable(name + "_TexelSize", tex_size.ptr(), 4);

	return true;
}
DC_END_NAMESPACE

