 
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

	const std::string& GetName() const noexcept { return _className; }
	bool IsSame(const tRttiObject &rtti) const noexcept { return this->_className == rtti._className; }
	bool Is(const tRttiObject &rtti) const noexcept
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

DC_END_NAMESPACE
