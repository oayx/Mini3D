/*****************************************************************************************************/
// @author hannibal
// @date   2021/04/29
// @brief  反射
/*****************************************************************************************************/
#pragma once

#include "Meta.h"
#include "Type.h"
#include "Constructor.h"

DC_BEGIN_NAMESPACE

/********************************************************************/
//头文件声明-有继承关系
#define BEGIN_DERIVED_REFECTION_TYPE(ClassName, SuperClass)  \
    using CurrentType = ClassName; \
    using base = SuperClass; \
private: \
    inline static Type* __type__ = nullptr; \
public: \
    static MetaClass& StaticClass() { \
        static MetaClass classInfo(#ClassName, &SuperClass::StaticClass()); \
        return classInfo; \
    } \
    static Type* StaticType(){ \
        if (__type__ == nullptr){ \
            __type__ = Memory::New<Type>(); \
            __type__ = Type::RegisterType(__type__, #ClassName, typeof(SuperClass))

#define BEGIN_REFECTION_TYPE(ClassName)  \
    using CurrentType = ClassName; \
    using base = std::nullptr_t; \
private: \
    inline static Type* __type__ = nullptr; \
public: \
    static MetaClass& StaticClass() { \
        static MetaClass classInfo(#ClassName, nullptr); \
        return classInfo; \
    } \
    static Type* StaticType(){ \
        if (__type__ == nullptr){ \
            __type__ = Memory::New<Type>(); \
            __type__ = Type::RegisterType(__type__, #ClassName, nullptr)

//头文件声明-结束
#define END_REFECTION_TYPE ; \
        } \
        return __type__; \
    } \
    virtual Type* GetType() const{ \
            return StaticType(); \
    } \
    virtual MetaClass* GetClass() const { \
        return &StaticClass(); \
    } \
    inline static const Type* g_typeof_##CurrentType = typeof(CurrentType); \
private:

#define END_FINAL_REFECTION_TYPE ; \
        } \
        return __type__; \
    } \
    Type* GetType() const{ \
            return StaticType(); \
    } \
    MetaClass* GetClass() const { \
        return &StaticClass(); \
    } \
    inline static const Type* g_typeof_##CurrentType = typeof(CurrentType); \
private:

//实现
#define IMPL_REFECTION_TYPE(TYPE)				;
#define IMPL_DERIVED_REFECTION_TYPE(TYPE, BASE)	;

#define CTORS(...)								->AddConstructors(std::vector<Constructor*>{__VA_ARGS__})
#define DEFAULT_CTOR(TYPE)                      Memory::New<ConstructorImpl<TYPE>>()
//#define CTOR(TYPE, ...)                       Memory::New<ConstructorImpl<TYPE, __VA_ARGS__>>()
#define INSERT_TYPE_FLAGS(flags)				->InsertFlags(flags)
#define REMOVE_TYPE_FLAGS(flags)				->RemoveFlags(flags)
#define ADD_EDITOR_COMPONENT(group)				->AddEditorComponent(group)

DC_END_NAMESPACE