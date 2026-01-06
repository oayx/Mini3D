 
/*****************************************************************************
* Author： hannibal
* Date：2025/5/19
* Description： opengles
*****************************************************************************/
#pragma once

#include "platform/common/glfw/GLFWRenderWindow.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class MacGLRenderWindow final : public GLFWRenderWindow
{
	friend class WindowManager;
	DEFAULT_CREATE(MacGLRenderWindow);
	FRIEND_CONSTRUCT_DESTRUCT(MacGLRenderWindow);
	DISALLOW_COPY_ASSIGN(MacGLRenderWindow);
	BEGIN_DERIVED_REFECTION_TYPE(MacGLRenderWindow, GLFWRenderWindow)
	END_REFECTION_TYPE;

protected:
	MacGLRenderWindow();
	~MacGLRenderWindow();

public:
	virtual bool Create(WindowDesc& info)override;
};
DC_END_NAMESPACE
