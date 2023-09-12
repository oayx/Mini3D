 
/*****************************************************************************************************/
// @author hannibal
// @date   2015/04/29
// @brief  已排序列表
/*****************************************************************************************************/
#pragma once

#include "core/memory/STLAllocate.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
template<class V, class Cmp = std::less<V>>
class Set
{
	DECLARE_ALLOCATOR;
public:
	typedef typename std::set<V, Cmp, STLAlloc<V>>::iterator iterator;
	typedef typename std::set<V, Cmp, STLAlloc<V>>::reverse_iterator reverse_iterator;
	typedef typename std::set<V, Cmp, STLAlloc<V>>::const_iterator const_iterator;
	typedef typename std::set<V, Cmp, STLAlloc<V>>::const_reverse_iterator const_reverse_iterator;

public:
	Set() { }
	Set(const_iterator begin, const_iterator end) : m_set(begin(), end()) { }

	void Add(const V& v) { m_set.insert(v); }
	void Clear() { m_set.clear(); }
	bool Contains(const V& v) const;
	void ForEach(std::function<void(V)> fun);
	void ForEachTrue(std::function<bool(V)> fun);
	bool IsEmpty() const { return m_set.empty(); }
	bool Remove(const V& v);
	iterator Remove(const_iterator pos) { return m_set.erase(pos); }
	void RemoveAll(const V& v) { m_set.remove(v); }
	int Size() const { return (int)m_set.size(); }

	iterator begin() { return m_set.begin(); }
	iterator end() { return m_set.end(); }
	reverse_iterator rbegin() { return m_set.rbegin(); }
	reverse_iterator rend() { return m_set.rend(); }
	const_iterator begin() const { return (const_iterator)m_set.begin(); }
	const_iterator end() const { return (const_iterator)m_set.end(); }
	const_reverse_iterator rbegin() const { return (const_reverse_iterator)m_set.rbegin(); }
	const_reverse_iterator rend() const { return (const_reverse_iterator)m_set.rend(); }

private:
	std::set<V, Cmp, STLAlloc<V>> m_set;
};
template<class V, class Cmp>
bool Set<V, Cmp>::Contains(const V& v) const
{
	for (auto i = m_set.begin(); i != m_set.end(); ++i)
	{
		if (*i == v)
		{
			return true;
		}
	}

	return false;
}
template<class V, class Cmp>
void Set<V, Cmp>::ForEach(std::function<void(V)> fun)
{
	for (auto i = m_set.begin(); i != m_set.end(); ++i)
	{
		fun(*i);
	}
}
template<class V, class Cmp>
void Set<V, Cmp>::ForEachTrue(std::function<bool(V)> fun)
{
	for (auto i = m_set.begin(); i != m_set.end(); ++i)
	{
		if (!fun(*i))break;
	}
}
template<class V, class Cmp>
bool Set<V, Cmp>::Remove(const V& v)
{
	for (auto i = m_set.begin(); i != m_set.end(); ++i)
	{
		if (*i == v)
		{
			m_set.erase(i);
			return true;
		}
	}

	return false;
}
DC_END_NAMESPACE