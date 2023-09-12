
/*****************************************************************************
* Author： hannibal
* Description：读取
*****************************************************************************/
#pragma once

#include "SerializeBase.h"
#include "SerializeTraits.h"
#include "core/Numberic.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class SerializeRead : public SerializeBase
{
	DEFAULT_CREATE(SerializeRead);
	FRIEND_CONSTRUCT_DESTRUCT(SerializeRead);
	DISALLOW_COPY_ASSIGN(SerializeRead);
	BEGIN_DERIVED_REFECTION_TYPE(SerializeRead, SerializeBase)
	END_DERIVED_REFECTION_TYPE;

public:
	SerializeRead(const String& file);
	~SerializeRead() {}

	void Reset();

	virtual bool IsRead()const override{ return true; }

	inline bool Push(const char* node_name)
	{
		if (!_isOpen || !_currNode)return false;

		_nodeStack.Push(_currNode);
		_currNode = _currNode->FirstChildElement(node_name);
		if (!_currNode)
		{
			_nodeStack.Pop(_currNode);
			return false;
		}
		return true;
	}
	inline bool Push(const char* node_name, int index)
	{
		if (!_isOpen || !_currNode)return false;

		_nodeStack.Push(_currNode);
		_currNode = _currNode->FirstChildElement(node_name);
		while (index > 0 && _currNode)
		{
			_currNode = _currNode->NextSiblingElement(node_name);
			--index;
		}
		if (!_currNode)
		{
			_nodeStack.Pop(_currNode);
			return false;
		}
		AssertEx(index == 0, "Index out range");
		return true;
	}
	inline void Pop()
	{
		if (!_isOpen || !_currNode)return;

		AssertEx(!_nodeStack.IsEmpty(), "Already is root node");
		_nodeStack.Pop(_currNode);
	}

	template<class T>
	inline void Transfer(T& data, void* ptr)
	{
		if (!_isOpen)return;
		if (!_currNode)return;//可能Push了无效节点

		char* name = RemovePre((char*)ptr);

		tinyxml2::XMLElement* curr_node = _currNode;
		_currNode = _currNode->FirstChildElement(name);
		if(_currNode)SerializeTraits<T>::Transfer(data, name, *this);
		_currNode = curr_node;
	}

	template<class T>
	inline void TransferEnum(T& data, void* ptr)
	{
		if (!_isOpen)return;
		if (!_currNode)return;//可能Push了无效节点

		char* name = RemovePre((char*)ptr);

		tinyxml2::XMLElement* curr_node = _currNode;
		_currNode = _currNode->FirstChildElement(name);
		if (_currNode)SerializeTraits<T>::TransferEnum(data, name, *this);
		_currNode = curr_node;
	}

	template<class T>
	inline void TransferClass(T& data, void* ptr)
	{
		if (!_isOpen)return;
		if (!_currNode)return;//可能Push了无效节点

		char* name = RemovePre((char*)ptr);

		tinyxml2::XMLElement* curr_node = _currNode;
		_currNode = _currNode->FirstChildElement((char*)name);
		if (_currNode)SerializeTraits<T>::Transfer(data, name, *this);
		_currNode = curr_node;
	}

	template<class T>
	inline void TransferVector(T& data, void* ptr)
	{
		if (!_isOpen)return;
		if (!_currNode)return;

		char* name = RemovePre((char*)ptr);

		tinyxml2::XMLElement* curr_node = _currNode;
		SerializeTraits<T>::Transfer(data, name, *this);
		_currNode = curr_node;
	}

	template<class T>
	void TransferBasicData(T& data, void* name);

	template<class T>
	void TransferStringData(T& data, void* name);

	template<class T>
	void TransferColorData(T& data, void* name);

	template<class T>
	void TransferVector2Data(T& data, void* name);
	template<class T>
	void TransferVector3Data(T& data, void* name);
	template<class T>
	void TransferVector4Data(T& data, void* name);

	template<class T>
	void TransferQuaternionData(T& data, void* name);

	template<class T>
	void TransferSizeData(T& data, void* name);
	template<class T>
	void TransferiSizeData(T& data, void* name);
	template<class T>
	void TransferRectData(T& data, void* name);

	template<class T>
	void TransferVectorData(Vector<T>& data, void* name);

private:
	Stack<tinyxml2::XMLElement*> _nodeStack;
};
template<>
inline void SerializeRead::TransferBasicData<bool>(bool& data, void* name)
{
	data = Bool::isTrue(_currNode->GetText());
}
template<>
inline void SerializeRead::TransferBasicData<char>(char& data, void* name)
{
	int i;
	::sscanf(_currNode->GetText(), "%d", &i);
	data = i;
}
template<>
inline void SerializeRead::TransferBasicData<byte>(byte& data, void* name)
{
	uint i;
	::sscanf(_currNode->GetText(), "%u", &i);
	data = i;
}
template<>
inline void SerializeRead::TransferBasicData<short>(short& data, void* name)
{
	::sscanf(_currNode->GetText(), "%hd", &data);
}
template<>
inline void SerializeRead::TransferBasicData<ushort>(ushort& data, void* name)
{
	::sscanf(_currNode->GetText(), "%hu", &data);
}
template<>
inline void SerializeRead::TransferBasicData<int>(int& data, void* name)
{
	::sscanf(_currNode->GetText(), "%d", &data);
}
template<>
inline void SerializeRead::TransferBasicData<uint>(uint& data, void* name)
{
	::sscanf(_currNode->GetText(), "%u", &data);
}
template<>
inline void SerializeRead::TransferBasicData<int64>(int64& data, void* name)
{
	data = std::stoll(_currNode->GetText());
}
template<>
inline void SerializeRead::TransferBasicData<uint64>(uint64& data, void* name)
{
	data = std::stoull(_currNode->GetText());
}
template<>
inline void SerializeRead::TransferBasicData<float>(float& data, void* name)
{
	data = std::stof(_currNode->GetText());
}
template<>
inline void SerializeRead::TransferBasicData<double>(double& data, void* name)
{
	data = std::stod(_currNode->GetText());
}
template<class T>
inline void SerializeRead::TransferStringData(T& data, void* name)
{
	const char* str = _currNode->GetText();
	if (str)
	{
		data = str;
	}
}
template<class T>
inline void SerializeRead::TransferColorData(T& data, void* name)
{
	data = T(_currNode->GetText());
}
template<class T>
inline void SerializeRead::TransferVector2Data(T& data, void* name)
{
	data = T(_currNode->GetText());
}
template<class T>
inline void SerializeRead::TransferVector3Data(T& data, void* name)
{
	data = T(_currNode->GetText());
}
template<class T>
inline void SerializeRead::TransferVector4Data(T& data, void* name)
{
	data = T(_currNode->GetText());
}
template<class T>
inline void SerializeRead::TransferQuaternionData(T& data, void* name)
{
	data = T(_currNode->GetText());
}
template<class T>
inline void SerializeRead::TransferSizeData(T& data, void* name)
{
	data = T(_currNode->GetText());
}
template<class T>
inline void SerializeRead::TransferiSizeData(T& data, void* name)
{
	data = T(_currNode->GetText());
}
template<class T>
inline void SerializeRead::TransferRectData(T& data, void* name)
{
	data = T(_currNode->GetText());
}
template<class T>
inline void SerializeRead::TransferVectorData(Vector<T>& data, void* name)
{
	_currNode = _currNode->FirstChildElement((const char*)name);
	while (_currNode)
	{
		T _data;
		SerializeTraits<T>::Transfer(_data, name, *this);
		data.Add(_data);

		_currNode = _currNode->NextSiblingElement((const char*)name);
	}
}
DC_END_NAMESPACE