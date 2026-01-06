 
/*****************************************************************************
* Author： hannibal
* Date：2021/8/7
* Description：公共组件
*****************************************************************************/
#pragma once

#include "editor/EditorDefine.h"

DC_BEGIN_NAMESPACE
class Texture;
/********************************************************************/
class ECommonComponent
{
public:
	//帮组提示
	static void HelpMarker(const char* desc);
	//样式选择
	static bool ShowStyleSelector(const char* label);
	//字体选择
	static void ShowFontSelector(const char* label);
	//颜色
	static bool ShowColor(const char* label, float* init_color, bool hdr);
	//Cullmask
	static bool ShowCullmask(const char* label, uint& cullmask);
	//弹出式下拉列表
	static bool ShowPopupList(const char* label, int& curr_select, const VecString& list);
	//文件图标
	static bool ShowFileIcon(const char* label, bool isSelected, Texture* icon, const Vector2& icon_size);
	//添加组件
	static void ShowAddComponent(std::function<void(String)> action, const EComponents& parentComponent);
	//创建对象
	static void ShowCreateObject();
	//创建资源
	static void ShowCreateAsset();
	//下拉列表
	static bool ShowDropDown(const char* label, const VecString& list, int& curr_select);

	static bool ToggleButton(const char* str_id, bool* v);
};
DC_END_NAMESPACE