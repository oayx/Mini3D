 
/*****************************************************************************
* Author： hannibal
* Date：2021/8/13
* Description：工具栏
*****************************************************************************/
#pragma once

#include "editor/EWindowBase.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class EMain_Tools : public EWindowBase
{
	friend class EditorMain;
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(EMain_Tools);
	BEGIN_DERIVED_REFECTION_TYPE(EMain_Tools, EWindowBase)
	END_DERIVED_REFECTION_TYPE;

public:
	static GizmoOperation GetOperation();

private:
	static void Render();

	static void ShowLeftToolbar();
	static void ShowRightToolbar();
	static void ShowPlayButton();

private:
	static void ResetOperation();

private:
	static bool _gizmoOperation[(int)GizmoOperation::Max];//移动+旋转+缩放
};
DC_END_NAMESPACE