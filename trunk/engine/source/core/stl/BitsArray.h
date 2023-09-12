 
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
	DISALLOW_COPY_ASSIGN(BitsArray);
	BEGIN_REFECTION_TYPE(BitsArray)
	END_FINAL_REFECTION_TYPE;
	
public:
	explicit BitsArray(int size = 256):m_array(NULL){ Init(size); }
	~BitsArray(){ DeleteArray(m_array); }

	inline void SetBit(int pos);
	inline void ClearBit(int pos);
	inline void AddRange(int s, int e);
	inline void ClearRange(int s, int e);
	inline void AddBitArray(BitsArray*);
	inline void ClearBitArray(BitsArray*);
	inline void IntersectBitArray(BitsArray*);
	inline void AddBitArray(char*, int);
	inline void ClearBitArray(char*, int);

	inline bool GetBit(int pos);

	inline int*	Data(){ return m_array; }
	inline int	Size(){ return m_size; }
	inline void Init(int m_size);	

private:
	inline void	CreateArray(bool set = false);

private:
	int* m_array = nullptr;
	int	 m_size = 0;
};
inline void BitsArray::Init(int m_size)
{
	if (m_array)
	{
		DeleteArray(m_array);
		m_array = nullptr;
	}

	this->m_size = m_size / 8 / 4 + 1;
	if (m_size % 8 == 0 && m_size / 8 % 4 == 0) this->m_size--;
	CreateArray();
}
inline void BitsArray::CreateArray(bool set)
{
	m_array = NewArray<int>(m_size);
	memset(m_array, set ? 0xFF : 0, m_size*sizeof(int));
}
inline void BitsArray::SetBit(int pos)
{
	if (!m_array) CreateArray();
	if (intptr_t(m_array) == 1) return;
	m_array[pos >> 5] |= 1 << (pos & 0x1f);
}
inline void BitsArray::ClearBit(int pos)
{
	if (!m_array) return;
	if (intptr_t(m_array) == 1) CreateArray(true);
	m_array[pos >> 5] &= ~(1 << (pos & 0x1f));
}
inline void BitsArray::AddRange(int s, int e)
{
	if (intptr_t(m_array) == 1) return;
	if (!m_array) CreateArray();
	int cs = s >> 5;
	if (s & 0x1f){
		int fillbytes = 0xFFFFFFFF << (s & 0x1f);
		if ((e >> 5) == (s >> 5)) fillbytes &= 0xFFFFFFFF >> (0x1F - e & 0x1F);
		m_array[cs] |= fillbytes;
		cs++;
	};
	int ce = e >> 5;
	if (s >> 5 != ce && (e & 0x1f) != 0x1f){
		m_array[ce] |= 0xFFFFFFFF >> (0x1F - e & 0x1F);
		ce--;
	};
	for (int idx = cs; idx <= ce; idx++)
		m_array[idx] = 0xFFFFFFFF;
	if (cs == 0 && ce == m_size - 1){
		DeleteArray(m_array);
		m_array = (int*)1;
	};
}
inline void BitsArray::ClearRange(int s, int e)
{
	if (!m_array) return;
	if (intptr_t(m_array) == 1) CreateArray(true);
	int cs = s >> 5;
	if (s & 0x1f){
		int fillbytes = 0xFFFFFFFF << (s & 0x1f);
		if ((e & 0x1F) == (s & 0x1F)) fillbytes &= 0xFFFFFFFF >> (0x1F - e & 0x1F);
		m_array[cs] &= ~fillbytes;
		cs++;
	};
	int ce = e >> 5;
	if (s >> 5 != ce && (e & 0x1f) != 0x1f){
		m_array[ce] &= ~(0xFFFFFFFF >> (0x1F - (e & 0x1F)));
		ce--;
	};
	for (int idx = cs; idx <= ce; idx++)
		m_array[idx] = 0x0;
	if (cs == 0 && ce == m_size - 1){
		DeleteArray(m_array);
		m_array = (int*)0;
	};
}
inline void BitsArray::AddBitArray(BitsArray* ba)
{
	if (intptr_t(m_array) == 1) return;
	if (!ba || !ba->m_array) return;
	if (intptr_t(ba->m_array) == 1)
	{
		m_array = (int*)1;
		return;
	};
	if (!m_array) CreateArray();
	for (int i = 0; i < m_size; i++)
		m_array[i] |= ba->m_array[i];
}
inline void BitsArray::ClearBitArray(BitsArray* ba)
{
	if (m_array == NULL) return;
	if (ba == NULL || ba->m_array == NULL) return;
	if (intptr_t(m_array) == 1) CreateArray(true);
	if (intptr_t(ba->m_array) == 1){
		DeleteArray(m_array);
		m_array = 0;
		return;
	};
	for (int i = 0; i < m_size; i++)
		m_array[i] &= ~ba->m_array[i];
}
inline void BitsArray::IntersectBitArray(BitsArray*ba)
{
	if (m_array == NULL) return;
	if (ba == NULL || ba->m_array == NULL){
		DeleteArray(m_array);
		m_array = 0;
		return;
	};
	if (intptr_t(ba->m_array) == 1) return;
	if (intptr_t(m_array) == 1) CreateArray(true);
	for (int i = 0; i < m_size; i++)
		m_array[i] &= ba->m_array[i];
}
inline void BitsArray::AddBitArray(char *bits, int m_size)
{
	if (intptr_t(m_array) == 1) return;
	if (!m_array) CreateArray();
	for (int i = 0; i < m_size && i < this->m_size * 4; i++)
		((char*)m_array)[i] |= bits[i];
}
inline void BitsArray::ClearBitArray(char *bits, int m_size)
{
	if (!m_array) return;
	if (intptr_t(m_array) == 1) CreateArray(true);
	for (int i = 0; i < m_size && i < this->m_size * 4; i++)
		((char*)m_array)[i] &= ~bits[i];
}
inline bool BitsArray::GetBit(int pos)
{
	if (!m_array) return false;
	if (intptr_t(m_array) == 1) return true;
	return (m_array[pos >> 5] & (1 << (pos & 0x1f))) != 0;
}
DC_END_NAMESPACE
