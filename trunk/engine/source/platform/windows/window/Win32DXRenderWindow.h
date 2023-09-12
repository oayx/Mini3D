 
/*****************************************************************************
* Author： hannibal
* Date：2009/11/24
* Description： win窗口
*****************************************************************************/
#pragma once

#include "platform/RenderWindow.h"

DC_BEGIN_NAMESPACE
LRESULT CALLBACK _WndProc(HWND hWnd, uint uMsg, WPARAM wParam, LPARAM lParam);
/********************************************************************/
class Win32DXRenderWindow Final : public RenderWindow
{
	friend class WindowManager;
	DEFAULT_CREATE(Win32DXRenderWindow);
	FRIEND_CONSTRUCT_DESTRUCT(Win32DXRenderWindow);
	DISALLOW_COPY_ASSIGN(Win32DXRenderWindow);
	BEGIN_DERIVED_REFECTION_TYPE(Win32DXRenderWindow, RenderWindow)
	END_DERIVED_REFECTION_TYPE;

protected:
	Win32DXRenderWindow();
	~Win32DXRenderWindow();

public:
	virtual bool Create(WindowDesc& info)override;
	virtual void Resize(WindowResizeDesc& desc)override;
	virtual int  OnWinMsg(uint uMsg, int64 param1, int64 param2, int64 param3, int64 param4)override;

	virtual void* GetHwnd()override { return m_hWnd; }

protected:
	virtual void OnFocus(bool focused)override;

private:
	static void Initialize();
	static void Destroy();
	static void PeekMessage();
	static void InitConsole();
	static void ReleaseConsole();

protected:
	static bool _initialize;

	HWND m_hWnd = nullptr;
	bool m_isMinimized = false;
	bool m_isMaximized = false;
	bool m_isResizing = false;
	bool m_isIgnoreSizeChange = false;//忽略大小改变消息
};
DC_END_NAMESPACE
