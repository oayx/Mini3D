 
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
	List(std::initializer_list<V> list) :_list(list) {}
	List(const_iterator begin, const_iterator end):_list(begin, end) {  }

	List& operator =(const List& from);

	void Add(const V& v) { _list.push_back(v); }
	void AddFirst(const V& v) { _list.push_front(v); }
	void AddLast(const V& v) { _list.push_back(v); }
	void AddRange(const List<V>& vs) { _list.insert(_list.end(), vs.begin(), vs.end()); }
	void AddRange(std::initializer_list<V> list) { _list.insert(_list.end(), list.begin(), list.end()); }
	void AddRange(const_iterator begin, const_iterator end) { _list.insert(_list.end(), begin, end); }
	void Clear() { _list.clear(); }
	bool Contains(const V& v) const;
	V& First() { return _list.front(); }
	const V& First() const { return _list.front(); }
	V& Get(int index) { auto it = _list.begin(); std::advance(it, index); return *it; }
	const V& Get(int index)const { auto it = _list.begin(); std::advance(it, index); return *it; }
	void ForEach(std::function<void(V)> fun);
	void ForEachTrue(std::function<bool(V)> fun);
	iterator Insert(const_iterator it, const V& v) { return _list.insert(it, v); }
	void InsertRange(const_iterator pos, const_iterator begin, const_iterator end) { return _list.insert(pos, begin, end); }
	bool IsEmpty() const { return _list.empty(); }
	V& Last() { return _list.back(); }
	const V& Last() const { return _list.back(); }
	bool Remove(const V& v);
	iterator Remove(const_iterator pos) { return _list.erase(pos); }
	void RemoveAll(const V& v) { _list.remove(v); }
	V RemoveFirst() { V v = _list.front(); _list.pop_front(); return v; }
	V RemoveLast() { V v = _list.back(); _list.pop_back(); return v; }
	void Reverse() { std::reverse(_list.begin(), _list.end()); }
	void Reverse(int index, int count);
	int Size() const { return (int)_list.size(); }
	void Sort(SortFunc func) { _list.sort(func); }
	void Sort() { _list.sort(); }
	void Unique() { _list.unique(); }

	iterator begin() { return _list.begin(); }
	iterator end() { return _list.end(); }
	reverse_iterator rbegin() { return _list.rbegin(); }
	reverse_iterator rend() { return _list.rend(); }
	const_iterator begin() const { return (const_iterator)_list.begin(); }
	const_iterator end() const { return (const_iterator)_list.end(); }
	const_reverse_iterator rbegin() const { return (const_reverse_iterator)_list.rbegin(); }
	const_reverse_iterator rend() const { return (const_reverse_iterator)_list.rend(); }

private:
	int ValidCount(int index, int count)const;

private:
	std::list<V, STLAlloc<V>> _list;
};
template<class V>
List<V>& List<V>::operator =(const List<V>& from)
{
	_list.clear();
	AddRange(from);
	return *this;
}
template<class V>
bool List<V>::Contains(const V& v) const
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
void List<V>::ForEach(std::function<void(V)> fun)
{
	for (auto i = _list.begin(); i != _list.end(); ++i)
	{
		fun(*i);
	}
}
template<class V>
void List<V>::ForEachTrue(std::function<bool(V)> fun)
{
	for (auto i = _list.begin(); i != _list.end(); ++i)
	{
		if (!fun(*i))break;
	}
}
template<class V>
bool List<V>::Remove(const V& v)
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
void List<V>::Reverse(int index, int count)
{
	CHECK_RETURN_PTR_VOID(index >= 0 && count >= 0);
	count = ValidCount(index, count);
	iterator it1 = _list.begin(); std::advance(it1, index);
	iterator it2 = _list.begin(); std::advance(it2, index + count);
	std::reverse(it1, it2);
}
template<class V>
int List<V>::ValidCount(int index, int count)const
{
	if (index + count > this->Size())
	{
		count = this->Size() - index;
		if (count < 0)count = 0;
	}
	return count;
}
DC_END_NAMESPACE