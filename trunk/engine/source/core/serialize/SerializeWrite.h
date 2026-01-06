
/*****************************************************************************
* Author： hannibal
* Description：写入
*****************************************************************************/
#pragma once

#include "SerializeBase.h"
#include "SerializeTraits.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class SerializeWrite : public SerializeBase
{
	DEFAULT_CREATE(SerializeWrite);
	FRIEND_CONSTRUCT_DESTRUCT(SerializeWrite);
	DISALLOW_COPY_ASSIGN(SerializeWrite);
	BEGIN_DERIVED_REFECTION_TYPE(SerializeWrite, SerializeBase)
	END_REFECTION_TYPE;

public:
	SerializeWrite(const String& file);
	~SerializeWrite();

	virtual bool IsRead()const override { return false; }

	void Save();

	inline bool Push(const char* nodeName)
	{
		if (!_isOpen || !_currNode)return false;

		_currNode = _currNode->InsertNewChildElement(nodeName);
		_isSaved = false;
		return true;
	}
	inline bool Push(const char* nodeName, int index)
	{
		if (!_isOpen || !_currNode)return false;

		tinyxml2::XMLElement* curr_node = _currNode;

		//获取当前已经有的数量
		_currNode = _currNode->FirstChildElement(nodeName);
		while (_currNode && index > 0)
		{
			index--;
			_currNode = _currNode->NextSiblingElement(nodeName);
		}

		//如果不够，创建新的
		while(index-- >= 0 || !_currNode)_currNode = curr_node->InsertNewChildElement(nodeName);

		_isSaved = false;
		return true;
	}
	inline void Pop()
	{
		if (!_isOpen)return;

		AssertEx(_currNode->Parent(), "Already is root node");
		_currNode = _currNode->Parent()->ToElement();
	}

	template<class T>
	inline void Transfer(T& data, void* ptr)
	{
		if (!_isOpen)return;

		char* name = RemovePre((char*)ptr);

		tinyxml2::XMLElement* curr_node = _currNode;
		_currNode = _currNode->InsertNewChildElement(name);
		SerializeTraits<T>::Transfer(data, name, *this);
		_currNode = curr_node;
		_isSaved = false;
	}

	template<class T>
	inline void TransferEnum(T& data, void* ptr)
	{
		if (!_isOpen)return;

		char* name = RemovePre((char*)ptr);

		tinyxml2::XMLElement* curr_node = _currNode;
		_currNode = _currNode->InsertNewChildElement(name);
		SerializeTraits<T>::TransferEnum(data, name, *this);
		_currNode = curr_node;
		_isSaved = false;
	}

	template<class T>
	inline void TransferClass(T& data, void* ptr)
	{
		if (!_isOpen)return;
		if (!_currNode)return;//可能Push了无效节点

		char* name = RemovePre((char*)ptr);

		tinyxml2::XMLElement* curr_node = _currNode;
		_currNode = _currNode->InsertNewChildElement(name);
		SerializeTraits<T>::Transfer(data, name, *this);
		_currNode = curr_node;
		_isSaved = false;
	}

	template<class T>
	inline void TransferVector(T& data, void* ptr)
	{
		if (!_isOpen)return;
		if (!_currNode)return;

		char* name = RemovePre((char*)ptr);

		SerializeTraits<T>::Transfer(data, name, *this);
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
	bool _isSaved = false;
};

template<>
inline void SerializeWrite::TransferBasicData<bool>(bool& data, void* name)
{
	_currNode->SetText(data ? "true" : "false");
}
template<>
inline void SerializeWrite::TransferBasicData<char>(char& data, void* name)
{
	_currNode->SetText(data);
}
template<>
inline void SerializeWrite::TransferBasicData<byte>(byte& data, void* name)
{
	_currNode->SetText(data);
}
template<>
inline void SerializeWrite::TransferBasicData<short>(short& data, void* name)
{
	_currNode->SetText(data);
}
template<>
inline void SerializeWrite::TransferBasicData<ushort>(ushort& data, void* name)
{
	_currNode->SetText(data);
}
template<>
inline void SerializeWrite::TransferBasicData<int>(int& data, void* name)
{
	_currNode->SetText(data);
}
template<>
inline void SerializeWrite::TransferBasicData<uint>(uint& data, void* name)
{
	_currNode->SetText(data);
}
template<>
inline void SerializeWrite::TransferBasicData<int64>(int64& data, void* name)
{
	_currNode->SetText(data);
}
template<>
inline void SerializeWrite::TransferBasicData<uint64>(uint64& data, void* name)
{
	_currNode->SetText(data);
}
template<>
inline void SerializeWrite::TransferBasicData<float>(float& data, void* name)
{
	_currNode->SetText(data);
}
template<>
inline void SerializeWrite::TransferBasicData<double>(double& data, void* name)
{
	_currNode->SetText(data);
}
template<class T>
inline void SerializeWrite::TransferStringData(T& data, void* name)
{
	_currNode->SetText(data.c_str());
}
template<class T>
inline void SerializeWrite::TransferColorData(T& data, void* name)
{
	_currNode->SetText(data.ToString().c_str());
}
template<class T>
inline void SerializeWrite::TransferVector2Data(T& data, void* name)
{
	_currNode->SetText(data.ToString().c_str());
}
template<class T>
inline void SerializeWrite::TransferVector3Data(T& data, void* name)
{
	_currNode->SetText(data.ToString().c_str());
}
template<class T>
inline void SerializeWrite::TransferVector4Data(T& data, void* name)
{
	_currNode->SetText(data.ToString().c_str());
}
template<class T>
inline void SerializeWrite::TransferQuaternionData(T& data, void* name)
{
	_currNode->SetText(data.ToString().c_str());
}
template<class T>
inline void SerializeWrite::TransferSizeData(T& data, void* name)
{
	_currNode->SetText(data.ToString().c_str());
}
template<class T>
inline void SerializeWrite::TransferiSizeData(T& data, void* name)
{
	_currNode->SetText(data.ToString().c_str());
}
template<class T>
inline void SerializeWrite::TransferRectData(T& data, void* name)
{
	_currNode->SetText(data.ToString().c_str());
}
template<class T>
inline void SerializeWrite::TransferVectorData(Vector<T>& data, void* name)
{
	tinyxml2::XMLElement* curr_node = _currNode;
	for(int i = 0; i < data.Size(); ++i)
	{
		_currNode = curr_node->InsertNewChildElement((const char*)name);
		SerializeTraits<T>::Transfer(data[i], name, *this);
	}
	_currNode = curr_node;
}
DC_END_NAMESPACE