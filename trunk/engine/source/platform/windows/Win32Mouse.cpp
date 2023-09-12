#include "Win32Mouse.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(Win32Mouse, Object);
void Win32Mouse::ClipToWindow(HWND hWnd)
{
	RECT rect;
	::GetClientRect(hWnd, &rect);

	POINT ul;
	ul.x = rect.left;
	ul.y = rect.top;

	POINT lr;
	lr.x = rect.right;
	lr.y = rect.bottom;

	::MapWindowPoints(hWnd, nullptr, &ul, 1);
	::MapWindowPoints(hWnd, nullptr, &lr, 1);

	rect.left = ul.x;
	rect.top = ul.y;

	rect.right = lr.x;
	rect.bottom = lr.y;

	::ClipCursor(&rect);
}
bool Win32Mouse::IsConnected()
{
	return ::GetSystemMetrics(SM_MOUSEPRESENT) != 0;
}
bool Win32Mouse::IsVisible()
{
	CURSORINFO info = { sizeof(CURSORINFO), 0, nullptr, {} };
	if (!::GetCursorInfo(&info))
	{
		throw std::exception("GetCursorInfo");
	}

	return (info.flags & CURSOR_SHOWING) != 0;
}
bool Win32Mouse::SetVisible(bool visible)
{
	CURSORINFO info = { sizeof(CURSORINFO), 0, nullptr, {} };
	if (!::GetCursorInfo(&info))
	{
		return false;
	}

	bool isvisible = (info.flags & CURSOR_SHOWING) != 0;
	if (isvisible != visible)
	{
		::ShowCursor(visible);
	}
	return true;
}
DC_END_NAMESPACE
