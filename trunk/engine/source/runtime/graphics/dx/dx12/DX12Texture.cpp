
#include "DX12Texture.h"
#include "DX12Device.h" 
#include "DX12TempCommand.h"
#include "core/image/Image.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(DX12Texture, Texture);
DX12Texture::DX12Texture(const TextureDesc& desc)
	: base(desc)
{
	AssertEx(_textureType == TextureType::D2 || _textureType == TextureType::Cube, "");
	if (_enableMips)
	{
		_enableMips = GetDX12Device()->GetCaps()->IsGenerateMipsSupported(_textureType, _imageFormat);
	}
	if (this->IsStaging())
	{
		_enableMips = false;
		AssertEx(_textureType != TextureType::Cube, "");
	}
	this->CreateDescriptorHeaps();
	this->CreateTexture();
	this->CreateTextureView();

}
DX12Texture::~DX12Texture()
{
	SAFE_RELEASE(_colorTexture);
	SAFE_RELEASE(_colorTextureView);

}
bool DX12Texture::Fill(Image* image)
{
	uint num_subresources = 1;
	if(_enableMips && _textureType == TextureType::D2) 
		num_subresources = (uint)image->MipLevels();
	else if (_textureType == TextureType::Cube)
		num_subresources = 6;
	uint upload_buffer_size = GetRequiredIntermediateSize(_colorTexture, 0, num_subresources);
	ID3D12Resource* uploadBuffer = NULL;
	auto properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto resource_desc = CD3DX12_RESOURCE_DESC::Buffer(upload_buffer_size);
	DX_ERROR(GetDX12Device()->GetDevice()->CreateCommittedResource(&properties, D3D12_HEAP_FLAG_NONE, &resource_desc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, IID_PPV_ARGS(&uploadBuffer)));

	std::unique_ptr<D3D12_SUBRESOURCE_DATA[]> resource_data(new D3D12_SUBRESOURCE_DATA[num_subresources]);
	for (int i = 0; i < num_subresources; ++i)
	{
		ImageMipData* mip_data = image->GetMipData(i);
		_totalBytes = mip_data->Size;
		resource_data[i].pData = (const void*)mip_data->Data;
		resource_data[i].RowPitch = static_cast<UINT>(mip_data->RowPitch);
		resource_data[i].SlicePitch = static_cast<UINT>(mip_data->Size);
	}

	{
		DX12TempCommand command;
		auto resource_barrier = CD3DX12_RESOURCE_BARRIER::Transition(_colorTexture, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
		command.cmd_list->ResourceBarrier(1, &resource_barrier);
		UpdateSubresources(command.cmd_list, _colorTexture, uploadBuffer, 0, 0, num_subresources, resource_data.get());
		resource_barrier = CD3DX12_RESOURCE_BARRIER::Transition(_colorTexture, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		command.cmd_list->ResourceBarrier(1, &resource_barrier);
	}

	SAFE_RELEASE(uploadBuffer);

	return true;
}
byte* DX12Texture::Lock(TextureLockDesc& lock_desc)
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
void DX12Texture::Unlock(const TextureLockDesc& lock_desc)
{
	if (_textureType != TextureType::D2 || !this->IsDynamic())
		return;

	uint num_subresources = _textureType == TextureType::Cube ? 6 : 1;
	uint upload_buffer_size = GetRequiredIntermediateSize(_colorTexture, 0, num_subresources);
	ID3D12Resource* uploadBuffer = NULL;
	auto heap_properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto resource_desc = CD3DX12_RESOURCE_DESC::Buffer(upload_buffer_size);
	DX_ERROR(GetDX12Device()->GetDevice()->CreateCommittedResource(&heap_properties, D3D12_HEAP_FLAG_NONE, &resource_desc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, IID_PPV_ARGS(&uploadBuffer)));

	std::unique_ptr<D3D12_SUBRESOURCE_DATA[]> resource_data(NEW D3D12_SUBRESOURCE_DATA[1]);
	{
		resource_data[0].pData = (const void*)_imageData;
		resource_data[0].RowPitch = static_cast<UINT>(this->GetPitch());
		resource_data[0].SlicePitch = static_cast<UINT>(this->GetBytes());
	}

	{
		DX12TempCommand command;
		auto resource_barrier = CD3DX12_RESOURCE_BARRIER::Transition(_colorTexture, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
		command.cmd_list->ResourceBarrier(1, &resource_barrier);
		UpdateSubresources(command.cmd_list, _colorTexture, uploadBuffer, 0, 0, num_subresources, resource_data.get());
		resource_barrier = CD3DX12_RESOURCE_BARRIER::Transition(_colorTexture, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		command.cmd_list->ResourceBarrier(1, &resource_barrier);
	}

	SAFE_RELEASE(uploadBuffer);
}
void DX12Texture::CreateDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC srv_heap_desc = {};
	srv_heap_desc.NumDescriptors = 1;
	srv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	DX_ERROR(GetDX12Device()->GetDevice()->CreateDescriptorHeap(&srv_heap_desc, IID_PPV_ARGS(&_colorTextureView)));
}
void DX12Texture::CreateTexture()
{
	D3D12_RESOURCE_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Alignment = 0;
	desc.Width = _imageWidth;
	desc.Height = _imageHeight;
	desc.DepthOrArraySize = _textureType == TextureType::Cube ? 6 : 1;
	desc.MipLevels = _enableMips ? 0 : 1;
	desc.Format = DX10GetTextureFormat(_imageFormat, this->IsSRGB());
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	desc.Flags = D3D12_RESOURCE_FLAG_NONE;
	auto heap_properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	DX_ERROR(GetDX12Device()->GetDevice()->CreateCommittedResource(&heap_properties, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_COMMON, NULL, IID_PPV_ARGS(&_colorTexture)));
}
void DX12Texture::CreateTextureView()
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DX10GetTextureFormat(_imageFormat, this->IsSRGB());
	srvDesc.ViewDimension = _textureType == TextureType::Cube ? D3D12_SRV_DIMENSION_TEXTURECUBE : D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = _enableMips ? -1 : 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	GetDX12Device()->GetDevice()->CreateShaderResourceView(_colorTexture, &srvDesc, _colorTextureView->GetCPUDescriptorHandleForHeapStart());
}
DC_END_NAMESPACE

