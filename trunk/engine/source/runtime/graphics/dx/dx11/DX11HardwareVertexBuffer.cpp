#include "DX11HardwareVertexBuffer.h"
#include "DX11Device.h"
#include "DX11Program.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(DX11HardwareVertexBuffer, HardwareVertexBuffer);
DX11HardwareVertexBuffer::DX11HardwareVertexBuffer()
{
}
DX11HardwareVertexBuffer::~DX11HardwareVertexBuffer()
{
	for (int i = 0; i < MAX_STREAM_COUNT; ++i)
	{
		SAFE_RELEASE(_vertexBuffer[i]);
	}
	SAFE_RELEASE(_inputLayout);
}
void* DX11HardwareVertexBuffer::Lock(const VertexBufferDesc& desc)
{
	bool rebuild = this->NeedRebuild(desc);

	base::Lock(desc);
	if (desc.num_vertex == 0)
		return nullptr;

	if (_inputLayout && rebuild)
	{
		SAFE_RELEASE(_inputLayout);
	}

	if (!_vertexBuffer[desc.stream] || rebuild)
	{
		SAFE_RELEASE(_vertexBuffer[desc.stream]);

		D3D11_BUFFER_DESC sd = {};
		sd.ByteWidth = this->GetBufferSize(desc.stream),
		sd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		switch (_usage)
		{
		case GPUResourceUsage::Static:
			sd.Usage = D3D11_USAGE_DEFAULT;
			sd.CPUAccessFlags = 0;
			break;
		case GPUResourceUsage::Dynamic:
			sd.Usage = D3D11_USAGE_DYNAMIC;
			sd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			break;
		default:
			AssertEx(false, "error usage:%d", (int)_usage);
			break;
		}
		DX_ERROR(GetDX11Device()->GetDevice()->CreateBuffer(&sd, nullptr, &_vertexBuffer[desc.stream]));
	}

	if (_usage == GPUResourceUsage::Dynamic)
	{
		D3D11_MAPPED_SUBRESOURCE res = { };
		DX_ERROR(GetDX11Device()->GetContent()->Map(_vertexBuffer[desc.stream], 0, D3D11_MAP_WRITE_DISCARD, 0, &res));
		return res.pData;
	}
	else
	{
		if (!_bufferData[desc.stream] || rebuild)
		{
			Memory::DeleteArray(_bufferData[desc.stream]);
			_bufferData[desc.stream] = Memory::NewArray<float>(this->GetBufferCapacity(desc.stream));
			::memset(_bufferData[desc.stream], 0, this->GetBufferCapacity(desc.stream));
		}
		return _bufferData[desc.stream];
	}
}
void  DX11HardwareVertexBuffer::Unlock(const VertexBufferDesc& desc)
{
	base::Unlock(desc);
	if (desc.num_vertex == 0)
		return;
	
	if (_usage == GPUResourceUsage::Dynamic)
	{
		GetDX11Device()->GetContent()->Unmap(_vertexBuffer[desc.stream], 0);
	}
	else
	{
		D3D11_BOX box = { 0, 0, 0, this->GetBufferSize(desc.stream), 1, 1 };
		GetDX11Device()->GetContent()->UpdateSubresource(_vertexBuffer[desc.stream], 0, &box, _bufferData[desc.stream], 0, 0);
	}
}
void DX11HardwareVertexBuffer::Render(CGProgram* shader)
{
	DC_PROFILE_FUNCTION;
	uint size[] = { this->GetVertexSize(0), this->GetVertexSize(1) };
	uint offset[] = { 0, 0 };
	int count = this->GetInstanceCount() > 0 ? 2 : 1;
	GetDX11Device()->GetContent()->IASetVertexBuffers(0, count, _vertexBuffer, size, offset);

	if (!_inputLayout || IsVertexSemanticChange(shader))
	{
		BuildInputLayout(shader);
	}
	GetDX11Device()->GetContent()->IASetInputLayout(_inputLayout);
}
void DX11HardwareVertexBuffer::BuildInputLayout(CGProgram* shader)
{
	DC_PROFILE_FUNCTION;
	SAFE_RELEASE(_inputLayout);

	Vector<D3D11_INPUT_ELEMENT_DESC> element_desc;
	for (byte stream = 0; stream < MAX_STREAM_COUNT; ++stream)
	{
		for (byte i = 0; i < this->GetElementCount(stream); ++i)
		{
			const VertexElement* element = this->GetElement(stream, i);
			if (!shader->HasInputVertexSemantic(element->GetSemantic()))continue;
			bool is_instance = IsInstanceVertexSemantic(element->GetSemantic());
			D3D11_INPUT_ELEMENT_DESC desc;
			desc.SemanticName = DXGetInputSemantic(element->GetSemantic());
			desc.SemanticIndex = (UINT)element->GetIndex() + stream;
			desc.Format = DX10GetVertexType(element->GetType());
			desc.InputSlot = (UINT)element->GetStream();
			desc.AlignedByteOffset = (UINT)element->GetOffset();
			desc.InputSlotClass = is_instance ? D3D11_INPUT_PER_INSTANCE_DATA : D3D11_INPUT_PER_VERTEX_DATA;
			desc.InstanceDataStepRate = (UINT)element->GetStream();
			element_desc.Add(desc);
		}
	}

	_shaderVertexSemantic = shader->GetInputVertexSemantic();
	ID3DBlob* blob = dynamic_cast<DX11Program*>(shader)->GetBlob(ShaderType::Vertex);
	GetDX11Device()->GetDevice()->CreateInputLayout(element_desc.Data(), (UINT)element_desc.Size(), blob->GetBufferPointer(), blob->GetBufferSize(), &_inputLayout);
}
DC_END_NAMESPACE
