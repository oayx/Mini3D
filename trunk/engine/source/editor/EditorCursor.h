
/*****************************************************************************
* Author： hannibal
* Description：
*****************************************************************************/
#pragma once

#include "core/Object.h"
#include "external/imgui/imgui.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
//鼠标
class EditorCursor final
{
	friend class GUIContext;

public:
	static bool Update();

	static void EndRender();	//一帧结束后的清理工作

	static void BeginDrag(const String& file);
	static void EndDrag();
	static const String& GetDragFile() { return _dragFile; }

	static void AddFiles(const std::wstring& files) { _addFiles.Add(files); }
	static const Vector<std::wstring>& GetAddFiles() { return _addFiles; }

private:
	inline static bool _isDraging = false;
	inline static ImGuiMouseCursor	_lastMouseCursor = ImGuiMouseCursor_COUNT;

	inline static String _dragFile = "";
	inline static Vector<std::wstring> _addFiles;		//拖动到编辑器的文件
};
DC_END_NAMESPACE