 
/*****************************************************************************************************/
// @author hannibal
// @date   2015/04/29
// @brief  栈
/*****************************************************************************************************/
#pragma once

#include "core/memory/STLAllocate.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
template<class V>
class Stack
{
	DECLARE_ALLOCATOR;
public:
	Stack() { }
	Stack(const Stack& other) :_stack(other._stack) {}
	Stack(Stack&& other) :_stack(std::move(other._stack)) {}
	Stack& operator =(const Stack& other) noexcept { _stack = other._stack; return *this; }
	Stack& operator =(Stack&& other) noexcept { _stack = std::move(other._stack); return *this; }

	void Push(const V& v) noexcept { _stack.push(v); }
	void Clear() noexcept { while(!_stack.empty())_stack.pop(); }
	bool Contains(const V& v) const noexcept;
	bool IsEmpty() const noexcept { return _stack.empty(); }
	const V& Peek()const noexcept { return _stack.top(); }
	bool Pop(V& v) noexcept;
	int Size() const noexcept { return (int)_stack.size(); }

private:
	std::stack<V, std::deque<V, STLAlloc<V>>> _stack;
};
template<class V>
bool Stack<V>::Contains(const V& v) const noexcept
{
	for (auto i = _stack.begin(); i != _stack.end(); ++i)
	{
		if (*i == v)
		{
			return true;
		}
	}

	return false;
}
template<class V>
bool Stack<V>::Pop(V& v) noexcept
{
	if (_stack.empty())return false;
	v = _stack.top();
	_stack.pop();
	return true;
}
DC_END_NAMESPACE