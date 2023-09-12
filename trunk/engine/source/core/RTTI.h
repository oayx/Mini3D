 
/*****************************************************************************
* Author： hannibal
* Date：2020/9/23
* Description：运行时类型反射
*****************************************************************************/
#pragma once

#include <string>
#include "BaseMacro.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL tRttiObject
{
public:
	explicit tRttiObject(const char *className): _className(className), _baseRtti(NULL) {}
	tRttiObject(const char *className, const tRttiObject &baseRTTI): _className(className), _baseRtti(&baseRTTI) {}

	const std::string& GetName() const { return _className; }
	bool IsSame(const tRttiObject &rtti) const { return this->_className == rtti._className; }
	bool Is(const tRttiObject &rtti) const
	{
		const tRttiObject *pCompare = this;
		while (pCompare)
		{
			if (pCompare->_className == rtti._className) return true;
			pCompare = pCompare->_baseRtti;
		}
		return false;
	}

private:
	const std::string _className;
	const tRttiObject* _baseRtti;
};

////声明
//#define REFECTION_TYPE(TYPE)	\
//public: \
//static const tRttiObject rtti; \
//const tRttiObject* GetClassType() const; \
//private:
//
//#define VIRTUAL_REFECTION_TYPE(TYPE)	\
//public: \
//static const tRttiObject rtti; \
//virtual const tRttiObject* GetClassType() const; \
//private:
//
//#define DERIVED_REFECTION_TYPE(TYPE, BASE) \
//typedef BASE base; \
//public: \
//static const tRttiObject rtti; \
//virtual const tRttiObject* GetClassType() const override; \
//private:

//实现
//#define IMPL_REFECTION_TYPE(TYPE) \
//const tRttiObject TYPE::rtti(#TYPE); \
//const tRttiObject* TYPE::GetClassType() const { return &rtti; }
//
//#define IMPL_TEMPLATE_REFECTION_TYPE(TYPE) \
//template<typename T> \
//const tRttiObject TYPE<T>::rtti(#TYPE); \
//template<typename T> \
//const tRttiObject* TYPE<T>::GetClassType() const { return &rtti; }
//
//#define IMPL_DERIVED_REFECTION_TYPE(TYPE, BASE) \
//const tRttiObject TYPE::rtti(#TYPE, BASE::rtti); \
//const tRttiObject* TYPE::GetClassType() const { return &rtti; }

DC_END_NAMESPACE
