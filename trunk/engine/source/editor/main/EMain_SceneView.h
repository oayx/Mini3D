 
/*****************************************************************************
* Author： hannibal
* Date：2021/9/2
* Description：场景视图
*****************************************************************************/
#pragma once

#include "editor/EWindowBase.h"

DC_BEGIN_NAMESPACE
class Camera;
class Transform;
class LineRender;
class EInspector_Base;
/********************************************************************/
class EMain_SceneView : public EWindowBase
{
	friend class EditorMain;
	friend class EMain_Menu;
	friend class EditorAppliction;
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(EMain_SceneView);
	BEGIN_DERIVED_REFECTION_TYPE(EMain_SceneView, EWindowBase)
	END_DERIVED_REFECTION_TYPE;

private:
	static void Update();
	static void Render();
	
	static void ShowToolbar();
	static void ShowSelectObject();
	static void ShowLineGrid();

	static void Set3D(bool is_3d);
	static void HandleInput();

public:
	static Camera* GetCamera();
	static bool IsFocus() { return _isFocus; }
	static void OnDoubleClick(Transform* node);

private:
	static const int	TEXTURE_SIZE = 2048;
	static bool			IsShow;
	static bool			_isFocus;
	static bool			_isForcus;		//获得焦点
	static bool			_isMouseIn;	//鼠标在窗口
	static Camera*		_camera;		//编辑模式相机
	static LineRender*	_lineGrid;		//网格
	static Rect			_viewPort;		//视口
	static Rect			_oldViewPort;
};
DC_END_NAMESPACE