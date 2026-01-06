 
/*****************************************************************************
* Author： hannibal
* Date：2021/7/12
* Description：编辑器主界面
*****************************************************************************/
#pragma once

#include "core/Object.h"
#include "EditorDefine.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class EditorMain : public Object
{
	friend class SceneManager;
	friend class EditorAppliction;
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(EditorMain);
	BEGIN_DERIVED_REFECTION_TYPE(EditorMain, Object)
	END_REFECTION_TYPE;

public:
	static ImFont* GetDefaultFont() { return _defaultFont; }
	static ImFont* GetBigFont() { return _bigFont; }
	static ImFont* GetBigIconFont() { return _bigIconFont; }

private:
	static void Initialize();
	static void Destroy();
	static void Render();
	static void Resize(const WindowResizeDesc& desc);

private:
	static void CreateDock();		//主停靠面板


private:
	inline static ImFont* _defaultFont = nullptr;
	inline static ImFont* _bigFont = nullptr;
	inline static ImFont* _bigIconFont = nullptr;
};
DC_END_NAMESPACE
