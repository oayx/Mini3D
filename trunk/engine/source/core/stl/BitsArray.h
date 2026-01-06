 
/*****************************************************************************************************/
// @author hannibal
// @date   2015/04/29
// @brief  设置标记位 0/1
/*****************************************************************************************************/
#pragma once

#include "core/BaseType.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL BitsArray : public object
{
	BEGIN_REFECTION_TYPE(BitsArray)
	END_FINAL_REFECTION_TYPE;
	
public:
	explicit BitsArray(int size = 256):_array(nullptr){ Init(size); }
	BitsArray(const BitsArray& other);
	BitsArray(BitsArray&& other)noexcept;
	~BitsArray(){ Memory::DeleteArray(_array); }

	BitsArray& operator =(const BitsArray& other);
	BitsArray& operator =(BitsArray&& other)noexcept;

public:
	inline void SetBit(int pos) noexcept;
	inline void ClearBit(int pos) noexcept;
	inline void AddRange(int s, int e) noexcept;
	inline void ClearRange(int s, int e) noexcept;
	inline void AddBitArray(BitsArray*) noexcept;
	inline void ClearBitArray(BitsArray*) noexcept;
	inline void IntersectBitArray(BitsArray*) noexcept;
	inline void AddBitArray(char*, int) noexcept;
	inline void ClearBitArray(char*, int) noexcept;

	inline bool GetBit(int pos) noexcept;

	inline int*	Data() noexcept { return _array; }
	inline int	Size() noexcept { return _size; }
	inline void Init(int _size) noexcept;

private:
	inline void	CreateArray(bool set = false) noexcept;

private:
	int* _array = nullptr;
	int	 _size = 0;
};
inline BitsArray::BitsArray(const BitsArray& other)
{
	_size = other._size;
	_array = Memory::NewArray<int>(_size);
	Memory::Copy(_array, other._array, sizeof(int) * _size);
}
inline BitsArray::BitsArray(BitsArray&& other)noexcept
{
	_size = other._size;
	_array = std::move(other._array);
	other._array = nullptr;
}

inline BitsArray& BitsArray::operator =(const BitsArray& other)
{
	if (this != &other)
	{
		Memory::DeleteArray(_array);
		_size = other._size;
		_array = Memory::NewArray<int>(_size);
		Memory::Copy(_array, other._array, sizeof(int) * _size);
	}
	return *this;
}
inline BitsArray& BitsArray::operator =(BitsArray&& other)noexcept
{
	if (this != &other)
	{
		Memory::DeleteArray(_array);
		_size = other._size;
		_array = std::move(other._array);
		other._array = nullptr;
	}
	return *this;
}
inline void BitsArray::Init(int _size) noexcept
{
	if (_array)
	{
		Memory::DeleteArray(_array);
		_array = nullptr;
	}

	this->_size = _size / 8 / 4 + 1;
	if (_size % 8 == 0 && _size / 8 % 4 == 0) this->_size--;
	CreateArray();
}
inline void BitsArray::CreateArray(bool set) noexcept
{
	_array = Memory::NewArray<int>(_size);
	memset(_array, set ? 0xFF : 0, _size*sizeof(int));
}
inline void BitsArray::SetBit(int pos) noexcept
{
	if (!_array) CreateArray();
	if (intptr_t(_array) == 1) return;
	_array[pos >> 5] |= 1 << (pos & 0x1f);
}
inline void BitsArray::ClearBit(int pos) noexcept
{
	if (!_array) return;
	if (intptr_t(_array) == 1) CreateArray(true);
	_array[pos >> 5] &= ~(1 << (pos & 0x1f));
}
inline void BitsArray::AddRange(int s, int e) noexcept
{
	if (intptr_t(_array) == 1) return;
	if (!_array) CreateArray();
	int cs = s >> 5;
	if (s & 0x1f){
		int fillbytes = 0xFFFFFFFF << (s & 0x1f);
		if ((e >> 5) == (s >> 5)) fillbytes &= 0xFFFFFFFF >> (0x1F - e & 0x1F);
		_array[cs] |= fillbytes;
		cs++;
	};
	int ce = e >> 5;
	if (s >> 5 != ce && (e & 0x1f) != 0x1f){
		_array[ce] |= 0xFFFFFFFF >> (0x1F - e & 0x1F);
		ce--;
	};
	for (int idx = cs; idx <= ce; idx++)
		_array[idx] = 0xFFFFFFFF;
	if (cs == 0 && ce == _size - 1){
		Memory::DeleteArray(_array);
		_array = (int*)1;
	};
}
inline void BitsArray::ClearRange(int s, int e) noexcept
{
	if (!_array) return;
	if (intptr_t(_array) == 1) CreateArray(true);
	int cs = s >> 5;
	if (s & 0x1f){
		int fillbytes = 0xFFFFFFFF << (s & 0x1f);
		if ((e & 0x1F) == (s & 0x1F)) fillbytes &= 0xFFFFFFFF >> (0x1F - e & 0x1F);
		_array[cs] &= ~fillbytes;
		cs++;
	};
	int ce = e >> 5;
	if (s >> 5 != ce && (e & 0x1f) != 0x1f){
		_array[ce] &= ~(0xFFFFFFFF >> (0x1F - (e & 0x1F)));
		ce--;
	};
	for (int idx = cs; idx <= ce; idx++)
		_array[idx] = 0x0;
	if (cs == 0 && ce == _size - 1){
		Memory::DeleteArray(_array);
		_array = (int*)0;
	};
}
inline void BitsArray::AddBitArray(BitsArray* ba) noexcept
{
	if (intptr_t(_array) == 1) return;
	if (!ba || !ba->_array) return;
	if (intptr_t(ba->_array) == 1)
	{
		_array = (int*)1;
		return;
	};
	if (!_array) CreateArray();
	for (int i = 0; i < _size; i++)
		_array[i] |= ba->_array[i];
}
inline void BitsArray::ClearBitArray(BitsArray* ba) noexcept
{
	if (_array == NULL) return;
	if (ba == NULL || ba->_array == NULL) return;
	if (intptr_t(_array) == 1) CreateArray(true);
	if (intptr_t(ba->_array) == 1){
		Memory::DeleteArray(_array);
		_array = 0;
		return;
	};
	for (int i = 0; i < _size; i++)
		_array[i] &= ~ba->_array[i];
}
inline void BitsArray::IntersectBitArray(BitsArray*ba) noexcept
{
	if (_array == NULL) return;
	if (ba == NULL || ba->_array == NULL){
		Memory::DeleteArray(_array);
		_array = 0;
		return;
	};
	if (intptr_t(ba->_array) == 1) return;
	if (intptr_t(_array) == 1) CreateArray(true);
	for (int i = 0; i < _size; i++)
		_array[i] &= ba->_array[i];
}
inline void BitsArray::AddBitArray(char *bits, int _size) noexcept
{
	if (intptr_t(_array) == 1) return;
	if (!_array) CreateArray();
	for (int i = 0; i < _size && i < this->_size * 4; i++)
		((char*)_array)[i] |= bits[i];
}
inline void BitsArray::ClearBitArray(char *bits, int _size) noexcept
{
	if (!_array) return;
	if (intptr_t(_array) == 1) CreateArray(true);
	for (int i = 0; i < _size && i < this->_size * 4; i++)
		((char*)_array)[i] &= ~bits[i];
}
inline bool BitsArray::GetBit(int pos) noexcept
{
	if (!_array) return false;
	if (intptr_t(_array) == 1) return true;
	return (_array[pos >> 5] & (1 << (pos & 0x1f))) != 0;
}
DC_END_NAMESPACE
