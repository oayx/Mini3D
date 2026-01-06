 
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
	Set(const_iterator begin, const_iterator end) : _set(begin(), end()) { }
	Set(const Set& other) :_set(other._set) {}
	Set(Set&& other) :_set(std::move(other._set)) {}

	void Add(const V& v) noexcept { _set.insert(v); }
	void Clear()  noexcept { _set.clear(); }
	bool Contains(const V& v) const noexcept;
	void ForEach(std::function<void(V)> fun) noexcept;
	void ForEachTrue(std::function<bool(V)> fun) noexcept;
	bool IsEmpty() const noexcept { return _set.empty(); }
	bool Remove(const V& v) noexcept;
	iterator Remove(const_iterator pos) noexcept { return _set.erase(pos); }
	void RemoveAll(const V& v) noexcept { _set.remove(v); }
	int Size() const noexcept { return (int)_set.size(); }

	Set& operator =(const Set& other) noexcept { _set = other._set; return *this; }
	Set& operator =(Set&& other) noexcept { _set = std::move(other._set); return *this; }

	iterator begin() noexcept { return _set.begin(); }
	iterator end() noexcept { return _set.end(); }
	reverse_iterator rbegin() noexcept { return _set.rbegin(); }
	reverse_iterator rend() noexcept { return _set.rend(); }
	const_iterator begin() const noexcept { return (const_iterator)_set.begin(); }
	const_iterator end() const noexcept { return (const_iterator)_set.end(); }
	const_reverse_iterator rbegin() const noexcept { return (const_reverse_iterator)_set.rbegin(); }
	const_reverse_iterator rend() const noexcept { return (const_reverse_iterator)_set.rend(); }

private:
	std::set<V, Cmp, STLAlloc<V>> _set;
};
template<class V, class Cmp>
bool Set<V, Cmp>::Contains(const V& v) const noexcept
{
	for (auto i = _set.begin(); i != _set.end(); ++i)
	{
		if (*i == v)
		{
			return true;
		}
	}

	return false;
}
template<class V, class Cmp>
void Set<V, Cmp>::ForEach(std::function<void(V)> fun) noexcept
{
	for (auto i = _set.begin(); i != _set.end(); ++i)
	{
		fun(*i);
	}
}
template<class V, class Cmp>
void Set<V, Cmp>::ForEachTrue(std::function<bool(V)> fun) noexcept
{
	for (auto i = _set.begin(); i != _set.end(); ++i)
	{
		if (!fun(*i))break;
	}
}
template<class V, class Cmp>
bool Set<V, Cmp>::Remove(const V& v) noexcept
{
	for (auto i = _set.begin(); i != _set.end(); ++i)
	{
		if (*i == v)
		{
			_set.erase(i);
			return true;
		}
	}

	return false;
}
DC_END_NAMESPACE