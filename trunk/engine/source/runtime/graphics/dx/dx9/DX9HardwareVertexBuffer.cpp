#include "DX9HardwareVertexBuffer.h"
#include "DX9Device.h"
#include "DX9Program.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(DX9HardwareVertexBuffer, HardwareVertexBuffer);
DX9HardwareVertexBuffer::DX9HardwareVertexBuffer()
{
	GetDX9Device()->AddEventListener(LostDeviceEvt, this, MakeEventHandler(DX9HardwareVertexBuffer::HandleLostDevice));
	GetDX9Device()->AddEventListener(ResetDeviceEvt, this, MakeEventHandler(DX9HardwareVertexBuffer::HandleResetDevice));
}
DX9HardwareVertexBuffer::~DX9HardwareVertexBuffer()
{
	GetDX9Device()->RemoveEventListener(LostDeviceEvt, this, MakeEventHandler(DX9HardwareVertexBuffer::HandleLostDevice));
	GetDX9Device()->RemoveEventListener(ResetDeviceEvt, this, MakeEventHandler(DX9HardwareVertexBuffer::HandleResetDevice));
	for (int i = 0; i < MAX_STREAM_COUNT; ++i)
	{
		SAFE_RELEASE(_vertexBuffer[i]);
	}
	SAFE_RELEASE(_vertexDeclaration);
}
void* DX9HardwareVertexBuffer::Lock(const VertexBufferDesc& desc)
{
	bool rebuild = this->NeedRebuild(desc);

	base::Lock(desc);
	if (desc.num_vertex == 0)
		return nullptr;

	if (_vertexDeclaration && rebuild)
	{
		SAFE_RELEASE(_vertexDeclaration);
	}

	if (!_vertexBuffer[desc.stream] || rebuild)
	{
		SAFE_RELEASE(_vertexBuffer[desc.stream]);
		DX9HR(GetDX9Device()->GetDevice()->CreateVertexBuffer(
			this->GetBufferCapacity(desc.stream),
			DX9GetResourceUsage(_usage),
			0,
			DX9GetResourcePool(_pool),
			&_vertexBuffer[desc.stream],
			NULL));
	}
	//由于需要处理设备丢失，这里不用Lock，而是分配一块内存；设备恢复时直接使用这块内存
	if (rebuild)
	{
		DeleteArray(_bufferData[desc.stream]);
		_bufferData[desc.stream] = NewArray<float>(this->GetBufferCapacity(desc.stream));
	}
	return _bufferData[desc.stream];
}
void  DX9HardwareVertexBuffer::Unlock(const VertexBufferDesc& desc)
{
	base::Unlock(desc);
	if (desc.num_vertex == 0)
		return;

	void *buffer = NULL;
	DX9HR(_vertexBuffer[desc.stream]->Lock(
		0,
		this->GetBufferSize(desc.stream),
		&buffer,
		(_usage == GPUResourceUsage::Dynamic) ? D3DLOCK_DISCARD : 0
	));
	Memory::Copy(buffer, _bufferData[desc.stream], this->GetBufferSize(desc.stream));
	DX9HR(_vertexBuffer[desc.stream]->Unlock());
}
void DX9HardwareVertexBuffer::Render(CGProgram* shader)
{
	for (uint i = 0; i < MAX_STREAM_COUNT; ++i)
	{
		DX9HR(GetDX9Device()->GetDevice()->SetStreamSourceFreq(i, 1));
	}

	if (_vertexBuffer[0])
	{
		DX9HR(GetDX9Device()->GetDevice()->SetStreamSource(0, _vertexBuffer[0], 0, this->GetVertexSize(0)));
		//实例
		if (_vertexCount[INSTANCE_STREAM_INDEX] > 0 && _vertexBuffer[INSTANCE_STREAM_INDEX])
		{
			DX9HR(GetDX9Device()->GetDevice()->SetStreamSourceFreq(0, D3DSTREAMSOURCE_INDEXEDDATA | _vertexCount[INSTANCE_STREAM_INDEX]));

			DX9HR(GetDX9Device()->GetDevice()->SetStreamSource(INSTANCE_STREAM_INDEX, _vertexBuffer[INSTANCE_STREAM_INDEX], 0, this->GetVertexSize(INSTANCE_STREAM_INDEX)));
			DX9HR(GetDX9Device()->GetDevice()->SetStreamSourceFreq(INSTANCE_STREAM_INDEX, D3DSTREAMSOURCE_INSTANCEDATA | 1ul));
		}
	}

	if (!_vertexDeclaration || IsVertexSemanticChange(shader))
	{
		BuildInputLayout(shader);
	}
	DX9HR(GetDX9Device()->GetDevice()->SetVertexDeclaration(_vertexDeclaration));
}
void DX9HardwareVertexBuffer::HandleLostDevice()
{
	for (int i = 0; i < MAX_STREAM_COUNT; ++i)
	{
		SAFE_RELEASE(_vertexBuffer[i]);
	}
}
void DX9HardwareVertexBuffer::HandleResetDevice()
{
	for (int i = 0; i < MAX_STREAM_COUNT; ++i)
	{
		if (_vertexCount[i] == 0)continue;
		DX9HR(GetDX9Device()->GetDevice()->CreateVertexBuffer(
			this->GetBufferCapacity(i),
			DX9GetResourceUsage(_usage),
			0,
			DX9GetResourcePool(_pool),
			&_vertexBuffer[i],
			NULL));
		{
			void *ret = NULL;
			DX9HR(_vertexBuffer[i]->Lock(
				0,
				this->GetBufferSize(i),
				&ret,
				(_usage == GPUResourceUsage::Dynamic) ? D3DLOCK_DISCARD : 0
			));
			Memory::Copy(ret, _bufferData[i], this->GetBufferSize(i));
			DX9HR(_vertexBuffer[i]->Unlock());
		}
	}
}
void DX9HardwareVertexBuffer::BuildInputLayout(CGProgram* shader)
{
	SAFE_RELEASE(_vertexDeclaration);

	Vector<D3DVERTEXELEMENT9> element_desc;
	for (byte stream = 0; stream < MAX_STREAM_COUNT; ++stream)
	{
		for (byte i = 0; i < this->GetElementCount(stream); ++i)
		{
			const VertexElement* element = this->GetElement(stream, i);
			//if (!(input_semantic & element->GetSemantic()))continue;
			D3DVERTEXELEMENT9 desc;
			desc.Stream = static_cast<WORD>(element->GetStream());
			desc.Offset = static_cast<WORD>(element->GetOffset());
			desc.Type = DX9GetVertexDeclType(element->GetType());
			desc.Method = D3DDECLMETHOD_DEFAULT;
			desc.Usage = DX9GetVertexDeclUsage(element->GetSemantic());
			desc.UsageIndex = element->GetIndex() + stream;
			element_desc.Add(desc);
		}
	}
	element_desc.Add(D3DDECL_END());

	_shaderVertexSemantic = shader->GetInputVertexSemantic();
	DX9HR(GetDX9Device()->GetDevice()->CreateVertexDeclaration(element_desc.Data(), &_vertexDeclaration));
}
DC_END_NAMESPACE