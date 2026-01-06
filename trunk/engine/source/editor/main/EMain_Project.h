 
/*****************************************************************************
* Author： hannibal
* Date：2021/8/7
* Description：项目资源面板
*****************************************************************************/
#pragma once

#include "editor/EWindowBase.h"

DC_BEGIN_NAMESPACE
class AssetMeta;
/********************************************************************/
class EMain_Project : public EWindowBase
{
	friend class EditorMain;
	friend class EMain_Menu;

	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(EMain_Project);
	BEGIN_DERIVED_REFECTION_TYPE(EMain_Project, EWindowBase)
	END_REFECTION_TYPE;

public:
	static void SetSelectFile(const String& filePath);
	static const String& GetSelectPath() { return _selectPath; }
	static const String& GetSelectFile() { return _selectFile; }

private:
	static void Render();

	static void ShowLeft(const Vector<AssetMeta*>& dirs, String selectPath);

	static void ShowRight();
	static void ShowFiles();
	static bool IsIgnoreExt(const String& ext);

	static void ShowRightMenu(const String& node);

	static void DragFileToEditor();

private:
	inline static bool IsShow = true;
	inline static bool _isInputMode = false;	//修改名称
	inline static String _selectPath = "";
	inline static String _selectFile = "";
};
DC_END_NAMESPACE