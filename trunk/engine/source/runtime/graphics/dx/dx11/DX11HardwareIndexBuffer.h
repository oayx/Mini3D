 
/*****************************************************************************
* Author： hannibal
* Date：2009/11/26
* Description： 索引buff
*****************************************************************************/
#pragma once

#include "runtime/graphics/null/HardwareBuffer.h"
#include "DX11Define.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class DX11HardwareIndexBuffer final : public HardwareIndexBuffer
{
	friend class DX11Device;
	DEFAULT_CREATE(DX11HardwareIndexBuffer);
	FRIEND_CONSTRUCT_DESTRUCT(DX11HardwareIndexBuffer);
	DISALLOW_COPY_ASSIGN(DX11HardwareIndexBuffer);
	BEGIN_DERIVED_REFECTION_TYPE(DX11HardwareIndexBuffer, HardwareIndexBuffer)
	END_REFECTION_TYPE;

protected:
	DX11HardwareIndexBuffer();
	~DX11HardwareIndexBuffer();

public:
	virtual void* Lock(const IndexBufferDesc& desc)override;
	virtual void  Unlock()override;
	virtual void  Render()override;

protected:
	ID3D11Buffer* _indexBuffer = nullptr;
};//DX11HardwareIndexBuffer
DC_END_NAMESPACE
