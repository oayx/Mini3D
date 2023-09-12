 
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

	void Push(const V& v) { m_stack.push(v); }
	void Clear() { while(!m_stack.empty())m_stack.pop(); }
	bool Contains(const V& v) const;
	bool IsEmpty() const { return m_stack.empty(); }
	const V& Peek()const { return m_stack.top(); }
	bool Pop(V& v);
	int Size() const { return (int)m_stack.size(); }

private:
	std::stack<V, std::deque<V, STLAlloc<V>>> m_stack;
};
template<class V>
bool Stack<V>::Contains(const V& v) const
{
	for (auto i = m_stack.begin(); i != m_stack.end(); ++i)
	{
		if (*i == v)
		{
			return true;
		}
	}

	return false;
}
template<class V>
bool Stack<V>::Pop(V& v)
{
	if (m_stack.empty())return false;
	v = m_stack.top();
	m_stack.pop();
	return true;
}
DC_END_NAMESPACE