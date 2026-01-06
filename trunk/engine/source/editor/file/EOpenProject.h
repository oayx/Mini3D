 
/*****************************************************************************
* Author： hannibal
* Date：2021/9/4
* Description：打开项目
*****************************************************************************/
#pragma once

#include "editor/EWindowBase.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class EOpenProject : public EWindowBase
{
	friend class EMain_Menu;

public:
	static void Render();

private:
	inline static bool IsShow = true;
};
DC_END_NAMESPACE