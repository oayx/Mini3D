#include "Win32DXRenderWindow.h"
#include "core/time/Timer.h"
#include "runtime/input/Input.h"
#include "runtime/input/MouseInput.h"
#include "runtime/input/KeyboardInput.h"
#include "runtime/graphics/null/RenderContent.h"
#include "runtime/ui/GUI.h"
#include "runtime/Application.h"
#include "runtime/Screen.h"
#include "platform/windows/Win32Mouse.h"
#include "platform/WindowManager.h"
#include "editor/EditorAppliction.h"
#include "editor/EditorCursor.h"
#include <zmouse.h>//可以使用鼠标滑轮
#include <shellapi.h>

DC_BEGIN_NAMESPACE
/********************************************************************/
static int GetKeyCode(int wParam, int lParam)
{
	int key = -1;
	if (wParam >= 48 && wParam < 48 + 10)
	{//数字键
		key = (int)KeyCode::Alpha0 + wParam - 48;
	}
	else if (wParam >= VK_NUMPAD0 && wParam < VK_NUMPAD0 + 10)
	{//小键盘数字键
		key = (int)KeyCode::Keypad0 + wParam - VK_NUMPAD0;
	}
	else if (wParam >= 65 && wParam <= 65 + 'z' - 'a')
	{
		key = (int)KeyCode::A + wParam - 65;
	}
	else if (wParam >= VK_F1 && wParam < VK_F1 + 12)
	{//F1-F12
		key = (int)KeyCode::F1 + wParam - VK_F1;
	}
	else
	{
		switch (wParam)
		{
		case VK_CONTROL:
		{
			short state_l = ((unsigned short)GetKeyState(VK_LCONTROL)) >> 15;
			short state_r = ((unsigned short)GetKeyState(VK_RCONTROL)) >> 15;
			if (state_l)
			{
				key = (int)KeyCode::LeftControl;
			}
			else if (state_r)
			{
				key = (int)KeyCode::RightControl;
			}
			break;
		}
		case VK_SHIFT:
		{
			short state_l = ((unsigned short)GetKeyState(VK_LSHIFT)) >> 15;
			short state_r = ((unsigned short)GetKeyState(VK_RSHIFT)) >> 15;
			if (state_l)
			{
				key = (int)KeyCode::LeftShift;
			}
			else if (state_r)
			{
				key = (int)KeyCode::RightShift;
			}
			break;
		}
		case VK_MENU:
		{
			short state_l = ((unsigned short)GetKeyState(VK_LMENU)) >> 15;
			short state_r = ((unsigned short)GetKeyState(VK_RMENU)) >> 15;
			if (state_l)
			{
				key = (int)KeyCode::LeftAlt;
			}
			else if (state_r)
			{
				key = (int)KeyCode::RightAlt;
			}
			break;
		}
		case VK_RETURN:
		{
			if (lParam & 0x1000000L)
				key = (int)KeyCode::KeypadEnter;
			else
				key = (int)KeyCode::Return;
			break;
		}
		case VK_ESCAPE: key = (int)KeyCode::Escape; break;
		case VK_OEM_3: key = (int)KeyCode::BackQuote; break;
		case VK_OEM_MINUS: key = (int)KeyCode::Minus; break;
		case VK_OEM_PLUS: key = (int)KeyCode::Equals; break;
		case VK_BACK: key = (int)KeyCode::Backspace; break;

		case VK_TAB: key = (int)KeyCode::Tab; break;
		case VK_OEM_4: key = (int)KeyCode::LeftBracket; break;
		case VK_OEM_6: key = (int)KeyCode::RightBracket; break;
		case VK_OEM_5: key = (int)KeyCode::Backslash; break;

		case VK_CAPITAL: key = (int)KeyCode::CapsLock; break;
		case VK_OEM_1: key = (int)KeyCode::Semicolon; break;
		case VK_OEM_7: key = (int)KeyCode::Quote; break;
		case VK_OEM_COMMA: key = (int)KeyCode::Comma; break;
		case VK_OEM_PERIOD: key = (int)KeyCode::Period; break;
		case VK_OEM_2: key = (int)KeyCode::Slash; break;

		case VK_SPACE: key = (int)KeyCode::Space; break;

		case VK_SNAPSHOT: key = (int)KeyCode::PrintScreen; break;
		case VK_SCROLL: key = (int)KeyCode::ScrollLock; break;
		case VK_PAUSE: key = (int)KeyCode::Pause; break;

		case VK_NUMLOCK: key = (int)KeyCode::Numlock; break;
		case VK_DIVIDE: key = (int)KeyCode::KeypadDivide; break;
		case VK_MULTIPLY: key = (int)KeyCode::KeypadMultiply; break;
		case VK_SUBTRACT: key = (int)KeyCode::KeypadMinus; break;
		case VK_ADD: key = (int)KeyCode::KeypadPlus; break;
		case VK_DECIMAL: key = (int)KeyCode::KeypadPeriod; break;

		case VK_UP:key = (int)KeyCode::UpArrow; break;
		case VK_DOWN:key = (int)KeyCode::DownArrow; break;
		case VK_LEFT:key = (int)KeyCode::LeftArrow; break;
		case VK_RIGHT:key = (int)KeyCode::RightArrow; break;

		case VK_INSERT: key = (int)KeyCode::Insert; break;
		case VK_DELETE: key = (int)KeyCode::Delete; break;
		case VK_HOME: key = (int)KeyCode::Home; break;
		case VK_END: key = (int)KeyCode::End; break;
		case VK_PRIOR: key = (int)KeyCode::PageUp; break;
		case VK_NEXT: key = (int)KeyCode::PageDown; break;
		}
	}

	return key;
}
LRESULT CALLBACK WinAppProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	RenderWindow* window = WindowManager::GetWindow(hWnd);
	if (window)
	{
		return window->OnWinMsg(uMsg, (int64)wParam, (int64)lParam, 0, 0);
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(Win32DXRenderWindow, RenderWindow);
bool Win32DXRenderWindow::_initialize = false;
Win32DXRenderWindow::Win32DXRenderWindow()
{
	Initialize();
}
Win32DXRenderWindow::~Win32DXRenderWindow()
{
	if (m_hWnd != nullptr)
	{
		::DestroyWindow(m_hWnd);
		m_hWnd = nullptr;
	}
	Destroy();
}
bool Win32DXRenderWindow::Create(WindowDesc& info)
{
	//计算屏幕大小
	int screen_width = ::GetSystemMetrics(SM_CXSCREEN);
	int screen_height = ::GetSystemMetrics(SM_CYSCREEN);
	Platform::AdjustWindowRect(info.x, info.y, info.width, info.height, screen_width, screen_height, info.flags & WindowFlag::FullScreen);

	//创建窗口
	DWORD dwStyle = WS_OVERLAPPEDWINDOW;
	if (!(info.flags & WindowFlag::Resizable))dwStyle = dwStyle ^ WS_THICKFRAME;
	m_hWnd = ::CreateWindow(L"MainWnd", info.name.ToWString().c_str(), dwStyle, info.x, info.y, info.width, info.height, NULL, NULL, ::GetModuleHandle(NULL), this);
	if (!m_hWnd)
	{
		Debuger::Error("CreateWindow Failed");
		return false;
	}

	m_isIgnoreSizeChange = true;
	int show_flag = SW_SHOWDEFAULT;
	if (info.flags & WindowFlag::MaxSize) show_flag = SW_MAXIMIZE;
	::ShowWindow(m_hWnd, show_flag);
	::UpdateWindow(m_hWnd);
	if (Application::IsEditor())
	{//确定窗口是否接收文件拖拽。
		::DragAcceptFiles(m_hWnd, TRUE);
	}

	//注册鼠标消息
	RAWINPUTDEVICE rid;
	rid.usUsagePage = 0x1; /* HID_USAGE_PAGE_GENERIC */
	rid.usUsage = 0x2; /* HID_USAGE_GENERIC_MOUSE */
	rid.dwFlags = RIDEV_INPUTSINK;
	rid.hwndTarget = m_hWnd;
	if (!::RegisterRawInputDevices(&rid, 1, sizeof(RAWINPUTDEVICE)))
		Debuger::Error("Win32DXRenderWindow::Create - RegisterRawInputDevices");
	m_isIgnoreSizeChange = false;

	//最终有效区域大小
	RECT rect;
	::GetClientRect(m_hWnd, &rect);
	info.width = rect.right - rect.left; info.height = rect.bottom - rect.top;
	
	return base::Create(info);
}
void Win32DXRenderWindow::Resize(WindowResizeDesc& desc)
{	
	m_isIgnoreSizeChange = true;

	//使用之前设定
	if (!desc.fullscreen_valid)
	{
		desc.fullscreen = m_fullscreen;
	}

	//位置
	int left = 0, top = 0;
	if (!desc.fullscreen)
	{
		RECT rect;
		::GetWindowRect(m_hWnd, &rect);
		left = rect.left; top = rect.top;
	}

	//计算屏幕大小
	int screen_width = ::GetSystemMetrics(SM_CXSCREEN);
	int screen_height = ::GetSystemMetrics(SM_CYSCREEN);

	//限定窗口大小不超过屏幕尺寸
	int width = desc.width, height = desc.height;
	if (!desc.fullscreen)
	{
		if (width > screen_width || width <= 0) width = screen_width;
		if (height > screen_height || height <= 0) height = screen_height;
	}
	else
	{
		width = screen_width; height = screen_height;
	}

	//获得最终窗口大小
	RECT rect = { 0, 0, width, height };
	::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
	width = rect.right - rect.left;
	height = rect.bottom - rect.top;
	::MoveWindow(m_hWnd, left, top, width, height, true);

	//最终有效区域大小
	::GetClientRect(m_hWnd, &rect);
	desc.width = rect.right - rect.left; desc.height = rect.bottom - rect.top;

	m_isIgnoreSizeChange = false;
	base::Resize(desc);
}
void Win32DXRenderWindow::OnFocus(bool focused)
{
	base::OnFocus(focused);
	if (m_mouseClip)
	{
		Win32Mouse::ClipToWindow(focused ? m_hWnd : nullptr);
	}
}
#define GET_X_LPARAM(lp) ((float)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp) ((float)(short)HIWORD(lp))
int Win32DXRenderWindow::OnWinMsg(uint uMsg, int64 param1, int64 param2, int64 param3, int64 param4)
{
	DC_PROFILE_FUNCTION();
	WPARAM wParam = WPARAM(param1); LPARAM lParam = LPARAM(param2);
	int win_width = this->GetWidth(), win_height = this->GetHeight();
	switch (uMsg)
	{
	case WM_ACTIVATE:
		this->OnFocus(LOWORD(wParam) != WA_INACTIVE);
		break;
	case WM_CHAR://功能键如shift、ctrl等键不会产生WM_CHAR消息
		if (wParam > 0 && wParam < 0x10000)
		{
			ushort c = (ushort)wParam;
			KeyboardInput::AddInputChar(c);
		}
		break;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	{
		int key = GetKeyCode((int)wParam, (int)lParam);
		if (key >= 0)
		{
			KeyboardInput::HandleKeyDown(key);
		}
		else
		{
			Debuger::Log("unhandle down key %d", wParam);
		}
	}
	break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
	{
		int key = GetKeyCode((int)wParam, (int)lParam);
		if (key >= 0)
		{
			KeyboardInput::HandleKeyUp(key);
		}
		else
		{
			switch (wParam)
			{
			case VK_CONTROL:
			{
				if (Input::GetKey(KeyCode::LeftControl))
					KeyboardInput::HandleKeyUp((int)KeyCode::LeftControl);
				if (Input::GetKey(KeyCode::RightControl))
					KeyboardInput::HandleKeyUp((int)KeyCode::RightControl);
				break;
			}
			case VK_SHIFT:
			{
				if (Input::GetKey(KeyCode::LeftShift))
					KeyboardInput::HandleKeyUp((int)KeyCode::LeftShift);
				if (Input::GetKey(KeyCode::RightShift))
					KeyboardInput::HandleKeyUp((int)KeyCode::RightShift);
				break;
			}
			case VK_MENU:
			{
				if (Input::GetKey(KeyCode::LeftAlt))
					KeyboardInput::HandleKeyUp((int)KeyCode::LeftAlt);
				if (Input::GetKey(KeyCode::RightAlt))
					KeyboardInput::HandleKeyUp((int)KeyCode::RightAlt);
				break;
			}
			default:
			{
				Debuger::Log("unhandle up key %d", wParam);
				break;
			}
			}
		}
	}
	break;
	case WM_SYSCHAR:
		break;
	case WM_LBUTTONDOWN:
		SetCapture(m_hWnd);
		MouseInput::HandleLBtnDown(GET_X_LPARAM(lParam), win_height - GET_Y_LPARAM(lParam));
		break;
	case WM_RBUTTONDOWN:
		SetCapture(m_hWnd);
		MouseInput::HandleRBtnDown(GET_X_LPARAM(lParam), win_height - GET_Y_LPARAM(lParam));
		break;
	case WM_MBUTTONDOWN:
		SetCapture(m_hWnd);
		MouseInput::HandleMBtnDown(GET_X_LPARAM(lParam), win_height - GET_Y_LPARAM(lParam));
		break;
	case WM_LBUTTONUP:
		ReleaseCapture();
		MouseInput::HandleLBtnUp(GET_X_LPARAM(lParam), win_height - GET_Y_LPARAM(lParam));
		break;
	case WM_RBUTTONUP:
		ReleaseCapture();
		MouseInput::HandleRBtnUp(GET_X_LPARAM(lParam), win_height - GET_Y_LPARAM(lParam));
		break;
	case WM_MBUTTONUP:
		ReleaseCapture();
		MouseInput::HandleMBtnUp(GET_X_LPARAM(lParam), win_height - GET_Y_LPARAM(lParam));
		break;
	case WM_LBUTTONDBLCLK:
		ReleaseCapture();
		MouseInput::HandleLDClick(GET_X_LPARAM(lParam), win_height - GET_Y_LPARAM(lParam));
		break;
	case WM_RBUTTONDBLCLK:
		ReleaseCapture();
		MouseInput::HandleRDClick(GET_X_LPARAM(lParam), win_height - GET_Y_LPARAM(lParam));
		break;
	case WM_MBUTTONDBLCLK:
		ReleaseCapture();
		MouseInput::HandleMDClick(GET_X_LPARAM(lParam), win_height - GET_Y_LPARAM(lParam));
		break;
	case WM_MOUSEMOVE:
		MouseInput::HandleInputPosition(GET_X_LPARAM(lParam), win_height - GET_Y_LPARAM(lParam));
		break;
	case WM_MOUSEWHEEL://向前是正，向后是负
	{
		int z_delta = 0;
		z_delta = (int)HIWORD(wParam);
		if (z_delta >= 32767) z_delta -= 65536;
		z_delta = z_delta / 120; //转换成整数(1, 2, 3)
		MouseInput::HandleInputWheel(0, (float)z_delta);
	}
	break;
	case WM_INPUT:
	{
		if (m_isPaused)break;

		RAWINPUT raw;
		UINT rawSize = sizeof(raw);
		UINT resultData = ::GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, &raw, &rawSize, sizeof(RAWINPUTHEADER));
		if (resultData == UINT(-1))Debuger::Error("GetRawInputData");

		if (raw.header.dwType == RIM_TYPEMOUSE)
		{
			if (!(raw.data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE))
			{//鼠标移动
				float x = (float)raw.data.mouse.lLastX;
				float y = (float)-raw.data.mouse.lLastY;
				MouseInput::HandleInputMove(x, y);
			}
		}
	}
	break;
	case WM_SIZE:
	{
		Debuger::Log("resize");
		if (m_isIgnoreSizeChange)
			break;

		int width = LOWORD(lParam);
		int height = HIWORD(lParam);
		if (wParam == SIZE_MINIMIZED)
		{
			m_isMaximized = false;
			m_isMinimized = true;
			m_isPaused = true;
		}
		else if (wParam == SIZE_MAXIMIZED)
		{//Window maximized
			m_isMaximized = true;
			m_isMinimized = false;
			Application::Resize(WindowResizeDesc(m_hWnd, width, height, false, false));
		}
		else if (wParam == SIZE_RESTORED)
		{//Window restored
			if (m_isMaximized)
			{//From maximized to restore
				m_isMaximized = false;
				m_isPaused = false;
				Application::Resize(WindowResizeDesc(m_hWnd, width, height, false, false));
			}
			else if (m_isMinimized)
			{//From minimized to restore
				m_isMinimized = false;
				m_isPaused = false;
				Application::Resize(WindowResizeDesc(m_hWnd, width, height, false, false));
			}
			else
			{
				if (!m_isResizing)
				{
					Application::Resize(WindowResizeDesc(m_hWnd, width, height, false, false));
				}
			}
		}
	}
	break;
	case WM_SETCURSOR:
	{
		if (LOWORD(lParam) == HTCLIENT && Application::IsEditor())
		{
			if(EditorCursor::Update())
			return 1;
		}
	}
	break;
	case WM_DROPFILES:
	{//拖动文件
		//这里再设置一次鼠标位置，有可能拖动文件时窗口丢失焦点
		POINT point;
		GetCursorPos(&point);				// 获取鼠标指针位置（屏幕坐标）
		ScreenToClient(m_hWnd, &point);		// 将鼠标指针位置转换为窗口坐标
		MouseInput::HandleInputPosition(point.x, win_height - point.y);

		HDROP hDrop = (HDROP)wParam;
		UINT nFileNum = ::DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
		wchar_t strFileName[MAX_PATH];
		for (int i = 0; i < nFileNum; i++)
		{//获得拖拽的第i个文件名
			::DragQueryFile(hDrop, i, strFileName, MAX_PATH);
			EditorCursor::AddFiles(strFileName);
		}
		::DragFinish(hDrop);
	}
	break;
	case WM_DESTROY:
		//WM_CLOSE和WM_DESTROY同属窗口消息，WM_QUIT则不在窗口消息之列。三者先后执行顺序是WM_CLOSE、WM_DESTROY、WM_QUIT但需要注意的是MFC窗口默认取消按钮函数OnCancel()是不会触发WM_CLOSE消息，只触发WM_DESTROY。
		//WM_CLOSE	: 在系统菜单里选择了“关闭”或者点击了窗口右上角的“X”按钮，你的窗口过程就会收到WM_CLOSE。DefWindowProc对WM_CLOSE的处理是调用DestroyWindow。当然，你可以不让DefWindowProc处理，而是自己处理，例如询问用户是否保存更改等。如果用户选择“取消”，你忽略此消息，那么程序照常运行；如果用户确认要退出，你就调用DestroyWindow。
		//WM_DESTROY: DestroyWindow完成窗口的清理工作，最后像窗口过程发送WM_DESTROY。对于WM_DESTROY，DefWindowProc不会处理。也就是说，你如果不处理这个消息，虽然你的窗口已经销毁，但进程并不会结束。一般处理WM_DESTROY时都是释放资源（例如申请的内存等），然后调用PostQuitMessage。
		//WM_QUIT	: PostQuitMessage会发送WM_QUIT给消息队列。注意，WM_QUIT永远不会到达窗口过程，因为GetMessage得到WM_QUIT后就会返回FALSE，从而结束消息循环，最后进程结束，程序退出。
		WindowManager::CloseWindow(m_hWnd);
		break;
	}
	return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
}
#undef GET_WIN_SIZE
#undef GET_X_LPARAM
#undef GET_Y_LPARAM

