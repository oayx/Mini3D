 
/*****************************************************************************
* Author： hannibal
* Date：2009/11/26
* Description： 索引buff
*****************************************************************************/
#pragma once

#include "runtime/graphics/null/HardwareBuffer.h"
#include "DX12Define.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class DX12HardwareIndexBuffer final : public HardwareIndexBuffer
{
	friend class DX12Device;
	DEFAULT_CREATE(DX12HardwareIndexBuffer);
	FRIEND_CONSTRUCT_DESTRUCT(DX12HardwareIndexBuffer);
	DISALLOW_COPY_ASSIGN(DX12HardwareIndexBuffer);
	BEGIN_DERIVED_REFECTION_TYPE(DX12HardwareIndexBuffer, HardwareIndexBuffer)
	END_REFECTION_TYPE;

protected:
	DX12HardwareIndexBuffer();
	~DX12HardwareIndexBuffer();

public:
	virtual void* Lock(const IndexBufferDesc& desc)override;
	virtual void  Unlock()override;
	virtual void  Render()override;

protected:
	ID3D12Resource* _indexBuffer = nullptr;
};//DX12HardwareIndexBuffer
DC_END_NAMESPACE

