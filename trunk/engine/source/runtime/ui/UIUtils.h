
/*****************************************************************************
* Author： hannibal
* Date：2020/8/19
* Description：
*****************************************************************************/
#pragma once

#include "runtime/component/Component.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
#define CLICK_INTERVAL_TIME 0.5f
#define DCLICK_INTERVAL_TIME 0.5f
enum class UIRenderMode
{
	None,
	ScreenSpace_Overlay,
	ScreenSpace_Camera,
	WorldSpace
};
DECLARE_ENUM_OPERATORS(UIRenderMode);

DC_END_NAMESPACE