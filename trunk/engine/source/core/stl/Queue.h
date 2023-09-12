 
/*****************************************************************************************************/
// @author hannibal
// @date   2015/04/29
// @brief  队列
/*****************************************************************************************************/
#pragma once

#include "core/memory/STLAllocate.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
template<class V>
class Queue
{
	DECLARE_ALLOCATOR;
public:
	Queue() { }

	void Clear() { m_queue.clear(); }
	bool Contains(const V& v) const;
	bool Dequeue(V& v);
	void Enqueue(const V& v) { m_queue.push(v); }
	bool IsEmpty() const { return m_queue.empty(); }
	const V& Peek()const { return m_queue.top(); }
	int Size() const { return (int)m_queue.size(); }

private:
	std::queue<V, std::deque<V, STLAlloc<V>>> m_queue;
};
template<class V>
bool Queue<V>::Contains(const V& v) const
{
	for (auto i = m_queue.begin(); i != m_queue.end(); ++i)
	{
		if (*i == v)
		{
			return true;
		}
	}

	return false;
}
template<class V>
bool Queue<V>::Dequeue(V& v)
{
	if (m_queue.empty())return false;
	v = m_queue.front();
	m_queue.pop();
	return true;
}
DC_END_NAMESPACE