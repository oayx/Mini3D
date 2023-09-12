#if defined(DC_GRAPHICS_API_OPENGLES3)
#include "Win32GLESRenderWindow.h"
#include "platform/common/egl/EGL_Context.h"
#pragma comment(lib,"libEGL.lib")
#pragma comment(lib,"libGLESv2.lib")

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(Win32GLESRenderWindow, GLFWRenderWindow);
Win32GLESRenderWindow::Win32GLESRenderWindow()
{
}
Win32GLESRenderWindow::~Win32GLESRenderWindow()
{
	EGL_Context::Destroy();
}
bool Win32GLESRenderWindow::Create(WindowDesc& info)
{
	bool ret = base::Create(info);
	if (ret)
	{
		EGL_Context::Initialize((EGLNativeWindowType)GetHwnd(), 2, _antiAlias);
	}
		
	return ret;
}
void Win32GLESRenderWindow::Draw()
{
	base::Draw();
	EGL_Context::Draw();
}
DC_END_NAMESPACE
#endif