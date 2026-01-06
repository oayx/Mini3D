 
/*****************************************************************************
* Author： hannibal
* Date：2020/3/9
* Description： 窗口
*****************************************************************************/
#pragma once
#if defined(DC_GRAPHICS_API_OPENGL)
#include "platform/common/glfw/GLFWRenderWindow.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class Win32GLRenderWindow final : public GLFWRenderWindow
{
	friend class WindowManager;
	DEFAULT_CREATE(Win32GLRenderWindow);
	FRIEND_CONSTRUCT_DESTRUCT(Win32GLRenderWindow);
	DISALLOW_COPY_ASSIGN(Win32GLRenderWindow);
	BEGIN_DERIVED_REFECTION_TYPE(Win32GLRenderWindow, GLFWRenderWindow)
	END_REFECTION_TYPE;

protected:
	Win32GLRenderWindow();
	~Win32GLRenderWindow();

public:
	virtual bool Create(WindowDesc& info)override;

protected:
	virtual void OnFocus(bool focused)override;
};
DC_END_NAMESPACE
#endif