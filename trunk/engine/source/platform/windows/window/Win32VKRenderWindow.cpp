#if defined(DC_GRAPHICS_API_VULKAN)
#include "Win32VKRenderWindow.h"
#include "runtime/graphics/vulkan/VKDevice.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(Win32VKRenderWindow, GLFWRenderWindow);
Win32VKRenderWindow::Win32VKRenderWindow()
{
}
Win32VKRenderWindow::~Win32VKRenderWindow()
{
}
bool Win32VKRenderWindow::Create(WindowDesc& info)
{
	bool ret = base::Create(info);
	if (ret)
	{
		if (!glfwVulkanSupported())
		{
			Debuger::Error("Win32VKRenderWindow::Create - glfwVulkanSupported");
			return false;
		}
	}
		
	return ret;
}
DC_END_NAMESPACE
#endif