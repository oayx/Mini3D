 
/*****************************************************************************
* Author： hannibal
* Date：2021/9/3
* Description：游戏视图
*****************************************************************************/
#pragma once

#include "editor/EWindowBase.h"

DC_BEGIN_NAMESPACE
class RenderTexture;
/********************************************************************/
class EMain_GameView : public EWindowBase
{
	friend class EditorMain;
	friend class EMain_Menu;
	friend class EditorAppliction;
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(EMain_GameView);
	BEGIN_DERIVED_REFECTION_TYPE(EMain_GameView, EWindowBase)
	END_REFECTION_TYPE;

private:
	static void Update();
	static void Render();
	static void Resize();

	static void ShowToolbar();

public:
	static bool IsFocus() { return _isFocus; }
	static Size GetWindowSize(float width, float height);
	static const Rect& GetViewPort() { return _viewPort; }

	static void			  CreateRenderTexture();
	static void			  EnableMSAA(bool enable) { _isEnableMSAA = enable;  _isRTDirty = true; }
	static RenderTexture* GetRenderTexture() { return _renderTexture; }

private:
	inline static bool	IsShow = true;
	inline static bool	_isRTDirty = true;
	inline static bool	_isEnableMSAA = false;
	inline static bool	_isFocus = false;
	inline static Rect	_viewPort = Rect::zero;		//坐标系：左上角(0,0)
	inline static Rect	_oldViewPort = Rect::zero;
	inline static RenderTexture* _renderTexture = nullptr;
};
DC_END_NAMESPACE