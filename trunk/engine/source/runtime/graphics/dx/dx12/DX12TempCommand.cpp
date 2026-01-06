
#include "DX12TempCommand.h"
#include "DX12Device.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
DX12TempCommand::DX12TempCommand()
{
	D3D12_COMMAND_QUEUE_DESC queue_desc = {};
	queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queue_desc.NodeMask = 1;
	DX_ERROR(GetDX12Device()->GetDevice()->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&cmd_queue)));
	DX_ERROR(GetDX12Device()->GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&cmd_alloc)));
	DX_ERROR(GetDX12Device()->GetDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, cmd_alloc, NULL, IID_PPV_ARGS(&cmd_list)));
	DX_ERROR(GetDX12Device()->GetDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
	fence_event = ::CreateEvent(0, 0, 0, 0);
}
DX12TempCommand::~DX12TempCommand()
{
	DX_ERROR(cmd_list->Close());
	cmd_queue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&cmd_list);
	DX_ERROR(cmd_queue->Signal(fence, 1));
	fence->SetEventOnCompletion(1, fence_event);
	::WaitForSingleObject(fence_event, INFINITE);

	SAFE_RELEASE(cmd_list);
	SAFE_RELEASE(cmd_alloc);
	SAFE_RELEASE(cmd_queue);
	SAFE_RELEASE(fence);
	SAFE_CLOSEHANDLE(fence_event);
}
DC_END_NAMESPACE

