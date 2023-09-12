#include "DX9HardwareIndexBuffer.h"
#include "DX9Device.h"


DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(DX9HardwareIndexBuffer, HardwareIndexBuffer);
DX9HardwareIndexBuffer::DX9HardwareIndexBuffer()
{
	GetDX9Device()->AddEventListener(LostDeviceEvt, this, MakeEventHandler(DX9HardwareIndexBuffer::HandleLostDevice));
	GetDX9Device()->AddEventListener(ResetDeviceEvt, this, MakeEventHandler(DX9HardwareIndexBuffer::HandleResetDevice));
}
DX9HardwareIndexBuffer::~DX9HardwareIndexBuffer()
{
	GetDX9Device()->RemoveEventListener(LostDeviceEvt, this, MakeEventHandler(DX9HardwareIndexBuffer::HandleLostDevice));
	GetDX9Device()->RemoveEventListener(ResetDeviceEvt, this, MakeEventHandler(DX9HardwareIndexBuffer::HandleResetDevice));
	SAFE_RELEASE(_indexBuffer);
}
void* DX9HardwareIndexBuffer::Lock(const IndexBufferDesc& desc)
{
	bool rebuild = this->NeedRebuild(desc);

	base::Lock(desc);
	if (_indexCount == 0)
		return nullptr;

	if(!_indexBuffer || rebuild)
	{
		SAFE_RELEASE(_indexBuffer);
		DX9HR(GetDX9Device()->GetDevice()->CreateIndexBuffer(
			this->GetBufferCapacity(),
			DX9GetResourceUsage(_usage),
			DX9GetIndexType(_indexType),
			DX9GetResourcePool(_pool),
			&_indexBuffer,
			NULL));
	}
	//由于需要处理设备丢失，这里不用Lock，而是分配一块内存；设备恢复时直接使用这块内存
	if (rebuild)
	{
		DeleteArray(_bufferData);
		_bufferData = NewArray<byte>(this->GetBufferCapacity());
	}
	return _bufferData;
}
void  DX9HardwareIndexBuffer::Unlock()
{
	base::Unlock();
	if (_indexCount == 0)
		return;
	
	void *buffer = NULL;
	DX9HR(_indexBuffer->Lock(
		0,
		this->GetBufferSize(),
		&buffer,
		(_usage == GPUResourceUsage::Dynamic) ? D3DLOCK_DISCARD : 0
	));
	Memory::Copy(buffer, _bufferData, this->GetBufferSize());
	DX9HR(_indexBuffer->Unlock());
}
void DX9HardwareIndexBuffer::Render()
{
	if (_indexBuffer)
	{
		DX9HR(GetDX9Device()->GetDevice()->SetIndices(_indexBuffer));
	}
}
void DX9HardwareIndexBuffer::HandleLostDevice()
{
	SAFE_RELEASE(_indexBuffer);
}
void DX9HardwareIndexBuffer::HandleResetDevice()
{
	DX9HR(GetDX9Device()->GetDevice()->CreateIndexBuffer(
		this->GetBufferSize(),
		DX9GetResourceUsage(_usage),
		DX9GetIndexType(_indexType),
		DX9GetResourcePool(_pool),
		&_indexBuffer,
		NULL));

	{
		void *buffer = NULL;
		DX9HR(_indexBuffer->Lock(
			0,
			this->GetBufferSize(),
			&buffer,
			(_usage == GPUResourceUsage::Dynamic) ? D3DLOCK_DISCARD : 0
		));
		Memory::Copy(buffer, _bufferData,this->GetBufferSize());
		DX9HR(_indexBuffer->Unlock());
	}
}
DC_END_NAMESPACE