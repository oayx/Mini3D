
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
	ByteArray(const ByteArray& other)
	{
		_bufferLen = other._bufferLen;
		_maxBufferLen = other._maxBufferLen;
		_head = other._head;
		_tail = other._tail;
		_buffer = Memory::NewArray<byte>(_bufferLen);
		Memory::Copy(_buffer, other._buffer, _bufferLen);
	}
	ByteArray(ByteArray&& other)noexcept
	{
		_bufferLen = other._bufferLen;
		_maxBufferLen = other._maxBufferLen;
		_head = other._head;
		_tail = other._tail;
		_buffer = std::move(other._buffer);
		other._buffer = nullptr;
	}
	virtual ~ByteArray();

	ByteArray& operator =(const ByteArray& other)
	{
		if (this != &other)
		{
			_bufferLen = other._bufferLen;
			_maxBufferLen = other._maxBufferLen;
			_head = other._head;
			_tail = other._tail;
			Memory::DeleteArray(_buffer);
			_buffer = Memory::NewArray<byte>(_bufferLen);
			Memory::Copy(_buffer, other._buffer, _bufferLen);
		}
		return *this;
	}
	ByteArray& operator =(ByteArray&& other)noexcept
	{
		if (this != &other)
		{
			_bufferLen = other._bufferLen;
			_maxBufferLen = other._maxBufferLen;
			_head = other._head;
			_tail = other._tail;
			Memory::DeleteArray(_buffer);
			_buffer = std::move(other._buffer);
			other._buffer = nullptr;
		}
		return *this;
	}

public:
	void			Init(uint BufferSize, uint MaxBufferSize) noexcept;
	bool			Resize(int size) noexcept;
	void			Clear() noexcept;

	char			ReadChar() noexcept;//读取数据，流标会跳动
	byte			ReadUChar() noexcept;
	short			ReadShort() noexcept;
	ushort			ReadUShort() noexcept;
	int				ReadInt() noexcept;
	uint			ReadUint() noexcept;
	int64			ReadInt64() noexcept;
	float			ReadFloat() noexcept;
	double			ReadDouble() noexcept;
	uint			Read(byte* buf, uint len) noexcept;

	uint			WriteChar(char buf) noexcept;//写数据
	uint			WriteUChar(byte buf) noexcept;
	uint			WriteShort(short buf) noexcept;
	uint			WriteUShort(ushort buf) noexcept;
	uint			WriteInt(int buf) noexcept;
	uint			WriteUInt(uint buf) noexcept;
	uint			WriteInt64(int64 buf) noexcept;
	uint			WriteFloat(float buf) noexcept;
	uint			WriteDouble(double buf) noexcept;
	uint			WriteString(const char* buf) noexcept;
	uint			Write(const byte* buf, uint len) noexcept;

	bool			Peek(byte* buf, uint len) noexcept;//读取数据：流标不调整
	bool			Skip(uint len) noexcept;//移动流标

	inline uint		Capacity()const noexcept { return _bufferLen; }
	uint			Available()const noexcept;

	inline uint		GetPosition()const noexcept { return _head; }
	inline void		SetPosition(uint nPos) noexcept { _head = (nPos) % _bufferLen; }

	inline byte*	GetBuffer() noexcept { return _buffer; }
	inline byte*	GetBuffer(int index) noexcept { return &_buffer[index]; }

protected:
	byte*			_buffer = nullptr;

	uint			_bufferLen = 0;
	uint			_maxBufferLen = 0;

	uint			_head = 0;
	uint			_tail = 0;
};
DC_END_NAMESPACE
