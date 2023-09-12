#include "GLFWRenderWindow.h"
#include "runtime/input/Input.h"
#include "runtime/input/MouseInput.h"
#include "runtime/input/KeyboardInput.h"
#include "runtime/Application.h"
#include "runtime/Screen.h"
#include "platform/WindowManager.h"

DC_BEGIN_NAMESPACE
static void* GetGLFWHwnd(GLFWwindow* window)
{
#if defined(DC_PLATFORM_WIN32)
	return glfwGetWin32Window(window);
//#elif defined(DC_PLATFORM_LINUX)
//	return glfwGetX11Window(window);
#else
	return window;
#endif
}

// 输入事件类型
enum class GLFWMsgType
{
	Key,		//输入
	Character,	//输入字符
	Mouse,		//鼠标操作
	MouseMove,	//鼠标移动
	Scroll,		//鼠标滑轮
	Resize,		//窗口大小改变
	Activate,	//最大+最小化
};
DECLARE_ENUM_OPERATORS(GLFWMsgType);

/********************************************************************/
static int GetKeyCode(int wParam)
{
	int key = -1;
	if (wParam >= GLFW_KEY_0 && wParam <= GLFW_KEY_9)
	{//数字键
		key = (int)KeyCode::Alpha0 + wParam - GLFW_KEY_0;
	}
	else if (wParam >= GLFW_KEY_KP_0 && wParam <= GLFW_KEY_KP_9)
	{//小键盘数字键
		key = (int)KeyCode::Keypad0 + wParam - GLFW_KEY_KP_0;
	}
	else if (wParam >= GLFW_KEY_A && wParam <= GLFW_KEY_A + 'z' - 'a')
	{//字母
		key = (int)KeyCode::A + wParam - GLFW_KEY_A;
	}
	else if (wParam >= GLFW_KEY_F1 && wParam <= GLFW_KEY_F12)
	{//F1-F12
		key = (int)KeyCode::F1 + wParam - GLFW_KEY_F1;
	}
	else
	{
		switch (wParam)
		{
		case GLFW_KEY_LEFT_CONTROL:key = (int)KeyCode::LeftControl; break;
		case GLFW_KEY_RIGHT_CONTROL:key = (int)KeyCode::RightControl; break;
		case GLFW_KEY_LEFT_SHIFT:key = (int)KeyCode::LeftShift; break;
		case GLFW_KEY_RIGHT_SHIFT:key = (int)KeyCode::RightShift; break;
		case GLFW_KEY_LEFT_ALT:key = (int)KeyCode::LeftAlt; break;
		case GLFW_KEY_RIGHT_ALT:key = (int)KeyCode::RightAlt; break;

		case GLFW_KEY_ESCAPE: key = (int)KeyCode::Escape; break;
		case GLFW_KEY_GRAVE_ACCENT: key = (int)KeyCode::BackQuote; break;
		case GLFW_KEY_MINUS: key = (int)KeyCode::Minus; break;
		case GLFW_KEY_EQUAL: key = (int)KeyCode::Equals; break;
		case GLFW_KEY_BACKSPACE: key = (int)KeyCode::Backspace; break;

		case GLFW_KEY_TAB: key = (int)KeyCode::Tab; break;
		case GLFW_KEY_LEFT_BRACKET: key = (int)KeyCode::LeftBracket; break;
		case GLFW_KEY_RIGHT_BRACKET: key = (int)KeyCode::RightBracket; break;
		case GLFW_KEY_BACKSLASH: key = (int)KeyCode::Backslash; break;

		case GLFW_KEY_CAPS_LOCK: key = (int)KeyCode::CapsLock; break;
		case GLFW_KEY_SEMICOLON: key = (int)KeyCode::Semicolon; break;
		case GLFW_KEY_APOSTROPHE: key = (int)KeyCode::Quote; break;
		case GLFW_KEY_ENTER: key = (int)KeyCode::Return; break;

		case GLFW_KEY_COMMA: key = (int)KeyCode::Comma; break;
		case GLFW_KEY_PERIOD: key = (int)KeyCode::Period; break;
		case GLFW_KEY_SLASH: key = (int)KeyCode::Slash; break;

		case GLFW_KEY_SPACE: key = (int)KeyCode::Space; break;

		case GLFW_KEY_PRINT_SCREEN: key = (int)KeyCode::PrintScreen; break;
		case GLFW_KEY_SCROLL_LOCK: key = (int)KeyCode::ScrollLock; break;
		case GLFW_KEY_PAUSE: key = (int)KeyCode::Pause; break;

		case GLFW_KEY_NUM_LOCK: key = (int)KeyCode::Numlock; break;
		case GLFW_KEY_KP_DIVIDE: key = (int)KeyCode::KeypadDivide; break;
		case GLFW_KEY_KP_MULTIPLY: key = (int)KeyCode::KeypadMultiply; break;
		case GLFW_KEY_KP_SUBTRACT: key = (int)KeyCode::KeypadMinus; break;
		case GLFW_KEY_KP_ADD: key = (int)KeyCode::KeypadPlus; break;
		case GLFW_KEY_KP_ENTER: key = (int)KeyCode::KeypadEnter; break; 
		case GLFW_KEY_KP_DECIMAL: key = (int)KeyCode::KeypadPeriod; break;
		
		case GLFW_KEY_UP: key = (int)KeyCode::UpArrow; break;
		case GLFW_KEY_DOWN: key = (int)KeyCode::DownArrow; break;
		case GLFW_KEY_LEFT: key = (int)KeyCode::LeftArrow; break;
		case GLFW_KEY_RIGHT: key = (int)KeyCode::RightArrow; break;

		case GLFW_KEY_INSERT: key = (int)KeyCode::Insert; break;
		case GLFW_KEY_DELETE: key = (int)KeyCode::Delete; break;
		case GLFW_KEY_HOME: key = (int)KeyCode::Home; break;
		case GLFW_KEY_END: key = (int)KeyCode::End; break;
		case GLFW_KEY_PAGE_UP: key = (int)KeyCode::PageUp; break;
		case GLFW_KEY_PAGE_DOWN: key = (int)KeyCode::PageDown; break;
		}
	}

	return key;
}
static void OnKeyCallback(GLFWwindow* hWnd, int key, int scancode, int action, int mods)
{
	RenderWindow* window = WindowManager::GetWindow(GetGLFWHwnd(hWnd));
	if (window)
	{
		window->OnWinMsg((uint)GLFWMsgType::Key, action, key, mods, scancode);
	}
}
static void OnCharCallback(GLFWwindow* hWnd, uint c)
{
	RenderWindow* window = WindowManager::GetWindow(GetGLFWHwnd(hWnd));
	if (window)
	{
		window->OnWinMsg((uint)GLFWMsgType::Character, (int64)c, 0, 0, 0);
	}
}
static void OnMouseBtnCallback(GLFWwindow* hWnd, int button, int action, int mods)
{
	RenderWindow* window = WindowManager::GetWindow(GetGLFWHwnd(hWnd));
	if (window)
	{
		window->OnWinMsg((uint)GLFWMsgType::Mouse, (int64)action, (int64)button, mods, 0);
	}
}
//鼠标位置，左上角(0,0)，右下角(w,h)
static void OnCursorPositionCallback(GLFWwindow* hWnd, double x, double y)
{
	RenderWindow* window = WindowManager::GetWindow(GetGLFWHwnd(hWnd));
	if (window)
	{
		window->OnWinMsg((uint)GLFWMsgType::MouseMove, (int64)x, (int64)y, 0, 0);
	}
}
//滑轮,y有效，向前是正，向后是负；整数(1,2,3)
static void OnScrollCallback(GLFWwindow* hWnd, double x, double y)
{
	RenderWindow* window = WindowManager::GetWindow(GetGLFWHwnd(hWnd));
	if (window)
	{
		window->OnWinMsg((uint)GLFWMsgType::Scroll, (int64)x, (int64)y, 0, 0);
	}
}
//改变大小
static void OnResizeCallback(GLFWwindow* hWnd, int w, int h)
{
	RenderWindow* window = WindowManager::GetWindow(GetGLFWHwnd(hWnd));
	if (window)
	{
		window->OnWinMsg((uint)GLFWMsgType::Resize, w, h, 0, 0);
	}
}
//获得焦点
static void OnWindowFocusCallback(GLFWwindow* hWnd, int focused)
{
	RenderWindow* window = WindowManager::GetWindow(GetGLFWHwnd(hWnd));
	if (window)
	{
		window->OnWinMsg((uint)GLFWMsgType::Activate, focused, 0, 0, 0);
	}
}
//关闭窗口
static void OnWindowCloseCallback(GLFWwindow* window)
{
	WindowManager::CloseWindow(GetGLFWHwnd(window));
}
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(GLFWRenderWindow, RenderWindow);
GLFWRenderWindow::GLFWRenderWindow()
{
}
GLFWRenderWindow::~GLFWRenderWindow()
{
	if (m_hWnd != nullptr)
	{
		glfwDestroyWindow(m_hWnd);
		m_hWnd = nullptr;
	}
}
bool GLFWRenderWindow::Create(WindowDesc& info)
{
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

#if defined(DC_GRAPHICS_API_OPENGL)
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
#elif defined(DC_GRAPHICS_API_OPENGLES3)
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#endif

	//颜色模式和刷新频率
	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_ALPHA_BITS, 8);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
	glfwWindowHint(GLFW_DEPTH_BITS, 24);
	glfwWindowHint(GLFW_STENCIL_BITS, 8);
	
	glfwWindowHint(GLFW_SAMPLES, info.antiAlias);//MSAA
	Debuger::Log("antiAlias:%d", info.antiAlias);

	//不可修改大小
	glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, info.flags & WindowFlag::Resizable ? GLFW_TRUE : GLFW_FALSE);

	//调整窗口
	int screen_width = mode->width;
	int screen_height = mode->height;
	Platform::AdjustWindowRect(info.x, info.y, info.width, info.height, screen_width, screen_height, info.flags & WindowFlag::FullScreen);

	m_isIgnoreSizeChange = true;
	//创建窗口，monitor非NULL的话，窗口会被全屏创建到指定的监视器上
	m_hWnd = glfwCreateWindow(info.width, info.height, info.name.c_str(), (info.flags & WindowFlag::FullScreen) ? monitor : NULL, NULL);
	if (!m_hWnd)
	{
		Debuger::Error("GLFWRenderWindow::Create - glfwCreateWindow");
		glfwTerminate();
		return false;
	}
	//位置
	glfwSetWindowPos(m_hWnd, info.x, info.y);
	//操作相应
	glfwSetFramebufferSizeCallback(m_hWnd, OnResizeCallback);
	glfwSetKeyCallback(m_hWnd, OnKeyCallback);
	glfwSetCharCallback(m_hWnd, OnCharCallback);
	glfwSetMouseButtonCallback(m_hWnd, OnMouseBtnCallback);
	glfwSetCursorPosCallback(m_hWnd, OnCursorPositionCallback);
	glfwSetScrollCallback(m_hWnd, OnScrollCallback);
	glfwSetWindowFocusCallback(m_hWnd, OnWindowFocusCallback);
	glfwSetWindowCloseCallback(m_hWnd, OnWindowCloseCallback);
