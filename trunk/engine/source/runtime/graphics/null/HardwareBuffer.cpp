#include "HardwareBuffer.h"
#include "CGProgram.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(HardwareBuffer, Object);
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(HardwareVertexBuffer, HardwareBuffer);
HardwareVertexBuffer::HardwareVertexBuffer()
{
	_vertexElements.Reserve(MAX_STREAM_COUNT);
	for (byte i = 0; i < MAX_STREAM_COUNT; ++i)
	{
		_vertexElements.Add(VertexElements());
	}
}
HardwareVertexBuffer::~HardwareVertexBuffer()
{
	ClearElement();
	for (byte i = 0; i < MAX_STREAM_COUNT; ++i)
	{
		DeleteArray(_bufferData[i]);
	}
}
void* HardwareVertexBuffer::Lock(const VertexBufferDesc& desc)
{
	AssertEx(!_isLocked, "HardwareVertexBuffer::Lock error");
	_isLocked = true;
	if (desc.stream == 0)
	{
		_usage = desc.usage;
		_pool = desc.pool;
	}

	_vertexCount[desc.stream] = desc.num_vertex;
	if (desc.num_vertex * this->GetVertexSize(desc.stream) > _bufferCapacity[desc.stream])
	{
		_bufferCapacity[desc.stream] = desc.num_vertex * this->GetVertexSize(desc.stream);
	}
	return nullptr;
}
void HardwareVertexBuffer::Unlock(const VertexBufferDesc& desc)
{
	_isLocked = false;
}
void HardwareVertexBuffer::Clear()
{
	for (byte i = 0; i < MAX_STREAM_COUNT; ++i)
	{
		_vertexCount[i] = 0;
	}
	this->ClearElement();
	_shaderVertexSemantic = VertexSemantic::None;
}
uint HardwareVertexBuffer::GetVertexSize(byte stream)const
{
	AssertEx(stream < MAX_STREAM_COUNT, "");
	uint sz = 0;
	for (auto it = _vertexElements[stream].begin(); it != _vertexElements[stream].end(); ++it)
	{
		if (it->GetStream() == stream)
			sz += GetSizeByVertex(it->GetType());
	}
	return sz;
}
const VertexElement* HardwareVertexBuffer::GetElement(byte stream, byte index)const
{
	AssertEx(stream < MAX_STREAM_COUNT, "");
	return &_vertexElements[stream][index];
}
const Vector<VertexElement>& HardwareVertexBuffer::GetElements(byte stream)const
{
	AssertEx(stream < MAX_STREAM_COUNT, "");
	return _vertexElements[stream];
}
void HardwareVertexBuffer::AddElement(byte stream, VertexSize vertex_ele_type, VertexSemantic vertex_ele_sem, ushort offset, byte index)
{
	AssertEx(stream < MAX_STREAM_COUNT, "");
	VertexElement element(stream, vertex_ele_type, vertex_ele_sem, offset, index);
	_vertexElements[stream].Add(element);
}
void HardwareVertexBuffer::ClearElement()
{
	for (int i = 0; i < _vertexElements.Size(); ++i)
	{
		_vertexElements[i].Clear();
	}
}
bool HardwareVertexBuffer::IsVertexSemanticChange(CGProgram* shader)
{
	return _shaderVertexSemantic != shader->GetInputVertexSemantic();
}
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(HardwareIndexBuffer, HardwareBuffer);
void* HardwareIndexBuffer::Lock(const IndexBufferDesc& desc)
{
	AssertEx(!_isLocked, "HardwareIndexBuffer::Lock error");
	_isLocked = true;
	_usage = desc.usage;
	_pool = desc.pool;
	_indexType = desc.index_type;
	_indexCount = desc.num_index;
	if (_indexCount * this->GetIndexSize() > _bufferCapacity)
	{
		_bufferCapacity = _indexCount * this->GetIndexSize();
	}
	return nullptr;
}
void HardwareIndexBuffer::Unlock()
{
	_isLocked = false;
}
void HardwareIndexBuffer::Clear()
{
	_indexCount = 0;
}
DC_END_NAMESPACE