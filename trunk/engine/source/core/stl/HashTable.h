
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
	int operator()(const std::string & key)
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
	bool operator()(const std::string & A, const std::string & B)
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
	HashNode& operator=(const HashNode& node)
	{
		_key = node._key;
		_value = node._value;
		next = node.next;
		return *this;
	}
};
/********************************************************************/
template <class Key, class Value, class HashFunc, class EqualKey>
class HashTable Final : public object
{
	DISALLOW_COPY_ASSIGN(HashTable);

public:
	HashTable(int size);
	~HashTable();
	bool Insert(const Key& key, const Value& value);
	bool Remove(const Key& key);
	Value& Find(const Key& key);
	Value& operator [](const Key& key);

private:
	HashFunc m_hash;
	EqualKey m_equal;
	HashNode<Key, Value> **m_table;
	uint m_size;
	Value ValueNULL;
};

template <class Key, class Value, class HashFunc, class EqualKey>
HashTable<Key, Value, HashFunc, EqualKey>::HashTable(int size) : m_size(size), m_hash(), m_equal()
{
	m_table = new HashNode<Key, Value>*[m_size];
	for (unsigned i = 0; i < m_size; i++)
		m_table[i] = NULL;
}
template <class Key, class Value, class HashFunc, class EqualKey>
HashTable<Key, Value, HashFunc, EqualKey>::~HashTable()
{
	for (unsigned i = 0; i < m_size; i++)
	{
		HashNode<Key, Value> *currentNode = m_table[i];
		while (currentNode)
		{
			HashNode<Key, Value> *temp = currentNode;
			currentNode = currentNode->next;
			delete temp;
		}
	}
	delete[] m_table;
}
template <class Key, class Value, class HashFunc, class EqualKey>
bool HashTable<Key, Value, HashFunc, EqualKey>::Insert(const Key& key, const Value& value)
{
	int index = m_hash(key) % m_size;
	HashNode<Key, Value> * node = new HashNode<Key, Value>(key, value);
	node->next = m_table[index];
	m_table[index] = node;
	return true;
}
template <class Key, class Value, class HashFunc, class EqualKey>
bool HashTable<Key, Value, HashFunc, EqualKey>::Remove(const Key& key)
{
	unsigned index = m_hash(key) % m_size;
	HashNode<Key, Value> * node = m_table[index];
	HashNode<Key, Value> * prev = NULL;
	while (node)
	{
		if (node->_key == key)
		{
			if (prev == NULL)
			{
				m_table[index] = node->next;
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
Value& HashTable<Key, Value, HashFunc, EqualKey>::Find(const Key& key)
{
	unsigned  index = m_hash(key) % m_size;
	if (m_table[index] == NULL)
		return ValueNULL;
	else
	{
		HashNode<Key, Value> * node = m_table[index];
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
Value& HashTable<Key, Value, HashFunc, EqualKey>::operator [](const Key& key)
{
	return Find(key);
}
DC_END_NAMESPACE