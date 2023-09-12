 
/*****************************************************************************
* Author： hannibal
* Date：2021/8/9
* Description：样式设置
*****************************************************************************/
#pragma once

#include "editor/EWindowBase.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class EIMGUISetter : public EWindowBase
{
	friend class EMain_Menu;

public:
	static void InitStyle();
	static void ResetStyle();

	static void InitLayout();
	static void SetLayout(LayoutType type);

	static void Render();
private:
	static bool IsShow;
};
DC_END_NAMESPACE