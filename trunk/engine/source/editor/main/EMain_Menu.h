 
/*****************************************************************************
* Author： hannibal
* Date：2021/8/7
* Description：主菜单
*****************************************************************************/
#pragma once

#include "editor/EWindowBase.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class EMain_Menu : public EWindowBase
{
	friend class EditorMain;
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(EMain_Menu);
	BEGIN_DERIVED_REFECTION_TYPE(EMain_Menu, EWindowBase)
	END_REFECTION_TYPE;

private:
	static void Render();

private:
};
DC_END_NAMESPACE