 
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
	Map(const Map& other) :_map(other._map) {}
	Map(Map&& other) :_map(std::move(other._map)) {}

	Map& operator =(const Map& other) noexcept { _map = other._map; return *this; }
	Map& operator =(Map&& other) noexcept { _map = std::move(other._map); return *this; }

	V& operator [](const K& k) noexcept { return _map.at(k); }

	bool Add(const K& k, const V& v) noexcept;
	void AddOrUpdate(const K& k, const V& v) noexcept { _map[k] = v; }
	void AddRange(const_iterator begin, const_iterator end) noexcept { _map.insert(begin, end); }
	bool Contains(const K& k) const noexcept { return _map.find(k) != _map.end(); }
	iterator Find(const K& k) noexcept { return _map.find(k); }
	const_iterator Find(const K& k)const noexcept { return _map.find(k); }
	bool IsEmpty() const noexcept { return _map.empty(); }
	std::pair<typename std::map<K, V, std::less<K>, STLAlloc<std::pair<const K, V>>>::iterator, bool> Insert(const K& k, const V& v) noexcept { return _map.insert(std::pair<K, V>(k, v)); }
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
	reverse_iterator rbegin() noexcept { return _map.rbegin(); }
	reverse_iterator rend() noexcept { return _map.rend(); }
	const_iterator begin() const noexcept { return (const_iterator)_map.begin(); }
	const_iterator end() const noexcept { return (const_iterator)_map.end(); }
	const_reverse_iterator rbegin() const noexcept { return (const_reverse_iterator)_map.rbegin(); }
	const_reverse_iterator rend() const noexcept { return (const_reverse_iterator)_map.rend(); }

private:
	std::map<K, V, std::less<K>, STLAlloc<std::pair<const K, V>>> _map;
};
template<class K, class V>
bool Map<K, V>::Add(const K& k, const V& v) noexcept
{
	std::pair<typename std::map<K, V, std::less<K>, STLAlloc<std::pair<const K, V>>>::iterator, bool> ret;
	ret = _map.insert(std::pair<K, V>(k, v));
	return ret.second;
}
template<class K, class V>
bool Map<K, V>::TryGet(const K& k, V** v) noexcept
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
bool Map<K, V>::TryGet(const K& k, const V** v) const noexcept
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
bool Map<K, V>::TryGet(const K& k, V* v)const noexcept
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
