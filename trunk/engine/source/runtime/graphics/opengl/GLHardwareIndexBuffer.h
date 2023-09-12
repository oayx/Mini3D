 
/*****************************************************************************
* Author： hannibal
* Date：2020/3/26
* Description： 索引VBO
*****************************************************************************/
#pragma once

#include "runtime/graphics/null/HardwareBuffer.h"
#include "GLDefine.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class GLHardwareIndexBuffer Final : public HardwareIndexBuffer
{
	friend class GLDevice;
	DEFAULT_CREATE(GLHardwareIndexBuffer);
	FRIEND_CONSTRUCT_DESTRUCT(GLHardwareIndexBuffer);
	DISALLOW_COPY_ASSIGN(GLHardwareIndexBuffer);
	BEGIN_DERIVED_REFECTION_TYPE(GLHardwareIndexBuffer, HardwareIndexBuffer)
	END_DERIVED_REFECTION_TYPE;

protected:
	GLHardwareIndexBuffer();
	~GLHardwareIndexBuffer();

public:
	virtual void* Lock(const IndexBufferDesc& desc)override;
	virtual void  Unlock()override;
	virtual void  Render()override;

protected:
	uint m_nBufferId = 0;
};//GLHardwareIndexBuffer
DC_END_NAMESPACE
