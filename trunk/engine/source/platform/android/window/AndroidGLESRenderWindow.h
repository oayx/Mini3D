
/*****************************************************************************
* Author： hannibal
* Date：2020/3/9
* Description： opengles
*****************************************************************************/
#pragma once
#if defined(DC_GRAPHICS_API_OPENGLES3)
 
#include "runtime/graphics/opengl/GLDefine.h"
#include "platform/RenderWindow.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
// Android window
class AndroidGLESRenderWindow Final : public RenderWindow
{
	friend class WindowManager;
	DEFAULT_CREATE(AndroidGLESRenderWindow);
	FRIEND_CONSTRUCT_DESTRUCT(AndroidGLESRenderWindow);
	DISALLOW_COPY_ASSIGN(AndroidGLESRenderWindow);
	BEGIN_DERIVED_REFECTION_TYPE(AndroidGLESRenderWindow, RenderWindow)
	END_DERIVED_REFECTION_TYPE;

protected:
	AndroidGLESRenderWindow();
	~AndroidGLESRenderWindow();

public:
	virtual bool Create(WindowDesc& info)override;
	virtual void Resize(WindowResizeDesc& desc)override;
	virtual int  OnWinMsg(uint uMsg, int64 param1, int64 param2, int64 param3, int64 param4)override { return 0; }
	virtual void Draw()override;
	virtual void* GetHwnd()override { return m_hWnd; }

private:
	EGLNativeWindowType m_hWnd;
};
DC_END_NAMESPACE
#endif