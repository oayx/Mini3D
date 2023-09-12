 
/*****************************************************************************
* Author： hannibal
* Date：2009/11/25
* Description： 顶点buff
*****************************************************************************/
#pragma once

#include "runtime/graphics/null/HardwareBuffer.h"
#include "DX11Define.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
// ClassName：DX11HardwareVertexBuffer
// Description：顶点buff
class DX11HardwareVertexBuffer Final : public HardwareVertexBuffer
{
	friend class DX11Device;
	DEFAULT_CREATE(DX11HardwareVertexBuffer);
	FRIEND_CONSTRUCT_DESTRUCT(DX11HardwareVertexBuffer);
	DISALLOW_COPY_ASSIGN(DX11HardwareVertexBuffer);
	BEGIN_DERIVED_REFECTION_TYPE(DX11HardwareVertexBuffer, HardwareVertexBuffer)
	END_DERIVED_REFECTION_TYPE;

protected:
	DX11HardwareVertexBuffer();
	virtual ~DX11HardwareVertexBuffer();

public:
	virtual void* Lock(const VertexBufferDesc& desc)override;
	virtual void  Unlock(const VertexBufferDesc& desc)override;
	virtual void  Render(CGProgram* shader)override;

private:
	void BuildInputLayout(CGProgram* shader);

private:
	ID3D11Buffer* _vertexBuffer[MAX_STREAM_COUNT] = { nullptr };
	ID3D11InputLayout* _inputLayout = nullptr;
};//DX11HardwareVertexBuffer
DC_END_NAMESPACE
