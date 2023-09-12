 
/*****************************************************************************
* Author： hannibal
* Date：2021/8/7
* Description：层级界面
*****************************************************************************/
#pragma once

#include "editor/EWindowBase.h"

DC_BEGIN_NAMESPACE
class Scene;
class Transform;
/********************************************************************/
class EMain_Hierarchy : public EWindowBase
{
	friend class EditorMain;
	friend class EMain_Menu;

	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(EMain_Hierarchy);
	BEGIN_DERIVED_REFECTION_TYPE(EMain_Hierarchy, EWindowBase)
	END_DERIVED_REFECTION_TYPE;

private:
	static void Render();

	static void ShowSceneGraph();
	static void ShowScene(Scene* scene, const char* filter);
	static bool ShowNode(Transform* node);
	static bool ShowSearchNode(Transform* node, const char* filter);
	static void ShowRightMenu(const String& node);

	static void HandleShortcutKey();
	static void CopySelectObject();
	static void DeleteSelectObject();

private:
	static bool IsShow;
	static bool	_isFocus;
};
DC_END_NAMESPACE