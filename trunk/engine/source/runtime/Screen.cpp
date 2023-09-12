#include "Screen.h"
#include "Application.h"
#include "platform/WindowManager.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_REFECTION_TYPE(Screen);
float Screen::m_width = 0.0f;
float Screen::m_height = 0.0f;
bool Screen::m_fullscreen = false;
void Screen::SetResolution(int w, int h, bool fullscreen)
{
	if (WindowManager::GetWindow(TargetDisplay::Display1) == nullptr)
		return;

	WindowResizeDesc desc(WindowManager::GetWindow(TargetDisplay::Display1)->GetHwnd(), w, h, fullscreen, true);
	Application::Resize(desc);
}
bool Screen::ShowCursor(bool show)
{
	return false;
}
bool Screen::SetOrientation(ScreenOrientation orientation)
{
	return false;
}
DC_END_NAMESPACE
