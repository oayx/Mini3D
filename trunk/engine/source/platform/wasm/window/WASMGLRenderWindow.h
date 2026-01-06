 
/*****************************************************************************
* Author： hannibal
* Date：2025/5/13
* Description： opengles
*****************************************************************************/
#pragma once

#include "platform/common/glfw/GLFWRenderWindow.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class WASMGLRenderWindow final : public GLFWRenderWindow
{
	friend class WindowManager;
	DEFAULT_CREATE(WASMGLRenderWindow);
	FRIEND_CONSTRUCT_DESTRUCT(WASMGLRenderWindow);
	DISALLOW_COPY_ASSIGN(WASMGLRenderWindow);
	BEGIN_DERIVED_REFECTION_TYPE(WASMGLRenderWindow, GLFWRenderWindow)
	END_REFECTION_TYPE;

protected:
	WASMGLRenderWindow();
	~WASMGLRenderWindow();

public:
	virtual bool Create(WindowDesc& info)override;
};
DC_END_NAMESPACE
