
/*****************************************************************************************************/
// @author hannibal
// @date   2017/04/2
// @brief  哈希表
/*****************************************************************************************************/
#pragma once

#include "core/BaseType.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class HashFunc_String
{
public:
	int operator()(const std::string & key) noexcept
	{
		int hash = 0;
		for (int i = 0; i < key.length(); ++i)
		{
			hash = hash << 7 ^ key[i];
		}
		return (hash & 0x7FFFFFFF);
	}
};


class EqualKey_String
{
public:
	bool operator()(const std::string & A, const std::string & B) noexcept
	{
		if (A.compare(B) == 0)
			return true;
		else
			return false;
	}
};
/********************************************************************/
template<class Key, class Value>
class HashNode
{
public:
	Key    _key;
	Value  _value;
	HashNode *next;

	HashNode(Key key, Value value)
	{
		_key = key;
		_value = value;
		next = NULL;
	}
	~HashNode()
	{

	}
	HashNode& operator=(const HashNode& node) noexcept
	{
		_key = node._key;
		_value = node._value;
		next = node.next;
		return *this;
	}
};
/********************************************************************/
template <class Key, class Value, class HashFunc, class EqualKey>
class HashTable final : public object
{
	DISALLOW_COPY_ASSIGN(HashTable);

public:
	HashTable(int size);
	~HashTable();
	bool Insert(const Key& key, const Value& value) noexcept;
	bool Remove(const Key& key) noexcept;
	Value& Find(const Key& key) noexcept;
	Value& operator [](const Key& key) noexcept;

private:
	HashFunc _hash;
	EqualKey _equal;
	HashNode<Key, Value> **_table;
	uint _size;
	Value ValueNULL;
};

template <class Key, class Value, class HashFunc, class EqualKey>
HashTable<Key, Value, HashFunc, EqualKey>::HashTable(int size) : _size(size), _hash(), _equal()
{
	_table = new HashNode<Key, Value>*[_size];
	for (unsigned i = 0; i < _size; i++)
		_table[i] = NULL;
}
template <class Key, class Value, class HashFunc, class EqualKey>
HashTable<Key, Value, HashFunc, EqualKey>::~HashTable()
{
	for (unsigned i = 0; i < _size; i++)
	{
		HashNode<Key, Value> *currentNode = _table[i];
		while (currentNode)
		{
			HashNode<Key, Value> *temp = currentNode;
			currentNode = currentNode->next;
			delete temp;
		}
	}
	delete[] _table;
}
template <class Key, class Value, class HashFunc, class EqualKey>
bool HashTable<Key, Value, HashFunc, EqualKey>::Insert(const Key& key, const Value& value) noexcept
{
	int index = _hash(key) % _size;
	HashNode<Key, Value> * node = new HashNode<Key, Value>(key, value);
	node->next = _table[index];
	_table[index] = node;
	return true;
}
template <class Key, class Value, class HashFunc, class EqualKey>
bool HashTable<Key, Value, HashFunc, EqualKey>::Remove(const Key& key) noexcept
{
	unsigned index = _hash(key) % _size;
	HashNode<Key, Value> * node = _table[index];
	HashNode<Key, Value> * prev = NULL;
	while (node)
	{
		if (node->_key == key)
		{
			if (prev == NULL)
			{
				_table[index] = node->next;
			}
			else
			{
				prev->next = node->next;
			}
			delete node;
			return true;
		}
		prev = node;
		node = node->next;
	}
	return false;
}
template <class Key, class Value, class HashFunc, class EqualKey>
Value& HashTable<Key, Value, HashFunc, EqualKey>::Find(const Key& key) noexcept
{
	unsigned  index = _hash(key) % _size;
	if (_table[index] == NULL)
		return ValueNULL;
	else
	{
		HashNode<Key, Value> * node = _table[index];
		while (node)
		{
			if (node->_key == key)
				return node->_value;
			node = node->next;
		}
		return ValueNULL;
	}
}
template <class Key, class Value, class HashFunc, class EqualKey>
Value& HashTable<Key, Value, HashFunc, EqualKey>::operator [](const Key& key) noexcept
{
	return Find(key);
}
DC_END_NAMESPACE