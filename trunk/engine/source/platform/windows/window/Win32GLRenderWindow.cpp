#if defined(DC_GRAPHICS_API_OPENGL)
#include "Win32GLRenderWindow.h"
#include "../Win32Mouse.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(Win32GLRenderWindow, GLFWRenderWindow);
Win32GLRenderWindow::Win32GLRenderWindow()
{
}
Win32GLRenderWindow::~Win32GLRenderWindow()
{
}
bool Win32GLRenderWindow::Create(WindowDesc& info)
{
	bool ret = base::Create(info);
	if (ret)
	{
		if (!gladLoadGL())
		{
			Debuger::Error("Win32GLRenderWindow::Create - gladLoadGL");
			return false;
		}
	}
		
	return ret;
}
void Win32GLRenderWindow::OnFocus(bool focused)
{
	base::OnFocus(focused);
	if (_mouseClip)
	{
		HWND hWnd = (HWND)this->GetHwnd();
		Win32Mouse::ClipToWindow(focused ? hWnd : nullptr);
	}
}
DC_END_NAMESPACE
#endif