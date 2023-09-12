
/*****************************************************************************************************/
/// @author hannibal
/// @date   2015/04/21
/// @brief  循环字节数组
/*****************************************************************************************************/
#pragma once

#include "core/BaseType.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL ByteArray : public object
{
	BEGIN_REFECTION_TYPE(ByteArray)
	END_FINAL_REFECTION_TYPE;
	
public:
	ByteArray() = default;
	ByteArray(const ByteArray& buffer)
	{
		_bufferLen = buffer._bufferLen;
		_maxBufferLen = buffer._maxBufferLen;
		_head = buffer._head;
		_tail = buffer._tail;
		_buffer = NewArray<byte>(_bufferLen);
		Memory::Copy(_buffer, buffer._buffer, _bufferLen);
	}
	virtual ~ByteArray();
	ByteArray& operator =(const ByteArray& buffer)
	{
		_bufferLen = buffer._bufferLen;
		_maxBufferLen = buffer._maxBufferLen;
		_head = buffer._head;
		_tail = buffer._tail;
		DeleteArray(_buffer);
		_buffer = NewArray<byte>(_bufferLen);
		Memory::Copy(_buffer, buffer._buffer, _bufferLen);
		return *this;
	}

public:
	void			Init(uint BufferSize, uint MaxBufferSize);
	bool			Resize(int size);
	void			Clear();

	char			ReadChar();//读取数据，流标会跳动
	byte			ReadUChar();
	short			ReadShort();
	ushort			ReadUShort();
	int				ReadInt();
	uint			ReadUint();
	int64			ReadInt64();
	float			ReadFloat();
	double			ReadDouble();
	uint			Read(byte* buf, uint len);

	uint			WriteChar(char buf);//写数据
	uint			WriteUChar(byte buf);
	uint			WriteShort(short buf);
	uint			WriteUShort(ushort buf);
	uint			WriteInt(int buf);
	uint			WriteUInt(uint buf);
	uint			WriteInt64(int64 buf);
	uint			WriteFloat(float buf);
	uint			WriteDouble(double buf);
	uint			WriteString(const char* buf);
	uint			Write(const byte* buf, uint len);

	bool			Peek(byte* buf, uint len);//读取数据：流标不调整
	bool			Skip(uint len);//移动流标

	inline uint		Capacity()const							{ return _bufferLen; }
	uint			Available()const;

	inline uint		GetPosition()const						{ return _head; }
	inline void		SetPosition(uint nPos)					{ _head = (nPos) % _bufferLen; }

	inline byte*	GetBuffer()								{ return _buffer; }
	inline byte*	GetBuffer(int index)					{ return &_buffer[index]; }

protected:
	byte*			_buffer = nullptr;

	uint			_bufferLen = 0;
	uint			_maxBufferLen = 0;

	uint			_head = 0;
	uint			_tail = 0;
};
DC_END_NAMESPACE
