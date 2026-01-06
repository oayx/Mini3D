 
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
	END_REFECTION_TYPE;

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
	inline static constexpr int	TEXTURE_SIZE = 2048;
	inline static bool IsShow = true;
	inline static bool _isFocus = false;
	inline static bool _isForcus = false;		//获得焦点
	inline static bool _isMouseIn = false;	//鼠标在窗口
	inline static Camera* _camera = nullptr;		//编辑模式相机
	inline static LineRender* _lineGrid = nullptr;		//网格
	inline static Rect _viewPort = Rect::zero;		//视口
	inline static Rect _oldViewPort = Rect::zero;
};
DC_END_NAMESPACE