 
/*****************************************************************************
* Author： hannibal
* Date：2021/8/21
* Description：属性-基础类
*****************************************************************************/
#pragma once

#include "editor/EditorDefine.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class EInspector_Base
{
public:
	virtual ~EInspector_Base() {}

	virtual void OnEnter() {}
	virtual void OnLeave() {}

	virtual void Render() = 0;
};
DC_END_NAMESPACE