#include "WindowManager.h"
#include "RenderWindow.h"
#include "runtime/profiler/Profiler.h"
#include "runtime/Screen.h"
#include "runtime/Application.h"

#if defined(DC_PLATFORM_WIN32)
	#if defined(DC_GRAPHICS_API_DX9) || defined(DC_GRAPHICS_API_DX11)
		#include "windows/window/Win32DXRenderWindow.h"
	#elif defined(DC_GRAPHICS_API_OPENGL)
		#include "windows/window/Win32GLRenderWindow.h"
	#elif defined(DC_GRAPHICS_API_OPENGLES3)
		#include "windows/window/Win32GLESRenderWindow.h"
	#endif
#elif defined(DC_PLATFORM_LINUX)
	#include "linux/window/LinuxGLRenderWindow.h"
#elif defined(DC_PLATFORM_ANDROID)
	#include "android/window/AndroidGLESRenderWindow.h"
#endif
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_REFECTION_TYPE(WindowManager);
WindowManager::RenderWindows WindowManager::_renderWindows;
WindowManager::MapRenderWindows WindowManager::_mapRenderWindows;
void WindowManager::Initialize()
{
#if defined(DC_PLATFORM_WIN32)
	#if defined(DC_GRAPHICS_API_DX9) || defined(DC_GRAPHICS_API_DX11)
		Win32DXRenderWindow::Initialize();
	#else
		GLFWRenderWindow::Initialize();
	#endif
#elif defined(DC_PLATFORM_LINUX)
	GLFWRenderWindow::Initialize();
#elif defined(DC_PLATFORM_ANDROID)
	//undo
#endif

	_renderWindows.Resize((int)TargetDisplay::Max, nullptr);
}
void WindowManager::Destroy()
{
	_mapRenderWindows.Clear();

	for (auto window : _renderWindows)
	{
		SAFE_DELETE(window);
	}
	_renderWindows.Clear();

#if defined(DC_PLATFORM_WIN32)
	#if defined(DC_GRAPHICS_API_DX9) || defined(DC_GRAPHICS_API_DX11)
		Win32DXRenderWindow::Destroy();
	#else
		GLFWRenderWindow::Destroy();
	#endif
#elif defined(DC_PLATFORM_LINUX)
	GLFWRenderWindow::Destroy();
#elif defined(DC_PLATFORM_ANDROID)
	//undo
#endif
}
RenderWindow* WindowManager::Create(WindowDesc& info)
{
	RenderWindow* window = nullptr;
#if defined(DC_PLATFORM_WIN32)
	#if defined(DC_GRAPHICS_API_DX9) || defined(DC_GRAPHICS_API_DX11)
		window = Win32DXRenderWindow::Create();
	#elif defined(DC_GRAPHICS_API_OPENGL)
		window = Win32GLRenderWindow::Create();
	#elif defined(DC_GRAPHICS_API_OPENGLES3)
		window = Win32GLESRenderWindow::Create();
	#else
		Debuger::Error("Win32Platform::CreateRenderWindow - unknow:%d", type);
	#endif
#elif defined(DC_PLATFORM_LINUX)
	window = LinuxGLRenderWindow::Create();
#elif defined(DC_PLATFORM_ANDROID)
	window = AndroidGLESRenderWindow::Create();
#endif

	if (window == nullptr)
	{
		Debuger::Error("WindowManager::Create - error");
		return nullptr;
	}
	if (!window->Create(info))
	{
		Debuger::Error("WindowManager::Create - Create error");
		return nullptr;
	}
	AddWindow(window->GetTargetDisplay(), window);

	if (window->GetTargetDisplay() == TargetDisplay::Display1)
	{
		Screen::m_width = window->GetWidth();
		Screen::m_height = window->GetHeight();
		Screen::m_fullscreen = window->IsFullscreen();
	}
	return window;
}
void WindowManager::AddWindow(TargetDisplay display, RenderWindow* window)
{
	AssertEx(_renderWindows[int(display)] == nullptr,"");

	_renderWindows[int(display)] = window;
	AssertEx(_mapRenderWindows.Add( (uint64)window->GetHwnd(), window),"");
}
void WindowManager::CloseWindow(void* hWnd)
{
	RenderWindow* window = GetWindow(hWnd);
	if (!window)return;

	uint64 id = (uint64)hWnd;
	_renderWindows[int(window->GetTargetDisplay())] = nullptr;
	_mapRenderWindows.Remove(id);
	SAFE_DELETE(window);

	if (_mapRenderWindows.Size() == 0)
	{
		Application::Quit();
	}
}
RenderWindow* WindowManager::GetWindow(void* hWnd)
{
	uint64 id = (uint64)hWnd;
	auto it = _mapRenderWindows.Find(id);
	if (it != _mapRenderWindows.end())
	{
		return it->second;
	}
	return nullptr;
}
void WindowManager::Update()
{
	DC_PROFILE_FUNCTION();
#if defined(DC_PLATFORM_WIN32)
	#if defined(DC_GRAPHICS_API_DX9) || defined(DC_GRAPHICS_API_DX11)
		Win32DXRenderWindow::PeekMessage();
	#else
		GLFWRenderWindow::PeekMessage();
	#endif
#elif defined(DC_PLATFORM_LINUX)
	GLFWRenderWindow::PeekMessage();
#elif defined(DC_PLATFORM_ANDROID)
	//undo
#endif

	for (const auto& window : _renderWindows)
	{
		if (window)window->Update();
	}
}
void WindowManager::Draws()
{
	DC_PROFILE_FUNCTION();
	for (const auto& window : _renderWindows)
	{
		if (window)window->Draw();
	}
}
void WindowManager::Resize(WindowResizeDesc& desc)
{
	for (const auto& window : _renderWindows)
	{
		if (window && (window->GetHwnd() == desc.hWnd || desc.hWnd == nullptr))
		{
			window->Resize(desc);
			if (window->GetTargetDisplay() == TargetDisplay::Display1)
			{
				Screen::m_width = _renderWindows[0]->GetWidth();
				Screen::m_height = _renderWindows[0]->GetHeight();
				Screen::m_fullscreen = _renderWindows[0]->IsFullscreen();
			}
		}
	}
}
DC_END_NAMESPACE