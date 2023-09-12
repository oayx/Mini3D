/*****************************************************************************************************/
// @author hannibal
// @date   2021/04/29
// @brief  类型
/*****************************************************************************************************/
#pragma once

#include <list>
#include <vector>
#include <unordered_map>
#include "core/memory/Memory.h"

DC_BEGIN_NAMESPACE
class Type;
class Constructor;
/********************************************************************/
enum class TypeFlag : int
{
	Null					= 0,
	AllowMultipleComponent	= 0x1,	//相同组件是否可以同时多个在一个Gameobject上
	ShowInEditor			= 0x2,	//组件是否显示在编辑器
};
/********************************************************************/
template<class T>
struct ReflectType
{
	static const Type* Value()
	{
		return T::StaticType();
	}
};
template<>
struct ReflectType<std::nullptr_t>
{
	static const Type* Value()
	{
		return nullptr;
	}
};
#define typeof(TYPE) ReflectType<TYPE>::Value()
/********************************************************************/
class ENGINE_DLL Type 
{
	DECLARE_ALLOCATOR;

	int m_flags;							//标记位
    std::string m_name;						//名称
	std::string m_editor_component;			//编辑器组件
    const Type* m_baseType;					//基类
	std::vector<Constructor*> m_constructors;//构造函数

public:
    Type()
	{
		this->m_flags = 0;
		this->m_name = "Unregistered_Type";
		this->m_editor_component = "";
		this->m_baseType = nullptr;
    }
	~Type();

	bool Is(const Type* type) const 
	{
		if (type == this)
			return true;

		if (m_baseType != nullptr)
			return m_baseType->Is(type);

		return false;
	}

	const std::string& GetName() const  { return m_name; }
	const Type* GetBaseType() const  { return m_baseType; }

	const Constructor* GetConstructor() const 
	{ 
		if (m_constructors.empty())return nullptr; 
		return m_constructors[0]; 
	}
	Type* AddConstructors(const std::vector<Constructor*>& m_constructors)
	{
		for (auto c : m_constructors)
		{
			this->m_constructors.push_back(c);
		}
		return this;
	}

	Type* InsertFlags(TypeFlag flags)
	{
		m_flags |= (int)flags;
		return this;
	}
	Type* RemoveFlags(TypeFlag flags)
	{
		m_flags ^= (int)flags;
		return this;
	}
	bool HasFlag(TypeFlag flag)const
	{
		return m_flags & (int)flag;
	}

	Type* AddEditorComponent(const std::string& group)
	{
		m_flags |= (int)TypeFlag::ShowInEditor;
		m_editor_component = group;
		return this;
	}
	const std::string& GetEditorComponent()const
	{
		return m_editor_component;
	}

public:
	static std::unordered_map<std::string, Type*>& GetTypes()
	{
		static std::unordered_map<std::string, Type*> types;
		return types;
	}
	//普通类型+类
	static Type* RegisterType(Type* type, const std::string& m_name, const Type* m_baseType);
	static const Type* GetType(const std::string& typeName);
	//返回所有反射到的类型字符串
	static void Output();
	//程序结束时释放所有反射的对象
	static void Destroy();
};
DC_END_NAMESPACE