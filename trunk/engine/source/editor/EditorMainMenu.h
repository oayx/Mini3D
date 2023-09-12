 
/*****************************************************************************
* Author： hannibal
* Date：2020/9/12
* Description：运行时场景信息
*****************************************************************************/
#pragma once

#include "core/Object.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class EditorMainMenu : public Object
{
	friend class EditorAppliction;
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(EditorMainMenu);
	BEGIN_DERIVED_REFECTION_TYPE(EditorMainMenu, Object)
	END_DERIVED_REFECTION_TYPE;

private:
	static void Render();
};
DC_END_NAMESPACE
