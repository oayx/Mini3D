 
/*****************************************************************************************************/
// @author hannibal
// @date   2015/04/29
// @brief  hash
/*****************************************************************************************************/
#pragma once

#include <unordered_map>
#include "core/memory/STLAllocate.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
template<class K, class V>
class HashMap
{
	DECLARE_ALLOCATOR;
public:
	typedef typename std::unordered_map<K, V, std::hash<K>, std::equal_to<K>, STLAlloc<std::pair<const K, V>>>::iterator iterator;
	typedef typename std::unordered_map<K, V, std::hash<K>, std::equal_to<K>, STLAlloc<std::pair<const K, V>>>::const_iterator const_iterator;

public:
	HashMap() { }
	HashMap(const HashMap& from):_map(from._map) {}

public:
	V& operator [](const K& k) noexcept { return _map.at(k); }

	bool Add(const K& k, const V& v) noexcept;
	void AddOrUpdate(const K& k, const V& v) noexcept { _map[k] = v; }
	void AddRange(const_iterator begin, const_iterator end) noexcept { _map.insert(begin, end); }
	bool Contains(const K& k) const noexcept { return _map.find(k) != _map.end(); }
	iterator Find(const K& k) noexcept { return _map.find(k); }
	const_iterator Find(const K& k)const noexcept { return _map.find(k); }
	bool IsEmpty() const noexcept { return _map.empty(); }
	std::pair<typename std::unordered_map<K, V, std::hash<K>, std::equal_to<K>, STLAlloc<std::pair<const K, V>>>::iterator, bool> Insert(const K& k, const V& v) noexcept { return _map.insert(std::pair<K, V>(k, v)); }
	bool Remove(const K& k) noexcept { return _map.erase(k) == 1; }
	iterator Remove(iterator pos) noexcept { return _map.erase(pos); }
	iterator Remove(const_iterator pos) noexcept { return _map.erase(pos); }
	void Clear() noexcept { _map.clear(); }
	int  Size() const noexcept { return (int)_map.size(); }
	bool TryGet(const K& k, V** v) noexcept;
	bool TryGet(const K& k, const V** v) const noexcept;
	bool TryGet(const K& k, V* v)const noexcept;

	iterator begin() noexcept { return _map.begin(); }
	iterator end() noexcept { return _map.end(); }
	const_iterator begin() const noexcept { return (const_iterator)_map.begin(); }
	const_iterator end() const noexcept { return (const_iterator)_map.end(); }

private:
	std::unordered_map<K, V, std::hash<K>, std::equal_to<K>, STLAlloc<std::pair<const K, V>>> _map;
};
template<class K, class V>
bool HashMap<K, V>::Add(const K& k, const V& v) noexcept
{
	std::pair<typename std::unordered_map<K, V, std::hash<K>, std::equal_to<K>, STLAlloc<std::pair<const K, V>>>::iterator, bool> ret;
	ret = _map.insert(std::pair<K, V>(k, v));
	return ret.second;
}
template<class K, class V>
bool HashMap<K, V>::TryGet(const K& k, V** v) noexcept
{
	iterator find = _map.find(k);
	if (find != _map.end())
	{
		*v = &find->second;
		return true;
	}

	return false;
}
template<class K, class V>
bool HashMap<K, V>::TryGet(const K& k, const V** v) const noexcept
{
	const_iterator find = _map.find(k);
	if (find != _map.end())
	{
		*v = &find->second;
		return true;
	}

	return false;
}
template<class K, class V>
bool HashMap<K, V>::TryGet(const K& k, V* v)const noexcept
{
	const_iterator find = _map.find(k);
	if (find != _map.end())
	{
		*v = find->second;
		return true;
	}

	return false;
}
DC_END_NAMESPACE
