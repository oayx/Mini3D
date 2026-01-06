 
/*****************************************************************************
* Author： hannibal
* Date：2020/3/26
* Description： 索引VBO
*****************************************************************************/
#pragma once

#include "runtime/graphics/null/HardwareBuffer.h"
#include "VKDefine.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class VKHardwareIndexBuffer final : public HardwareIndexBuffer
{
	friend class VKDevice;
	DEFAULT_CREATE(VKHardwareIndexBuffer);
	FRIEND_CONSTRUCT_DESTRUCT(VKHardwareIndexBuffer);
	DISALLOW_COPY_ASSIGN(VKHardwareIndexBuffer);
	BEGIN_DERIVED_REFECTION_TYPE(VKHardwareIndexBuffer, HardwareIndexBuffer)
	END_REFECTION_TYPE;

protected:
	VKHardwareIndexBuffer();
	~VKHardwareIndexBuffer();

public:
	virtual void*	Lock(const IndexBufferDesc& desc)override;
	virtual void	Unlock()override;
	virtual void	Render()override;

private:
	void			Release();

protected:
	VkBuffer		_indexBuffer = VK_NULL_HANDLE;
	VkDeviceMemory  _bufferMemory = VK_NULL_HANDLE;
};//VKHardwareIndexBuffer
DC_END_NAMESPACE
