
#include "DX12HardwareVertexBuffer.h"
#include "DX12Device.h"
#include "DX12Program.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(DX12HardwareVertexBuffer, HardwareVertexBuffer);
DX12HardwareVertexBuffer::DX12HardwareVertexBuffer()
{
}
DX12HardwareVertexBuffer::~DX12HardwareVertexBuffer()
{
	for (int i = 0; i < MAX_STREAM_COUNT; ++i)
	{
		SAFE_RELEASE(_vertexBuffer[i]);
	}
}
void* DX12HardwareVertexBuffer::Lock(const VertexBufferDesc& desc)
{
	bool rebuild = this->NeedRebuild(desc);

	base::Lock(desc);
	if (desc.num_vertex == 0)
		return nullptr;

	if (!_vertexBuffer[desc.stream] || rebuild)
	{
		SAFE_RELEASE(_vertexBuffer[desc.stream]);

		CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
		CD3DX12_RESOURCE_DESC bufferResource = CD3DX12_RESOURCE_DESC::Buffer(this->GetBufferSize(desc.stream));
		DX_ERROR(GetDX12Device()->GetDevice()->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &bufferResource, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&_vertexBuffer[desc.stream])));
	
		_vertexBufferViews[desc.stream].BufferLocation = _vertexBuffer[desc.stream]->GetGPUVirtualAddress();
		_vertexBufferViews[desc.stream].SizeInBytes = this->GetBufferSize(desc.stream);
		_vertexBufferViews[desc.stream].StrideInBytes = this->GetVertexSize(desc.stream);
	}

	void* buffer = nullptr;
	CD3DX12_RANGE readRange(0, 0);
	DX_ERROR(_vertexBuffer[desc.stream]->Map(0, &readRange, &buffer));
	return buffer;
}
void  DX12HardwareVertexBuffer::Unlock(const VertexBufferDesc& desc)
{
	base::Unlock(desc);
	if (desc.num_vertex == 0)
		return;
	
	_vertexBuffer[desc.stream]->Unmap(0, nullptr);
}
void DX12HardwareVertexBuffer::Render(CGProgram* shader)
{
	DC_PROFILE_FUNCTION;
	int numViews = _vertexBuffer[INSTANCE_STREAM_INDEX] != nullptr ? 2 : 1;
	GetDX12Device()->GetCommandList()->IASetVertexBuffers(0, numViews, _vertexBufferViews);
}
DC_END_NAMESPACE

