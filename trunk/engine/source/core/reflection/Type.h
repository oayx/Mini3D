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
	friend class Memory;

	int _flags;								//标记位
    std::string _name;						//名称
	std::string _editorComponent;			//编辑器组件
    const Type* _baseType;					//基类
	std::vector<Constructor*> _constructors;//构造函数

public:
    Type()
	{
		this->_flags = 0;
		this->_name = "Unregistered_Type";
		this->_editorComponent = "";
		this->_baseType = nullptr;
    }
	Type(const Type& other) = delete;
	Type(Type&& other) = delete;
	Type& operator=(const Type& other) = delete;
	Type& operator=(Type&& other) = delete;
	~Type();

public:
	bool Is(const Type* type) const 
	{
		if (type == this)
			return true;

		if (_baseType != nullptr)
			return _baseType->Is(type);

		return false;
	}

	const std::string& GetName() const noexcept { return _name; }
	const Type* GetBaseType() const noexcept { return _baseType; }

	const Constructor* GetConstructor() const noexcept
	{ 
		if (_constructors.empty())return nullptr; 
		return _constructors[0]; 
	}
	Type* AddConstructors(const std::vector<Constructor*>& _constructors) noexcept
	{
		for (auto c : _constructors)
		{
			this->_constructors.push_back(c);
		}
		return this;
	}

	Type* InsertFlags(TypeFlag flags) noexcept
	{
		_flags |= (int)flags;
		return this;
	}
	Type* RemoveFlags(TypeFlag flags) noexcept
	{
		_flags ^= (int)flags;
		return this;
	}
	constexpr bool HasFlag(TypeFlag flag)const
	{
		return _flags & (int)flag;
	}

	Type* AddEditorComponent(const std::string& group) noexcept
	{
		_flags |= (int)TypeFlag::ShowInEditor;
		_editorComponent = group;
		return this;
	}
	const std::string& GetEditorComponent()const noexcept
	{
		return _editorComponent;
	}

public:
	static std::unordered_map<std::string, Type*>& GetTypes() noexcept
	{
		static std::unordered_map<std::string, Type*> types;
		return types;
	}
	//普通类型+类
	static Type* RegisterType(Type* type, const std::string& _name, const Type* _baseType) noexcept;
	static const Type* GetType(const std::string& typeName) noexcept;
	//返回所有反射到的类型字符串
	static void Output() noexcept;
	//程序结束时释放所有反射的对象
	static void Destroy() noexcept;
};
DC_END_NAMESPACE