#if defined(DC_GRAPHICS_API_OPENGL) || defined(DC_GRAPHICS_API_OPENGLES3)
	//设置window中的窗口所关联的OpenGL环境为当前环境
	glfwMakeContextCurrent(m_hWnd);
#endif
	//标题
	glfwSetWindowTitle(m_hWnd, info.name.c_str());
	//隐藏鼠标
	//glfwSetInputMode(m_hWnd, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	m_isIgnoreSizeChange = false;
	return base::Create(info);
}
void GLFWRenderWindow::Update()
{
	if (glfwWindowShouldClose(m_hWnd))
	{
		WindowManager::CloseWindow(m_hWnd);
	}
}
void GLFWRenderWindow::Resize(WindowResizeDesc& desc)
{
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	//使用之前设定
	if (!desc.fullscreen_valid)
	{
		desc.fullscreen = m_fullscreen;
	}

	//位置
	int left = 0, top = 0;
	if (!desc.fullscreen)
	{
		glfwGetWindowPos(m_hWnd, &left, &top);
	}

	//屏幕大小
	int screen_width = mode->width;
	int screen_height = mode->height;

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
	desc.width = width; desc.height = height;
	m_isIgnoreSizeChange = true;
	glfwSetWindowMonitor(m_hWnd, desc.fullscreen ? monitor : NULL, left, top, width, height, m_fps);
	m_isIgnoreSizeChange = false;
	base::Resize(desc);
}
int GLFWRenderWindow::OnWinMsg(uint uMsg, int64 param1, int64 param2, int64 param3, int64 param4)
{
	GLFWMsgType type = GLFWMsgType(uMsg);
	switch (type)
	{
	case GLFWMsgType::Activate:
	{
		this->OnFocus(int(param1) != 0);
		break;
	}
	case GLFWMsgType::Key:
	{
		int action = int(param1);  int key = int(param2);
		key = GetKeyCode(key);
		if (key >= 0)
		{
			if (action == GLFW_PRESS)
			{
				KeyboardInput::HandleKeyDown(key);
			}
			else if (action == GLFW_RELEASE)
			{
				KeyboardInput::HandleKeyUp(key);

				//特殊功能键转char
				ushort unicode_char = 0;
				switch ((KeyCode)key)
				{
				case KeyCode::Backspace:unicode_char = key;
				case KeyCode::Return:unicode_char = key;
				case KeyCode::Tab:unicode_char = key;
				}
				if (unicode_char > 0)
				{
					KeyboardInput::AddInputChar((ushort)unicode_char);
				}
			}
		}
		else
		{
			Debuger::Log("unhandle key %d", key);
		}
		break;
	}
	case GLFWMsgType::Character:
	{
		KeyboardInput::AddInputChar((ushort)param1);
		break;
	}
	case GLFWMsgType::Mouse:
	{
		int action = int(param1); int button = int(param2);
		if (action == GLFW_PRESS)
		{
			switch (button)
			{
			case GLFW_MOUSE_BUTTON_LEFT:
				MouseInput::HandleLBtnDown(Input::mousePosition.x, Input::mousePosition.y);
				break;
			case GLFW_MOUSE_BUTTON_MIDDLE:
				MouseInput::HandleMBtnDown(Input::mousePosition.x, Input::mousePosition.y);
				break;
			case GLFW_MOUSE_BUTTON_RIGHT:
				MouseInput::HandleRBtnDown(Input::mousePosition.x, Input::mousePosition.y);
				break;
			}
		}
		else if (action == GLFW_RELEASE)
		{
			switch (button)
			{
			case GLFW_MOUSE_BUTTON_LEFT:
				MouseInput::HandleLBtnUp(Input::mousePosition.x, Input::mousePosition.y);
				break;
			case GLFW_MOUSE_BUTTON_MIDDLE:
				MouseInput::HandleMBtnUp(Input::mousePosition.x, Input::mousePosition.y);
				break;
			case GLFW_MOUSE_BUTTON_RIGHT:
				MouseInput::HandleRBtnUp(Input::mousePosition.x, Input::mousePosition.y);
				break;
			}
		}
		break;
	}
	case GLFWMsgType::MouseMove:
	{
		float mouse_x = (float)param1;
		float mouse_y = (float)this->GetHeight() - (float)param2;
		if (!Math::FloatEqual(Input::mousePosition.x, mouse_x) || !Math::FloatEqual(Input::mousePosition.y, mouse_y))
		{
			Vector3 old_pos = Input::mousePosition;
			MouseInput::HandleInputPosition(mouse_x, mouse_y);
			if (m_isFirstMove)
			{
				m_isFirstMove = false;
				MouseInput::HandleInputMove(0, 0);
			}
			else
			{
				float offset_x = Input::mousePosition.x - old_pos.x;
				float offset_y = Input::mousePosition.y - old_pos.y;
				MouseInput::HandleInputMove(offset_x, offset_y);
			}
		}
		break;
	}
	case GLFWMsgType::Scroll:
	{
		MouseInput::HandleInputWheel((float)param1, (float)param2);
		break;
	}
	case GLFWMsgType::Resize:
	{
		if (!m_isIgnoreSizeChange)
		{
			WindowResizeDesc desc(GetHwnd(), (int)param1, (int)param2, false, false);
			Application::Resize(desc);
		}
		break;
	}
	}
	return 0;
}
void GLFWRenderWindow::Draw()
{
	base::Draw();
#if defined(DC_GRAPHICS_API_OPENGL) || defined(DC_GRAPHICS_API_OPENGLES3)
	if (m_hWnd != nullptr && !glfwWindowShouldClose(m_hWnd))
	{
		//请求窗口系统将参数window关联的后缓存画面呈现给用户
		glfwSwapBuffers(m_hWnd);
	}
#endif
}
void* GLFWRenderWindow::GetHwnd()
{
	return GetGLFWHwnd(m_hWnd);
}
void GLFWRenderWindow::Initialize()
{
	//错误回调
	glfwSetErrorCallback([](int error, const char* description)
		{
			Debuger::Error("glfw internal error(%d)%s", error, description);
		});

	//初始化,必须在其他任何GLFW函数之前被调用，因为它负责初始化整个GLFW库
	if (!glfwInit())
	{
		Debuger::Error("glfwInit error");
	}
}
void GLFWRenderWindow::Destroy()
{
	glfwTerminate();
}
void GLFWRenderWindow::PeekMessage()
{
	glfwPollEvents();
}
DC_END_NAMESPACE