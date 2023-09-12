 
/*****************************************************************************
* Author： hannibal
* Date：2021/9/9
* Description：灯光
*****************************************************************************/
#pragma once

#include "editor/EWindowBase.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class ELightSetting : public EWindowBase
{
	friend class EMain_Menu;

public:
	static void Render();

private:
	static bool IsShow;
};
DC_END_NAMESPACE