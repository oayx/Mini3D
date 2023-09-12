 
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
	END_DERIVED_REFECTION_TYPE;

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
	static bool			  IsShow;
	static bool			  _isRTDirty;
	static bool			  _isEnableMSAA;
	static bool			  _isFocus;
	static Rect			  _viewPort;		//坐标系：左上角(0,0)
	static Rect			  _oldViewPort;
	static RenderTexture* _renderTexture;
};
DC_END_NAMESPACE