#include "DX11HardwareIndexBuffer.h"
#include "DX11Device.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(DX11HardwareIndexBuffer, HardwareIndexBuffer);
DX11HardwareIndexBuffer::DX11HardwareIndexBuffer()
{
}
DX11HardwareIndexBuffer::~DX11HardwareIndexBuffer()
{
	SAFE_RELEASE(_indexBuffer);
}
void* DX11HardwareIndexBuffer::Lock(const IndexBufferDesc& desc)
{
	bool rebuild = this->NeedRebuild(desc);

	base::Lock(desc);
	if (_indexCount == 0)
		return nullptr;

	if (!_indexBuffer || rebuild)
	{
		SAFE_RELEASE(_indexBuffer);

		D3D11_BUFFER_DESC sd = {};
		sd.ByteWidth = this->GetBufferSize();
		sd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		sd.Usage = D3D11_USAGE_DEFAULT;
		switch (_usage)
		{
		case GPUResourceUsage::Static:
			sd.Usage = D3D11_USAGE_DEFAULT;
			sd.CPUAccessFlags = 0;
			break;
		case GPUResourceUsage::Dynamic:
			sd.Usage = D3D11_USAGE_DYNAMIC;
			sd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			break;
		default:
			AssertEx(false, "error usage:%d", (int)_usage);
			break;
		}
		HR(GetDX11Device()->GetDevice()->CreateBuffer(&sd, nullptr, &_indexBuffer));
	}

	if (_usage == GPUResourceUsage::Dynamic)
	{
		D3D11_MAPPED_SUBRESOURCE res = { };
		HR(GetDX11Device()->GetContent()->Map(_indexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res));
		return res.pData;
	}
	else
	{
		if (!_bufferData || rebuild)
		{
			DeleteArray(_bufferData);
			_bufferData = NewArray<byte>(this->GetBufferCapacity());
			::memset(_bufferData, 0, this->GetBufferCapacity());
		}
		return _bufferData;
	}
}
void  DX11HardwareIndexBuffer::Unlock()
{
	base::Unlock();
	if (_indexCount == 0)
		return;

	if (_usage == GPUResourceUsage::Dynamic)
	{
		GetDX11Device()->GetContent()->Unmap(_indexBuffer, 0);
	}
	else
	{
		D3D11_BOX box = { 0, 0, 0, this->GetBufferSize(), 1, 1 };
		GetDX11Device()->GetContent()->UpdateSubresource(_indexBuffer, 0, &box, _bufferData, 0, 0);
	}
}
void DX11HardwareIndexBuffer::Render()
{
	UINT offset = 0;
	GetDX11Device()->GetContent()->IASetIndexBuffer(_indexBuffer, DX11GetIndexType(_indexType), 0);
}
DC_END_NAMESPACE
