
/*****************************************************************************
* Author： hannibal
* Date：2009年11月26日
* Description： 索引buff
*****************************************************************************/
#pragma once

#include "runtime/graphics/null/HardwareBuffer.h"
#include "DX9Define.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class DX9HardwareIndexBuffer final : public HardwareIndexBuffer
{
	friend class DX9Device;
	DEFAULT_CREATE(DX9HardwareIndexBuffer);
	FRIEND_CONSTRUCT_DESTRUCT(DX9HardwareIndexBuffer);
	DISALLOW_COPY_ASSIGN(DX9HardwareIndexBuffer);
	BEGIN_DERIVED_REFECTION_TYPE(DX9HardwareIndexBuffer, HardwareIndexBuffer)
	END_REFECTION_TYPE;

protected:
	DX9HardwareIndexBuffer();
	~DX9HardwareIndexBuffer();

	void HandleLostDevice();
	void HandleResetDevice();

public:
	virtual void* Lock(const IndexBufferDesc& desc)override;
	virtual void  Unlock()override;
	virtual void  Render()override;

protected:
	IDirect3DIndexBuffer9* _indexBuffer = nullptr;
};//DX9HardwareIndexBuffer
DC_END_NAMESPACE
