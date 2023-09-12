
/*****************************************************************************
* Author： hannibal
* Date：2009年11月25日
* Description： 顶点buff
*****************************************************************************/
#pragma once

#include "runtime/graphics/null/HardwareBuffer.h"
#include "DX9Define.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class DX9HardwareVertexBuffer Final : public HardwareVertexBuffer
{
	friend class DX9Device;
	DEFAULT_CREATE(DX9HardwareVertexBuffer);
	FRIEND_CONSTRUCT_DESTRUCT(DX9HardwareVertexBuffer);
	DISALLOW_COPY_ASSIGN(DX9HardwareVertexBuffer);
	BEGIN_DERIVED_REFECTION_TYPE(DX9HardwareVertexBuffer, HardwareVertexBuffer)
	END_DERIVED_REFECTION_TYPE;

protected:
	DX9HardwareVertexBuffer();
	virtual ~DX9HardwareVertexBuffer();

	void HandleLostDevice();
	void HandleResetDevice();

public:
	virtual void* Lock(const VertexBufferDesc& desc)override;
	virtual void  Unlock(const VertexBufferDesc& desc)override;
	virtual void  Render(CGProgram* shader)override;

private:
	void BuildInputLayout(CGProgram* shader);

protected:
	IDirect3DVertexBuffer9* _vertexBuffer[MAX_STREAM_COUNT] = { nullptr };
	IDirect3DVertexDeclaration9* _vertexDeclaration = nullptr;
};
DC_END_NAMESPACE
