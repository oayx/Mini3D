 
/*****************************************************************************
* Author： hannibal
* Date：2009/11/25
* Description： 
*****************************************************************************/
#pragma once

#include "runtime/graphics/RenderDefine.h"
#include "runtime/CommonDefine.h"

DC_BEGIN_NAMESPACE
class CGProgram;
/********************************************************************/
// 缓冲区基类
class HardwareBuffer : public Object
{
	DISALLOW_COPY_ASSIGN(HardwareBuffer);
	BEGIN_DERIVED_REFECTION_TYPE(HardwareBuffer, Object)
	END_DERIVED_REFECTION_TYPE;

protected:
	HardwareBuffer() {}
	virtual ~HardwareBuffer(){}

public:
	virtual void		Clear() = 0;

	GPUResourceUsage	GetUsage()const { return _usage; }
	GPUResourcePool		GetPool()const { return _pool; }

protected:
	GPUResourceUsage	_usage = GPUResourceUsage::Static;
	GPUResourcePool		_pool = GPUResourcePool::Default;
	bool				_isLocked = false;
};
/********************************************************************/
class VertexElement
{
public:
	VertexElement(byte stream, VertexSize ele_type, VertexSemantic ele_sem, ushort offset, byte index)
		: _stream(stream), _vertexEleType(ele_type), _vertexEleSem(ele_sem), m_nOffset(offset), m_nIndex(index)
	{}

public:
	byte				GetStream()const { return _stream; }
	VertexSize			GetType(void) const { return _vertexEleType; }
	VertexSemantic		GetSemantic(void) const { return _vertexEleSem; }
	ushort				GetOffset(void) const { return m_nOffset; }
	byte				GetIndex(void) const { return m_nIndex; }

private:
	byte				_stream = 0;
	VertexSize			_vertexEleType = VertexSize::Float4;
	VertexSemantic		_vertexEleSem = VertexSemantic::Position;
	ushort				m_nOffset = 0;
	byte				m_nIndex = 0;
};
/********************************************************************/
class HardwareVertexBuffer : public HardwareBuffer
{
	friend class Primitive;
	typedef Vector<VertexElement> VertexElements;
	FRIEND_CONSTRUCT_DESTRUCT(HardwareVertexBuffer);
	DISALLOW_COPY_ASSIGN(HardwareVertexBuffer);
	BEGIN_DERIVED_REFECTION_TYPE(HardwareVertexBuffer, HardwareBuffer)
	END_DERIVED_REFECTION_TYPE;

protected:
	HardwareVertexBuffer();
	~HardwareVertexBuffer();

public:
	// 锁住内存的具体实现
	virtual void*		Lock(const VertexBufferDesc& desc);
	virtual void		Unlock(const VertexBufferDesc& desc);
	virtual void  		Render(CGProgram* shader) = 0;
	virtual void  		Clear()override;

public:
	bool				NeedRebuild(const VertexBufferDesc& desc)const { return (_usage != desc.usage || _bufferCapacity[desc.stream] < desc.num_vertex * this->GetVertexSize(desc.stream)); }
	uint				GetVertexCount()const { return _vertexCount[0]; }
	uint				GetVertexSize(byte stream)const;
	uint				GetBufferSize(int stream)const { return _vertexCount[stream] * this->GetVertexSize(stream); }
	uint				GetBufferCapacity(int stream)const { return _bufferCapacity[stream]; }

	uint				GetInstanceCount()const { return _vertexCount[INSTANCE_STREAM_INDEX]; }

public:
	const VertexElement*GetElement(byte stream, byte index)const;
	const VertexElements&GetElements(byte stream)const;
	byte				GetElementCount(byte stream)const { return (byte)_vertexElements[stream].Size(); }
	void				AddElement(byte stream, VertexSize vertex_ele_type, VertexSemantic vertex_ele_sem, ushort offset, byte index);
	void				ClearElement();

protected:
	bool				IsVertexSemanticChange(CGProgram* shader);

protected:
	uint				_vertexCount[MAX_STREAM_COUNT] = {0};		//如果是instance，表示instance数量
	uint				_bufferCapacity[MAX_STREAM_COUNT] = { 0 };
	float*				_bufferData[MAX_STREAM_COUNT] = { nullptr };
	Vector<VertexElements> _vertexElements;//最大两个
	VertexSemantic		_shaderVertexSemantic = VertexSemantic::None;
};
/********************************************************************/
class HardwareIndexBuffer : public HardwareBuffer
{
	friend class Primitive;
	FRIEND_CONSTRUCT_DESTRUCT(HardwareIndexBuffer);
	DISALLOW_COPY_ASSIGN(HardwareIndexBuffer);
	BEGIN_DERIVED_REFECTION_TYPE(HardwareIndexBuffer, HardwareBuffer)
	END_DERIVED_REFECTION_TYPE;

protected:
	HardwareIndexBuffer()
	{
	}
	~HardwareIndexBuffer()
	{
		if (_bufferData)
		{
			DeleteArray(_bufferData);
			_bufferData = nullptr;
		}
	}

public:
	virtual void* 		Lock(const IndexBufferDesc& desc);
	virtual void  		Unlock();
	virtual void  		Render() = 0;
	virtual void  		Clear()override;

public:
	bool				NeedRebuild(const IndexBufferDesc& desc) { return (_usage != desc.usage || _bufferCapacity < desc.num_index * (desc.index_type == IndexType::B16 ? 2 : 4)); }
	IndexType			GetIndexType()const { return _indexType; }
	uint				GetIndexCount()const { return _indexCount; }
	uint				GetIndexSize()const { return _indexType == IndexType::B16 ? 2 : 4; }
	uint				GetBufferSize()const { return _indexCount * this->GetIndexSize(); }
	uint				GetBufferCapacity()const { return _bufferCapacity; }

protected:
	IndexType			_indexType = IndexType::B32;
	uint				_indexCount = 0;
	uint				_bufferCapacity = 0;
	byte*				_bufferData = nullptr;

};
DC_END_NAMESPACE