void Win32DXRenderWindow::Initialize()
{
	if (_initialize)return;
	InitConsole();

	//注册窗口类
	WNDCLASS wc;
	wc.style = CS_DBLCLKS;
	wc.lpfnWndProc = WinAppProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = ::GetModuleHandle(NULL);
	wc.hIcon = ::LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = nullptr;//这里不要设置，否则IMGUI设置的鼠标会失效
	wc.hbrBackground = (HBRUSH)::GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = L"MainWnd";
	if (!::RegisterClass(&wc))
	{
		Debuger::Error("RegisterClass Failed.");
	}

	_initialize = true;
}
void Win32DXRenderWindow::Destroy()
{
	if (!_initialize)return;

	ReleaseConsole();
	::UnregisterClass(L"MainWnd", ::GetModuleHandle(NULL));

	_initialize = false;
}
void Win32DXRenderWindow::PeekMessage()
{
	DC_PROFILE_FUNCTION();
	MSG msg = {};
	while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
	{//这里一定要用while，否则帧率低的时候可能存在响应延迟
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
}
void Win32DXRenderWindow::InitConsole()
{
#if DC_DEBUG
	::AllocConsole();
	freopen("CONOUT$", "w+t", stdout);// 申请写
	freopen("CONIN$", "r+t", stdin);  // 申请读
#endif
}
void Win32DXRenderWindow::ReleaseConsole()
{
#if DC_DEBUG
	//TODO:退出时会抛错
	//::FreeConsole();
#endif
}
DC_END_NAMESPACE