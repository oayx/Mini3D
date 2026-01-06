 
/*****************************************************************************
* Author： hannibal
* Date：2020/3/9
* Description： 窗口
*****************************************************************************/
#pragma once
#if defined(DC_GRAPHICS_API_VULKAN)
#include "runtime/graphics/vulkan/VKDefine.h"
#include "platform/common/glfw/GLFWRenderWindow.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class Win32VKRenderWindow final : public GLFWRenderWindow
{
	friend class WindowManager;
	DEFAULT_CREATE(Win32VKRenderWindow);
	FRIEND_CONSTRUCT_DESTRUCT(Win32VKRenderWindow);
	DISALLOW_COPY_ASSIGN(Win32VKRenderWindow);
	BEGIN_DERIVED_REFECTION_TYPE(Win32VKRenderWindow, GLFWRenderWindow)
	END_REFECTION_TYPE;

protected:
	Win32VKRenderWindow();
	~Win32VKRenderWindow();

public:
	virtual bool Create(WindowDesc& info)override;
};
DC_END_NAMESPACE
#endif