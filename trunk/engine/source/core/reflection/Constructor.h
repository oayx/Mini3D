/*****************************************************************************************************/
// @author hannibal
// @date   2021/04/29
// @brief  构造函数
/*****************************************************************************************************/
#pragma once

#include "core/memory/Memory.h"
#include "core/memory/Allocate.h"

DC_BEGIN_NAMESPACE
class Type;
class Object;
/********************************************************************/
class ENGINE_DLL Constructor
{
	DECLARE_ALLOCATOR;
    friend class Type;
protected:
    const Type* type;
    
    Constructor(const Type* type){ this->type = type; }

public:
	virtual ~Constructor() = default;
    const Type* GetType() const{  return type; }
	virtual Object* Invoke() const { return nullptr; }
};

template<class T>
class ENGINE_DLL ConstructorImpl : public Constructor
{
private:
    const Type* type = nullptr;

public:
    ConstructorImpl() : Constructor(typeof(T))
	{
    }

    virtual Object* Invoke() const override
	{
        return (Object*)Memory::New<T>();
    }
};
DC_END_NAMESPACE