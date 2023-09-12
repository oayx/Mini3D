/*****************************************************************************************************/
// @author hannibal
// @date   2021/04/29
// @brief  反射
/*****************************************************************************************************/
#pragma once

#include "Type.h"
#include "Constructor.h"

DC_BEGIN_NAMESPACE

/********************************************************************/
//头文件声明-有继承关系
#define BEGIN_DERIVED_REFECTION_TYPE(TYPE, BASE)  \
typedef BASE base; \
public: \
    static Type* type; \
    static Type* StaticType(){ \
        if (type == nullptr){ \
            type = DBG_NEW Type(); \
            type = Type::RegisterType(const_cast<Type*>(typeof(TYPE)), #TYPE, typeof(BASE))

#define BEGIN_REFECTION_TYPE(TYPE)				BEGIN_DERIVED_REFECTION_TYPE(TYPE, std::nullptr_t)

//头文件声明-结束
#define END_REFECTION_TYPE ; \
        } \
        return type; \
    } \
    virtual Type* GetType() const{ \
            return StaticType(); \
    } \
private:
#define END_DERIVED_REFECTION_TYPE ; \
        } \
        return type; \
    } \
    virtual Type* GetType() const override{ \
            return StaticType(); \
    } \
private:
#define END_FINAL_REFECTION_TYPE ; \
        } \
        return type; \
    } \
    Type* GetType() const{ \
            return StaticType(); \
    } \
private:

//实现
#define IMPL_REFECTION_TYPE(TYPE)				Type* TYPE::type = nullptr;static const Type* g_typeof_##TYPE = typeof(TYPE);
#define IMPL_DERIVED_REFECTION_TYPE(TYPE, BASE)	Type* TYPE::type = nullptr;static const Type* g_typeof_##TYPE = typeof(TYPE);
#define IMPL_TEMPLATE_REFECTION_TYPE(TYPE)		template<typename T> Type* TYPE<T>::type = nullptr;

#define CTORS(...)								->AddConstructors(std::vector<Constructor*>{__VA_ARGS__})
#define DEFAULT_CTOR(TYPE)                      DBG_NEW ConstructorImpl<TYPE>()
//#define CTOR(TYPE, ...)                       DBG_NEW ConstructorImpl<TYPE, __VA_ARGS__>()
#define INSERT_TYPE_FLAGS(flags)				->InsertFlags(flags)
#define REMOVE_TYPE_FLAGS(flags)				->RemoveFlags(flags)
#define ADD_EDITOR_COMPONENT(group)				->AddEditorComponent(group)

DC_END_NAMESPACE