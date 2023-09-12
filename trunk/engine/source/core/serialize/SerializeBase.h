
/*****************************************************************************
* Author： hannibal
* Description：序列化
*****************************************************************************/
#pragma once

#include "core/String.h"
#include "core/stl/Stack.h"
#include "external/tinyxml2/tinyxml2.h"

namespace tinyxml2
{
	class XMLElement;
}

DC_BEGIN_NAMESPACE
/********************************************************************/
#define TRANSFER_SIMPLE(x) transfer.Transfer(x, (char*)#x)
#define TRANSFER_ENUM(x) transfer.TransferEnum(x, (char*)#x)
#define TRANSFER_CLASS(x) transfer.TransferClass(x, (char*)((x).GetClassId().c_str()))
#define TRANSFER_VECTOR(x, c) transfer.TransferVector(x, (char*)c)

#define DECLARE_SERIALIZE(x) \
	public: \
	template<class TransferFunction> void Transfer (TransferFunction& transfer, void* name = nullptr); \
	private: 

#define DECLARE_OBJECT_SERIALIZE(x) \
	public: \
	template<class TransferFunction> void Transfer (TransferFunction& transfer, void* name = nullptr); \
	virtual void Serialize(SerializeRead& transfer, void* ptr)override { Transfer(transfer, ptr); } \
	virtual void Serialize(SerializeWrite& transfer, void* ptr)override { Transfer(transfer, ptr); } \
	virtual void Deserialize(SerializeRead& transfer, void* ptr)override { Transfer(transfer, ptr); } \
	virtual void Deserialize(SerializeWrite& transfer, void* ptr)override { Transfer(transfer, ptr); } \
	private: 

#define INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(x)	\
template void x::Transfer(SerializeRead& transfer, void* name); \
template void x::Transfer(SerializeWrite& transfer, void* name);

/********************************************************************/
class SerializeBase : public object
{
	DISALLOW_COPY_ASSIGN(SerializeBase);
	BEGIN_REFECTION_TYPE(SerializeBase)
	END_REFECTION_TYPE;

public:
	SerializeBase(const String& file) { _filePath = file; }
	~SerializeBase() {}

	virtual bool IsRead()const { return false; }

	char* RemovePre(char* name)
	{
		if (Strncmp(name, "m_", 2) == 0)name += 2;
		if (Strncmp(name, "_", 1) == 0)name += 1;
		if (*name >= 'a' && *name <= 'z')
		{
			int len = ::strlen(name);
			Assert(len + 1 < 1024);
			::strncpy(_tempChar, name, len);
			_tempChar[len] = '\0';
			_tempChar[0] = ::toupper(_tempChar[0]);
			return _tempChar;
		}
		return name;
	}

protected:
	bool	_isOpen = false;
	String	_filePath = "";
	char	_tempChar[1024];
	tinyxml2::XMLDocument _docment;
	tinyxml2::XMLElement* _rootNode = nullptr;
	tinyxml2::XMLElement* _currNode = nullptr;
};
DC_END_NAMESPACE