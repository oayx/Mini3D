 
/*****************************************************************************
* Author： hannibal
* Date：2025/5/13
* Description： opengles
*****************************************************************************/
#pragma once

#include "platform/common/glfw/GLFWRenderWindow.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class iOSGLRenderWindow final : public GLFWRenderWindow
{
	friend class WindowManager;
	DEFAULT_CREATE(iOSGLRenderWindow);
	FRIEND_CONSTRUCT_DESTRUCT(iOSGLRenderWindow);
	DISALLOW_COPY_ASSIGN(iOSGLRenderWindow);
	BEGIN_DERIVED_REFECTION_TYPE(iOSGLRenderWindow, GLFWRenderWindow)
	END_REFECTION_TYPE;

protected:
	iOSGLRenderWindow();
	~iOSGLRenderWindow();

public:
	virtual bool Create(WindowDesc& info)override;
};
DC_END_NAMESPACE
