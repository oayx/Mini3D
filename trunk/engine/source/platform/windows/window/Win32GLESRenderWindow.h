 
/*****************************************************************************
* Author： hannibal
* Date：2020/3/9
* Description： 窗口
*****************************************************************************/
#pragma once
#if defined(DC_GRAPHICS_API_OPENGLES3)
#include "platform/common/glfw/GLFWRenderWindow.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class Win32GLESRenderWindow Final : public GLFWRenderWindow
{
	friend class WindowManager;
	DEFAULT_CREATE(Win32GLESRenderWindow);
	FRIEND_CONSTRUCT_DESTRUCT(Win32GLESRenderWindow);
	DISALLOW_COPY_ASSIGN(Win32GLESRenderWindow);
	BEGIN_DERIVED_REFECTION_TYPE(Win32GLESRenderWindow, GLFWRenderWindow)
	END_DERIVED_REFECTION_TYPE;

protected:
	Win32GLESRenderWindow();
	~Win32GLESRenderWindow();

public:
	virtual bool Create(WindowDesc& info)override;
	virtual void Draw()override;
};
DC_END_NAMESPACE
#endif