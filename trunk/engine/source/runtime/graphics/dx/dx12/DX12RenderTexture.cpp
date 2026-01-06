
#include "DX12RenderTexture.h"
#include "DX12Device.h"
#include "runtime/input/Input.h"
#include "runtime/Application.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(DX12RenderTexture, RenderTexture);
DX12RenderTexture::DX12RenderTexture(const TextureDesc& desc)
	: base(desc)
{
	if (_flags & TextureFlag::COLOR)
	{
		this->CreateRenderTexture();
	}
	else if (_flags & TextureFlag::DEPTH || _flags & TextureFlag::STENCIL)
	{
		this->CreateDepthStencilTexture();
	}
}
DX12RenderTexture::~DX12RenderTexture()
{
	ZoneScoped;
	SAFE_RELEASE(_colorTexture);
	SAFE_RELEASE(_colorTextureView);
	SAFE_RELEASE(_renderTargetView);
	SAFE_RELEASE(_depthStencilView);
}
void DX12RenderTexture::PreRender()
{
	D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON;
	if (_flags & TextureFlag::COLOR)
		state = D3D12_RESOURCE_STATE_RENDER_TARGET;
	else if (_flags & TextureFlag::DEPTH || _flags & TextureFlag::STENCIL)
		state = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(_colorTexture, D3D12_RESOURCE_STATE_GENERIC_READ, state);
	GetDX12Device()->GetCommandList()->ResourceBarrier(1, &barrier);
}
void DX12RenderTexture::BeginFrame(RenderFrameDesc& desc)
{
	AssertEx(desc.face < (_textureType == TextureType::Cube ? 6 : 1), "");

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
	if (_renderTargetView != nullptr)
	{
		rtvHandle = _renderTargetView->GetCPUDescriptorHandleForHeapStart();
		rtvHandle.ptr += desc.face * GetDX12Device()->GetRtvDescriptorSize();
	}
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle;
	if (_depthStencilView != nullptr)
	{
		dsvHandle = _depthStencilView->GetCPUDescriptorHandleForHeapStart();
	}

	desc.viewPort.Set((float)_imageWidth, (float)_imageHeight);
	desc.targetBuffer = _renderTargetView ? &rtvHandle : nullptr;
	desc.depthStencilBuffer = _depthStencilView ? &dsvHandle : nullptr;
	base::BeginFrame(desc);
}
void DX12RenderTexture::PostRender()
{
	D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON;
	if (_flags & TextureFlag::COLOR)
		state = D3D12_RESOURCE_STATE_RENDER_TARGET;
	else if (_flags & TextureFlag::DEPTH || _flags & TextureFlag::STENCIL)
		state = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(_colorTexture,state, D3D12_RESOURCE_STATE_GENERIC_READ);
	GetDX12Device()->GetCommandList()->ResourceBarrier(1, &barrier);
}
void DX12RenderTexture::CreateRenderTexture()
{
	{//heap
		D3D12_DESCRIPTOR_HEAP_DESC srv_heap_desc = {};
		srv_heap_desc.NumDescriptors = 1;
		srv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		DX_ERROR(GetDX12Device()->GetDevice()->CreateDescriptorHeap(&srv_heap_desc, IID_PPV_ARGS(&_colorTextureView)));

		D3D12_DESCRIPTOR_HEAP_DESC rtv_heap_desc = {};
		rtv_heap_desc.NumDescriptors = _textureType == TextureType::Cube ? 6 : 1;
		rtv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		rtv_heap_desc.NodeMask = 0;
		DX_ERROR(GetDX12Device()->GetDevice()->CreateDescriptorHeap(&rtv_heap_desc, IID_PPV_ARGS(&_renderTargetView)));
	}
	{//texture
		D3D12_RESOURCE_DESC tex_desc = {};
		tex_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		tex_desc.Alignment = 0;
		tex_desc.Width = _imageWidth;
		tex_desc.Height = _imageHeight;
		tex_desc.Format = DX10GetTextureFormat(_imageFormat, false);
		tex_desc.DepthOrArraySize = _textureType == TextureType::Cube ? 6 : 1;
		tex_desc.MipLevels = 1;
		tex_desc.SampleDesc.Count = 1;
		tex_desc.SampleDesc.Quality = 0;
		tex_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		tex_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		auto properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		DX_ERROR(GetDX12Device()->GetDevice()->CreateCommittedResource(
			&properties,
			D3D12_HEAP_FLAG_NONE,
			&tex_desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&_colorTexture)));

		D3D12_SHADER_RESOURCE_VIEW_DESC srv_view_desc = {};
		srv_view_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srv_view_desc.Format = DX10GetTextureViewFormat(_imageFormat, false);
		if (_textureType == TextureType::Cube)
		{
			srv_view_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
			srv_view_desc.TextureCube.MostDetailedMip = 0;
			srv_view_desc.TextureCube.MipLevels = 1;
			srv_view_desc.TextureCube.ResourceMinLODClamp = 0.0f;
		}
		else
		{
			srv_view_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			srv_view_desc.Texture2D.MostDetailedMip = 0;
			srv_view_desc.Texture2D.MipLevels = 1;
			srv_view_desc.Texture2D.ResourceMinLODClamp = 0.0f;
			srv_view_desc.Texture2D.PlaneSlice = 0;
		}
		GetDX12Device()->GetDevice()->CreateShaderResourceView(_colorTexture, &srv_view_desc, _colorTextureView->GetCPUDescriptorHandleForHeapStart());
	}
	{//target
		D3D12_RENDER_TARGET_VIEW_DESC rtv_view_desc = {};
		rtv_view_desc.Format = DX10GetTextureViewFormat(_imageFormat, false);
		rtv_view_desc.ViewDimension = _textureType == TextureType::Cube ? D3D12_RTV_DIMENSION_TEXTURE2DARRAY : D3D12_RTV_DIMENSION_TEXTURE2D;
		rtv_view_desc.Texture2DArray.MipSlice = 0;
		auto rtvCpuStart = _renderTargetView->GetCPUDescriptorHandleForHeapStart();
		if (_textureType == TextureType::Cube)
		{
			CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandles[6];
			for (int i = 0; i < 6; ++i)rtvHandles[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvCpuStart, i, GetDX12Device()->GetRtvDescriptorSize());

			for (int i = 0; i < 6; ++i)
			{
				rtv_view_desc.Texture2DArray.PlaneSlice = 0;
				rtv_view_desc.Texture2DArray.FirstArraySlice = i;
				rtv_view_desc.Texture2DArray.ArraySize = 1;
				GetDX12Device()->GetDevice()->CreateRenderTargetView(_colorTexture, &rtv_view_desc, rtvHandles[i]);
			}
		}
		else
		{
			GetDX12Device()->GetDevice()->CreateRenderTargetView(_colorTexture, &rtv_view_desc, rtvCpuStart);
		}
	}
}
void DX12RenderTexture::CreateDepthStencilTexture()
{
	{//heap
		D3D12_DESCRIPTOR_HEAP_DESC srv_heap_desc = {};
		srv_heap_desc.NumDescriptors = 1;
		srv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		DX_ERROR(GetDX12Device()->GetDevice()->CreateDescriptorHeap(&srv_heap_desc, IID_PPV_ARGS(&_colorTextureView)));

		D3D12_DESCRIPTOR_HEAP_DESC dsv_heap_desc = {};
		dsv_heap_desc.NumDescriptors = 1;
		dsv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		dsv_heap_desc.NodeMask = 0;
		DX_ERROR(GetDX12Device()->GetDevice()->CreateDescriptorHeap(&dsv_heap_desc, IID_PPV_ARGS(&_depthStencilView)));
	}
	{//texture
		D3D12_RESOURCE_DESC tex_desc = {};
		tex_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		tex_desc.Alignment = 0;
		tex_desc.Width = _imageWidth;
		tex_desc.Height = _imageHeight;
		tex_desc.Format = DX10GetTextureFormat(_imageFormat, false);
		tex_desc.MipLevels = 1;
		tex_desc.DepthOrArraySize = 1;
		tex_desc.SampleDesc.Count = 1;
		tex_desc.SampleDesc.Quality = 0;
		tex_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		tex_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		D3D12_CLEAR_VALUE opt_clear = {};
		opt_clear.Format = DX10GetTextureViewFormat(_imageFormat, false);
		opt_clear.DepthStencil.Depth = 1.0f;
		opt_clear.DepthStencil.Stencil = 0;
		auto properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		DX_ERROR(GetDX12Device()->GetDevice()->CreateCommittedResource(
			&properties,
			D3D12_HEAP_FLAG_NONE,
			&tex_desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			&opt_clear,
			IID_PPV_ARGS(&_colorTexture)));

		D3D12_SHADER_RESOURCE_VIEW_DESC srv_view_desc = {};
		srv_view_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srv_view_desc.Format = DX10GetShaderViewFormat(_imageFormat, false);
		srv_view_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srv_view_desc.Texture2D.MostDetailedMip = 0;
		srv_view_desc.Texture2D.MipLevels = 1;
		srv_view_desc.Texture2D.ResourceMinLODClamp = 0.0f;
		srv_view_desc.Texture2D.PlaneSlice = 0;
		GetDX12Device()->GetDevice()->CreateShaderResourceView(_colorTexture, &srv_view_desc, _colorTextureView->GetCPUDescriptorHandleForHeapStart());
	}
	{//depth stencil
		D3D12_DEPTH_STENCIL_VIEW_DESC dsv_view_desc = {};
		dsv_view_desc.Flags = D3D12_DSV_FLAG_NONE;
		dsv_view_desc.Format = DX10GetTextureViewFormat(_imageFormat, false);
		dsv_view_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsv_view_desc.Texture2D.MipSlice = 0;
		GetDX12Device()->GetDevice()->CreateDepthStencilView(_colorTexture, &dsv_view_desc, _depthStencilView->GetCPUDescriptorHandleForHeapStart());
	}
}
DC_END_NAMESPACE

