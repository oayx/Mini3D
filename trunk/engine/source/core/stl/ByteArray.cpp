#include "ByteArray.h"
#include "core/Assert.h"
#include "core/math/Math.h"

DC_BEGIN_NAMESPACE
/*****************************************************************************************************/
IMPL_REFECTION_TYPE(ByteArray);
ByteArray::~ByteArray()
{
	Memory::DeleteArray(_buffer);
}
void ByteArray::Init(uint BufferSize, uint MaxBufferSize) noexcept
{
	AssertEx(_buffer == nullptr,"Already init buffer");
	_head = 0;
	_tail = 0;

	_bufferLen = BufferSize;
	_maxBufferLen = MaxBufferSize;

	_buffer = Memory::NewArray<byte>(_bufferLen);
	memset(_buffer, 0, _bufferLen);
}
bool ByteArray::Resize(int size) noexcept
{
	size = Math::Max<int>(size, (int)(_bufferLen >> 1));
	uint newBufferLen = _bufferLen + size;
	uint len = Available();

	if (size < 0)
	{
		if (newBufferLen < len)return false;
	}

	byte* newBuffer = Memory::NewArray<byte>(newBufferLen);

	if (_head<_tail)
	{
		::memcpy(newBuffer, &_buffer[_head], _tail - _head);
	}
	else if (_head>_tail)
	{
		::memcpy(newBuffer, &_buffer[_head], _bufferLen - _head);
		::memcpy(&newBuffer[_bufferLen - _head], _buffer, _tail);
	}

	Memory::DeleteArray(_buffer);

	_buffer = newBuffer;
	_bufferLen = newBufferLen;
	_head = 0;
	_tail = len;

	return true;
}
void ByteArray::Clear() noexcept
{
	_head = 0;
	_tail = 0;
}

char ByteArray::ReadChar() noexcept
{
	char val;
	Read((byte*)&val, sizeof(char));
	return val;
}
byte ByteArray::ReadUChar() noexcept
{
	byte val;
	Read((byte*)&val, sizeof(byte));
	return val;
}
short ByteArray::ReadShort() noexcept
{
	short val;
	Read((byte*)&val, sizeof(short));
	return val;
}
ushort ByteArray::ReadUShort() noexcept
{
	ushort val;
	Read((byte*)&val, sizeof(ushort));
	return val;
}
int ByteArray::ReadInt() noexcept
{
	int val;
	Read((byte*)&val, sizeof(int));
	return val;
}
uint ByteArray::ReadUint() noexcept
{
	uint val;
	Read((byte*)&val, sizeof(uint));
	return val;
}
int64 ByteArray::ReadInt64() noexcept
{
	int64 val;
	Read((byte*)&val, sizeof(int64));
	return val;
}
float ByteArray::ReadFloat() noexcept
{
	float val;
	Read((byte*)&val, sizeof(float));
	return val;
}
double ByteArray::ReadDouble() noexcept
{
	double val;
	Read((byte*)&val, sizeof(double));
	return val;
}
//返回0表示没有读到数据
uint ByteArray::Read(byte* buf, uint len) noexcept
{
	if (len == 0)
		return 0;

	if (len > Available())
		return 0;

	if (_head < _tail)
	{
		::memcpy(buf, &_buffer[_head], len);
	}
	else
	{
		uint rightLen = _bufferLen - _head;
		if (len <= rightLen)
		{
			::memcpy(buf, &_buffer[_head], len);
		}
		else
		{
			::memcpy(buf, &_buffer[_head], rightLen);
			::memcpy(&buf[rightLen], _buffer, len - rightLen);
		}
	}

	_head = (_head + len) % _bufferLen;

	return len;
}

uint ByteArray::WriteChar(char buf) noexcept
{
	return Write((byte*)&buf, sizeof(char));
}
uint ByteArray::WriteUChar(byte buf) noexcept
{
	return Write((byte*)&buf, sizeof(byte));
}
uint ByteArray::WriteShort(short buf) noexcept
{
	return Write((byte*)&buf, sizeof(short));
}
uint ByteArray::WriteUShort(ushort buf) noexcept
{
	return Write((byte*)&buf, sizeof(ushort));
}
uint ByteArray::WriteInt(int buf) noexcept
{
	return Write((byte*)&buf, sizeof(int));
}
uint ByteArray::WriteUInt(uint buf) noexcept
{
	return Write((byte*)&buf, sizeof(uint));
}
uint ByteArray::WriteInt64(int64 buf) noexcept
{
	return Write((byte*)&buf, sizeof(int64));
}
uint ByteArray::WriteFloat(float buf) noexcept
{
	return Write((byte*)&buf, sizeof(float));
}
uint ByteArray::WriteDouble(double buf) noexcept
{
	return Write((byte*)&buf, sizeof(double));
}
uint ByteArray::WriteString(const char* buf) noexcept
{
	return Write((const byte*)buf, sizeof(buf));
}
uint ByteArray::Write(const byte* buf, uint len) noexcept
{
	//					//
	//     T  H			//    H   T			LEN=10
	// 0123456789		// 0123456789
	// abcd...efg		// ...abcd...
	//					//

	uint nFree = ((_head <= _tail) ? (_bufferLen - _tail + _head - 1) : (_head - _tail - 1));

	if (len >= nFree)
	{
		if (!Resize(len - nFree + 1))
			return 0;
	}

	if (_head <= _tail)
	{
		if (_head == 0)
		{
			nFree = _bufferLen - _tail - 1;
			::memcpy(&_buffer[_tail], buf, len);
		}
		else
		{
			nFree = _bufferLen - _tail;
			if (len <= nFree)
			{
				::memcpy(&_buffer[_tail], buf, len);
			}
			else
			{
				::memcpy(&_buffer[_tail], buf, nFree);
				::memcpy(_buffer, &buf[nFree], len - nFree);
			}
		}
	}
	else
	{
		::memcpy(&_buffer[_tail], buf, len);
	}

	_tail = (_tail + len) % _bufferLen;

	return len;
}

bool ByteArray::Peek(byte* buf, uint len) noexcept
{
	if (len == 0)
		return false;

	if (len > Available())
		return false;

	if (_head < _tail)
	{
		::memcpy(buf, &_buffer[_head], len);

	}
	else
	{
		uint rightLen = _bufferLen - _head;
		if (len <= rightLen)
		{
			::memcpy(&buf[0], &_buffer[_head], len);
		}
		else
		{
			::memcpy(&buf[0], &_buffer[_head], rightLen);
			::memcpy(&buf[rightLen], &_buffer[0], len - rightLen);
		}
	}

	return true;
}

bool ByteArray::Skip(uint len) noexcept
{
	if (len == 0)
		return true;

	if (len < 0) len += _bufferLen;
	_head = (_head + len) % _bufferLen;

	return true;
}

uint ByteArray::Available()const noexcept
{
	if (_head<_tail)
		return _tail - _head;
	else if (_head>_tail)
		return _bufferLen - _head + _tail;

	return 0;
}

DC_END_NAMESPACE
