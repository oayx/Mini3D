 
/*****************************************************************************
* Author： hannibal
* Date：2020/3/30
* Description： 临时执行的命令
*****************************************************************************/
#pragma once

#include "VKDefine.h"

DC_BEGIN_NAMESPACE
class VKTempCommand final
{
	friend class VKRenderContent;
public:
	VKTempCommand();
	~VKTempCommand();

	VkCommandBuffer GetCommandBuffer()const { return _commandBuffer; }

private:
	VkCommandPool SpawnCommandPool();
	void DespawnCommandPool(VkCommandPool pool);
	static void Destroy();

private:
	static List<VkCommandPool> _commandPools;
	VkCommandPool _commandPool = VK_NULL_HANDLE;
	VkCommandBuffer _commandBuffer = VK_NULL_HANDLE;
};
DC_END_NAMESPACE