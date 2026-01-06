
#include "DX12HardwareIndexBuffer.h"
#include "DX12Device.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(DX12HardwareIndexBuffer, HardwareIndexBuffer);
DX12HardwareIndexBuffer::DX12HardwareIndexBuffer()
{
}
DX12HardwareIndexBuffer::~DX12HardwareIndexBuffer()
{
	SAFE_RELEASE(_indexBuffer);
}
void* DX12HardwareIndexBuffer::Lock(const IndexBufferDesc& desc)
{
	bool rebuild = this->NeedRebuild(desc);

	base::Lock(desc);
	if (_indexCount == 0)
		return nullptr;

	if (!_indexBuffer || rebuild)
	{
		SAFE_RELEASE(_indexBuffer);

		D3D12_HEAP_PROPERTIES props;
		memset(&props, 0, sizeof(D3D12_HEAP_PROPERTIES));
		props.Type = D3D12_HEAP_TYPE_UPLOAD;
		props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

		D3D12_RESOURCE_DESC sd;
		memset(&sd, 0, sizeof(D3D12_RESOURCE_DESC));
		sd.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		sd.Width = this->GetBufferSize();
		sd.Height = 1;
		sd.DepthOrArraySize = 1;
		sd.MipLevels = 1;
		sd.Format = DXGI_FORMAT_UNKNOWN;
		sd.SampleDesc.Count = 1;
		sd.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		sd.Flags = D3D12_RESOURCE_FLAG_NONE;
		DX_ERROR(GetDX12Device()->GetDevice()->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &sd, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, IID_PPV_ARGS(&_indexBuffer)));
	}

	void* buffer = nullptr;
	DX_ERROR(_indexBuffer->Map(0, nullptr, &buffer));
	return buffer;
}
void DX12HardwareIndexBuffer::Unlock()
{
	base::Unlock();
	if (_indexCount == 0)
		return;
	_indexBuffer->Unmap(0, nullptr);
}
void DX12HardwareIndexBuffer::Render()
{
	D3D12_INDEX_BUFFER_VIEW ibv;
	memset(&ibv, 0, sizeof(D3D12_INDEX_BUFFER_VIEW));
	ibv.BufferLocation = _indexBuffer->GetGPUVirtualAddress();
	ibv.SizeInBytes = this->GetBufferSize();
	ibv.Format = DX10GetIndexType(_indexType);
	GetDX12Device()->GetCommandList()->IASetIndexBuffer(&ibv);
}
DC_END_NAMESPACE

