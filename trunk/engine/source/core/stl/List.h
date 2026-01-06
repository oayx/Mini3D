 
/*****************************************************************************************************/
// @author hannibal
// @date   2015/04/29
// @brief  列表
/*****************************************************************************************************/
#pragma once

#include "core/memory/STLAllocate.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
template<class V>
class List
{
	DECLARE_ALLOCATOR;
public:
	typedef std::function<bool(const V&, const V&)> SortFunc;
	typedef typename std::list<V, STLAlloc<V>>::iterator iterator;
	typedef typename std::list<V, STLAlloc<V>>::reverse_iterator reverse_iterator;
	typedef typename std::list<V, STLAlloc<V>>::const_iterator const_iterator;
	typedef typename std::list<V, STLAlloc<V>>::const_reverse_iterator const_reverse_iterator;

public:
	List() { }
	List(const List& from) :_list(from._list) {}
	List(List&& other) :_list(std::move(other._list)) {}
	List(std::initializer_list<V> list) :_list(list) {}
	List(const_iterator begin, const_iterator end):_list(begin, end) {  }

	List& operator =(const List& from) noexcept;
	List& operator =(List&& other) noexcept { _list = std::move(other._list); return *this; }

	void Add(const V& v) noexcept { _list.push_back(v); }
	void AddFirst(const V& v) noexcept { _list.push_front(v); }
	void AddLast(const V& v) noexcept { _list.push_back(v); }
	void AddRange(const List<V>& vs) noexcept { _list.insert(_list.end(), vs.begin(), vs.end()); }
	void AddRange(std::initializer_list<V> list) noexcept { _list.insert(_list.end(), list.begin(), list.end()); }
	void AddRange(const_iterator begin, const_iterator end) noexcept { _list.insert(_list.end(), begin, end); }
	void Clear() noexcept { _list.clear(); }
	bool Contains(const V& v) const noexcept;
	V& First() noexcept { return _list.front(); }
	const V& First() const noexcept { return _list.front(); }
	V& Get(int index) noexcept { auto it = _list.begin(); std::advance(it, index); return *it; }
	const V& Get(int index)const noexcept { auto it = _list.begin(); std::advance(it, index); return *it; }
	void ForEach(std::function<void(V)> fun) noexcept;
	void ForEachTrue(std::function<bool(V)> fun) noexcept;
	iterator Insert(const_iterator it, const V& v) noexcept { return _list.insert(it, v); }
	void InsertRange(const_iterator pos, const_iterator begin, const_iterator end) noexcept { return _list.insert(pos, begin, end); }
	bool IsEmpty() const noexcept { return _list.empty(); }
	V& Last() noexcept { return _list.back(); }
	const V& Last() const noexcept { return _list.back(); }
	bool Remove(const V& v) noexcept;
	iterator Remove(const_iterator pos) noexcept { return _list.erase(pos); }
	void RemoveAll(const V& v) noexcept { _list.remove(v); }
	V RemoveFirst() noexcept { V v = _list.front(); _list.pop_front(); return v; }
	V RemoveLast() noexcept { V v = _list.back(); _list.pop_back(); return v; }
	void Reverse() noexcept { std::reverse(_list.begin(), _list.end()); }
	void Reverse(int index, int count) noexcept;
	int Size() const noexcept { return (int)_list.size(); }
	void Sort(SortFunc func) noexcept { _list.sort(func); }
	void Sort() noexcept { _list.sort(); }
	void Unique() noexcept { _list.unique(); }

	iterator begin() noexcept { return _list.begin(); }
	iterator end() noexcept { return _list.end(); }
	reverse_iterator rbegin() noexcept { return _list.rbegin(); }
	reverse_iterator rend() noexcept { return _list.rend(); }
	const_iterator begin() const noexcept { return (const_iterator)_list.begin(); }
	const_iterator end() const noexcept { return (const_iterator)_list.end(); }
	const_reverse_iterator rbegin() const noexcept { return (const_reverse_iterator)_list.rbegin(); }
	const_reverse_iterator rend() const noexcept { return (const_reverse_iterator)_list.rend(); }

private:
	int ValidCount(int index, int count)const noexcept;

private:
	std::list<V, STLAlloc<V>> _list;
};
template<class V>
List<V>& List<V>::operator =(const List<V>& from) noexcept
{
	_list.clear();
	AddRange(from);
	return *this;
}
template<class V>
bool List<V>::Contains(const V& v) const noexcept
{
	for (auto i = _list.begin(); i != _list.end(); ++i)
	{
		if (*i == v)
		{
			return true;
		}
	}

	return false;
}
template<class V>
void List<V>::ForEach(std::function<void(V)> fun) noexcept
{
	for (auto i = _list.begin(); i != _list.end(); ++i)
	{
		fun(*i);
	}
}
template<class V>
void List<V>::ForEachTrue(std::function<bool(V)> fun) noexcept
{
	for (auto i = _list.begin(); i != _list.end(); ++i)
	{
		if (!fun(*i))break;
	}
}
template<class V>
bool List<V>::Remove(const V& v) noexcept
{
	for (auto i = _list.begin(); i != _list.end(); ++i)
	{
		if (*i == v)
		{
			_list.erase(i);
			return true;
		}
	}

	return false;
}
template<class V>
void List<V>::Reverse(int index, int count) noexcept
{
	CHECK_RETURN_PTR_VOID(index >= 0 && count >= 0);
	count = ValidCount(index, count);
	iterator it1 = _list.begin(); std::advance(it1, index);
	iterator it2 = _list.begin(); std::advance(it2, index + count);
	std::reverse(it1, it2);
}
template<class V>
int List<V>::ValidCount(int index, int count)const noexcept
{
	if (index + count > this->Size())
	{
		count = this->Size() - index;
		if (count < 0)count = 0;
	}
	return count;
}
DC_END_NAMESPACE