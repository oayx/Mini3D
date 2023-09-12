 
/*****************************************************************************
* Author： hannibal
* Date：2021/8/7
* Description：属性面板
*****************************************************************************/
#pragma once

#include "editor/EWindowBase.h"

DC_BEGIN_NAMESPACE
class EInspector_Base;
/********************************************************************/
class EMain_Inspector : public EWindowBase
{
	friend class EditorMain;
	friend class EMain_Menu;

	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(EMain_Inspector);
	BEGIN_DERIVED_REFECTION_TYPE(EMain_Inspector, EWindowBase)
	END_DERIVED_REFECTION_TYPE;

private:
	static void Initialize();
	static void Destroy();

	static void Render();

private:
	static bool IsShow;
	static InspectorType    _inspectorType;
	static EInspector_Base* _inspectors[(int)InspectorType::Max];
};
DC_END_NAMESPACE