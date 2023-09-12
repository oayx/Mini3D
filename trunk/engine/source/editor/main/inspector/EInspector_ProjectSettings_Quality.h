 
/*****************************************************************************
* Author： hannibal
* Date：2021/9/25
* Description：质量设置
*****************************************************************************/
#pragma once

#include "EInspector_Base.h"
#include "editor/EWindowBase.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class EInspector_ProjectSettings_Quality : public EInspector_Base
{
public:
	virtual void Render()override;
};
DC_END_NAMESPACE