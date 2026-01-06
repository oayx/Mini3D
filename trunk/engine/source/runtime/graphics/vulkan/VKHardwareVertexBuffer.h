 
/*****************************************************************************
* Author： hannibal
* Date：2020/3/25
* Description： 顶点VBO
*****************************************************************************/
#pragma once

#include "runtime/graphics/null/HardwareBuffer.h"
#include "VKDefine.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
// 顶点VBO
class VKHardwareVertexBuffer final : public HardwareVertexBuffer
{
	friend class VKDevice;
	DEFAULT_CREATE(VKHardwareVertexBuffer);
	FRIEND_CONSTRUCT_DESTRUCT(VKHardwareVertexBuffer);
	DISALLOW_COPY_ASSIGN(VKHardwareVertexBuffer);
	BEGIN_DERIVED_REFECTION_TYPE(VKHardwareVertexBuffer, HardwareVertexBuffer)
	END_REFECTION_TYPE;

protected:
	VKHardwareVertexBuffer();
	virtual ~VKHardwareVertexBuffer();

public:
	virtual void*	Lock(const VertexBufferDesc& desc)override;
	virtual void	Unlock(const VertexBufferDesc& desc)override;
	virtual void	Render(CGProgram* shader)override;

private:
	void			Release();

protected:
	VkBuffer		_vertexBuffer = VK_NULL_HANDLE;
	VkDeviceMemory  _bufferMemory = VK_NULL_HANDLE;
};
DC_END_NAMESPACE
