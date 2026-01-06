 
/*****************************************************************************
* Author： hannibal
* Date：2009/11/25
* Description： 顶点buff
*****************************************************************************/
#pragma once

#include "runtime/graphics/null/HardwareBuffer.h"
#include "DX12Define.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class DX12HardwareVertexBuffer final : public HardwareVertexBuffer
{
	friend class DX12Device;
	DEFAULT_CREATE(DX12HardwareVertexBuffer);
	FRIEND_CONSTRUCT_DESTRUCT(DX12HardwareVertexBuffer);
	DISALLOW_COPY_ASSIGN(DX12HardwareVertexBuffer);
	BEGIN_DERIVED_REFECTION_TYPE(DX12HardwareVertexBuffer, HardwareVertexBuffer)
	END_REFECTION_TYPE;

protected:
	DX12HardwareVertexBuffer();
	virtual ~DX12HardwareVertexBuffer();

public:
	virtual void* Lock(const VertexBufferDesc& desc)override;
	virtual void  Unlock(const VertexBufferDesc& desc)override;
	virtual void  Render(CGProgram* shader)override;

private:
	ID3D12Resource* _vertexBuffer[MAX_STREAM_COUNT] = { nullptr };
	D3D12_VERTEX_BUFFER_VIEW _vertexBufferViews[MAX_STREAM_COUNT];
};
DC_END_NAMESPACE

