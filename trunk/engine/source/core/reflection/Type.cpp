#include "Type.h"
#include "Constructor.h"
#include "core/memory/Memory.h"
#include <sstream>

DC_BEGIN_NAMESPACE
/********************************************************************/
Type::~Type()
{
	m_name = "";
	m_baseType = nullptr;
	for (auto obj : m_constructors)
	{
		SAFE_DELETE(obj);
	}
	m_constructors.clear();
}
Type* Type::RegisterType(Type* type, const std::string& name, const Type* m_baseType)
{
	static std::unordered_map<std::string, Type*>& types = GetTypes();
	if (types.find(name) != types.end())
		return (Type*)(types.find(name)->second);//由于win32静态变量在dll不共享，可能会导致多份，这里修改后支持dll中使用

	if (type)
	{
		type->m_name = name;
		type->m_baseType = m_baseType;
	}
	types.insert(std::make_pair(name, type));

	return type;
}
const Type* Type::GetType(const std::string& typeName)
{
	static std::unordered_map<std::string, Type*>& types = GetTypes();
	auto type = types[typeName];
	if (!type && typeName.find('.') >= 0)
	{
		type = types[typeName.substr(typeName.find('.') + 1)];
	}
	return type;
}
void Type::Output()
{
	std::stringstream ss;
	for (auto t : GetTypes())
	{
		ss << t.first << ",";
	}
	Debuger::Log("%s", ss.str().c_str());
}
void Type::Destroy()
{
	static std::unordered_map<std::string, Type*>& types = GetTypes();
	for (auto obj : types)
	{
		SAFE_DELETE(obj.second);
	}
	types.clear();
}
DC_END_NAMESPACE