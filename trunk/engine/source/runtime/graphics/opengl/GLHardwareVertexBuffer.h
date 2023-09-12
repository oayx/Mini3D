 
/*****************************************************************************
* Author： hannibal
* Date：2020/3/25
* Description： 顶点VBO
*****************************************************************************/
#pragma once

#include "runtime/graphics/null/HardwareBuffer.h"
#include "GLDefine.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class GLHardwareVertexBuffer Final : public HardwareVertexBuffer
{
	friend class GLDevice;
	DEFAULT_CREATE(GLHardwareVertexBuffer);
	FRIEND_CONSTRUCT_DESTRUCT(GLHardwareVertexBuffer);
	DISALLOW_COPY_ASSIGN(GLHardwareVertexBuffer);
	BEGIN_DERIVED_REFECTION_TYPE(GLHardwareVertexBuffer, HardwareVertexBuffer)
	END_DERIVED_REFECTION_TYPE;

protected:
	GLHardwareVertexBuffer();
	virtual ~GLHardwareVertexBuffer();

public:
	virtual void* Lock(const VertexBufferDesc& desc)override;
	virtual void  Unlock(const VertexBufferDesc& desc)override;
	virtual void  Render(CGProgram* shader)override;
	virtual void  Clear()override;

private:
	void BuildInputLayout(CGProgram* shader);

protected:
	uint m_nBufferId[MAX_STREAM_COUNT] = {0};
	GLuint _vao = 0;
};//GLHardwareVertexBuffer
DC_END_NAMESPACE
