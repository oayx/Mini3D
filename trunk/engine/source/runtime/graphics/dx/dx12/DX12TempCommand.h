 
/*****************************************************************************
* Author： hannibal
* Date：2020/3/30
* Description： 临时执行的命令
*****************************************************************************/
#pragma once

#include "DX12Define.h"

DC_BEGIN_NAMESPACE
class DX12TempCommand
{
public:
	DX12TempCommand();
	~DX12TempCommand();

	ID3D12CommandQueue*			cmd_queue = nullptr;
	ID3D12CommandAllocator*		cmd_alloc = nullptr;
	ID3D12GraphicsCommandList*	cmd_list = nullptr;
	ID3D12Fence*				fence = nullptr;
	HANDLE						fence_event = nullptr;
};
DC_END_NAMESPACE

