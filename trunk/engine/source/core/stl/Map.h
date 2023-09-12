 
/*****************************************************************************************************/
// @author hannibal
// @date   2015/04/29
// @brief  字典
/*****************************************************************************************************/
#pragma once

#include "core/memory/STLAllocate.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
template<class K, class V>
class Map
{
	DECLARE_ALLOCATOR;
public:
	typedef typename std::map<K, V, std::less<K>, STLAlloc<std::pair<const K, V>>>::iterator iterator;
	typedef typename std::map<K, V, std::less<K>, STLAlloc<std::pair<const K, V>>>::reverse_iterator reverse_iterator;
	typedef typename std::map<K, V, std::less<K>, STLAlloc<std::pair<const K, V>>>::const_iterator const_iterator;
	typedef typename std::map<K, V, std::less<K>, STLAlloc<std::pair<const K, V>>>::const_reverse_iterator const_reverse_iterator;

public:
	Map() { }
	Map(const Map& from):m_map(from.m_map) {}

	V& operator [](const K& k) { return m_map.at(k); }

	bool Add(const K& k, const V& v);
	void AddOrUpdate(const K& k, const V& v) { m_map[k] = v; }
	void AddRange(const_iterator begin, const_iterator end) { m_map.insert(begin, end); }
	bool Contains(const K& k) const { return m_map.find(k) != m_map.end(); }
	iterator Find(const K& k) { return m_map.find(k); }
	const_iterator Find(const K& k)const { return m_map.find(k); }
	bool IsEmpty() const { return m_map.empty(); }
	std::pair<typename std::map<K, V, std::less<K>, STLAlloc<std::pair<const K, V>>>::iterator, bool> Insert(const K& k, const V& v) { return m_map.insert(std::pair<K, V>(k, v)); }
	bool Remove(const K& k) { return m_map.erase(k) == 1; }
	iterator Remove(iterator pos) { return m_map.erase(pos); }
	iterator Remove(const_iterator pos) { return m_map.erase(pos); }
	void Clear() { m_map.clear(); }
	int  Size() const { return (int)m_map.size(); }
	bool TryGet(const K& k, V** v);
	bool TryGet(const K& k, const V** v) const;
	bool TryGet(const K& k, V* v)const;

	iterator begin() { return m_map.begin(); }
	iterator end() { return m_map.end(); }
	reverse_iterator rbegin() { return m_map.rbegin(); }
	reverse_iterator rend() { return m_map.rend(); }
	const_iterator begin() const { return (const_iterator)m_map.begin(); }
	const_iterator end() const { return (const_iterator)m_map.end(); }
	const_reverse_iterator rbegin() const { return (const_reverse_iterator)m_map.rbegin(); }
	const_reverse_iterator rend() const { return (const_reverse_iterator)m_map.rend(); }

private:
	std::map<K, V, std::less<K>, STLAlloc<std::pair<const K, V>>> m_map;
};
template<class K, class V>
bool Map<K, V>::Add(const K& k, const V& v)
{
	std::pair<typename std::map<K, V, std::less<K>, STLAlloc<std::pair<const K, V>>>::iterator, bool> ret;
	ret = m_map.insert(std::pair<K, V>(k, v));
	return ret.second;
}
template<class K, class V>
bool Map<K, V>::TryGet(const K& k, V** v)
{
	iterator find = m_map.find(k);
	if (find != m_map.end())
	{
		*v = &find->second;
		return true;
	}

	return false;
}
template<class K, class V>
bool Map<K, V>::TryGet(const K& k, const V** v) const
{
	const_iterator find = m_map.find(k);
	if (find != m_map.end())
	{
		*v = &find->second;
		return true;
	}

	return false;
}
template<class K, class V>
bool Map<K, V>::TryGet(const K& k, V* v)const
{
	const_iterator find = m_map.find(k);
	if (find != m_map.end())
	{
		*v = find->second;
		return true;
	}

	return false;
}
DC_END_NAMESPACE
