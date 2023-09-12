
/*****************************************************************************
* Author： hannibal
* Date：2021/8/8
* Description：
*****************************************************************************/
#pragma once

#include "core/Object.h"
#include "runtime/CommonDefine.h"
#include "external/imgui/ex/imgui_user.h"
#include "external/imgui/icon/IconsFontAwesome5.h"

DC_BEGIN_NAMESPACE

#define NO_ICON "     "
#define INACTIVE_TEXT_COLOR ImVec4(0.6f, 0.6f, 0.6f, 1.0f)	//未激活文字颜色

#ifdef _WIN32
	#define IM_NEWLINE  "\r\n"
#else
	#define IM_NEWLINE  "\n"
#endif

//操作方式
enum class GizmoOperation
{
	None = 0,//NOTE:需要从0开始
	Drag,
	Translate,
	Rotate,
	Scale,
	Universal,//Translate+Rotate+Scale

	Max
};
DECLARE_ENUM_OPERATORS(GizmoOperation);

//Inspector显示的主类型
enum class InspectorType
{
	None = 0,
	Hierarchy,
	Project,
	ProjectSettings_Layers,
	ProjectSettings_Physics,
	ProjectSettings_Quality,

	Max
};
DECLARE_ENUM_OPERATORS(InspectorType);

//布局类型
enum class LayoutType
{
	Default = 0,
	Split2_3,
};
DECLARE_ENUM_OPERATORS(LayoutType);

//组件
struct EComponents
{
	Vector<std::pair<String, String>> Components;
	Map<String, EComponents> Childrends;
};

//Game视图分辨率
enum class GameViewResolution
{
	Free = 0,
	Scale_5_4,	//5:4
	Scale_4_3,
	Scale_3_2,
	Scale_16_9,
	Scale_9_16,
};

DC_END_NAMESPACE