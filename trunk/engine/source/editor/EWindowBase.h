 
/*****************************************************************************
* Author： hannibal
* Date：2021/8/7
* Description：窗口基类
*****************************************************************************/
#pragma once

#include "EditorDefine.h"
#include "core/Object.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class EWindowBase : public Object
{
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(EWindowBase);
	BEGIN_DERIVED_REFECTION_TYPE(EWindowBase, Object)
	END_REFECTION_TYPE;
};
DC_END_NAMESPACE