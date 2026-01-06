 
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
	Queue(const Queue& other) :_queue(other._queue) {}
	Queue(Queue&& other) :_queue(std::move(other._queue)) {}

	Queue& operator =(const Queue& other) noexcept { _queue = other._queue; return *this; }
	Queue& operator =(Queue&& other) noexcept { _queue = std::move(other._queue); return *this; }

	void Clear() noexcept { _queue.clear(); }
	bool Contains(const V& v) const noexcept;
	bool Dequeue(V& v) noexcept;
	void Enqueue(const V& v) noexcept { _queue.push(v); }
	bool IsEmpty() const noexcept { return _queue.empty(); }
	const V& Peek()const noexcept { return _queue.top(); }
	int Size() const noexcept { return (int)_queue.size(); }

private:
	std::queue<V, std::deque<V, STLAlloc<V>>> _queue;
};
template<class V>
bool Queue<V>::Contains(const V& v) const noexcept
{
	for (auto i = _queue.begin(); i != _queue.end(); ++i)
	{
		if (*i == v)
		{
			return true;
		}
	}

	return false;
}
template<class V>
bool Queue<V>::Dequeue(V& v) noexcept
{
	if (_queue.empty())return false;
	v = _queue.front();
	_queue.pop();
	return true;
}
DC_END_NAMESPACE