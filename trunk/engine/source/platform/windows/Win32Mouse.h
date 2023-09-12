 
/*****************************************************************************
* Author： hannibal
* Date：2020/2/12
* Description：鼠标
*****************************************************************************/
#pragma once

#include "core/Object.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class Win32Mouse Final : public Object
{
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(Win32Mouse);
	BEGIN_DERIVED_REFECTION_TYPE(Win32Mouse, Object)
	END_DERIVED_REFECTION_TYPE;

public:
	static void ClipToWindow(HWND hWnd);//限制鼠标活动范围
	static bool IsConnected();
	static bool IsVisible();
	static bool SetVisible(bool visible);
};
DC_END_NAMESPACE
