 
/*****************************************************************************
* Author： hannibal
* Date：2020/3/9
* Description： opengles
*****************************************************************************/
#pragma once

#include "platform/common/glfw/GLFWRenderWindow.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class LinuxGLRenderWindow final : public GLFWRenderWindow
{
	friend class WindowManager;
	DEFAULT_CREATE(LinuxGLRenderWindow);
	FRIEND_CONSTRUCT_DESTRUCT(LinuxGLRenderWindow);
	DISALLOW_COPY_ASSIGN(LinuxGLRenderWindow);
	BEGIN_DERIVED_REFECTION_TYPE(LinuxGLRenderWindow, GLFWRenderWindow)
	END_REFECTION_TYPE;

protected:
	LinuxGLRenderWindow();
	~LinuxGLRenderWindow();

public:
	virtual bool Create(WindowDesc& info)override;
};
DC_END_NAMESPACE
