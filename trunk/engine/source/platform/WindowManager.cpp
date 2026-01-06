#include "WindowManager.h"
#include "RenderWindow.h"
#include "runtime/profiler/Profiler.h"
#include "runtime/Screen.h"
#include "runtime/Application.h"

#if defined(DC_PLATFORM_WIN32)
	#if defined(DC_GRAPHICS_API_DX9) || defined(DC_GRAPHICS_API_DX11) || defined(DC_GRAPHICS_API_DX12)
		#include "windows/window/Win32DXRenderWindow.h"
	#elif defined(DC_GRAPHICS_API_OPENGL)
		#include "windows/window/Win32GLRenderWindow.h"
	#elif defined(DC_GRAPHICS_API_OPENGLES3)
		#include "windows/window/Win32GLESRenderWindow.h"
	#elif defined(DC_GRAPHICS_API_VULKAN)
		#include "windows/window/Win32VKRenderWindow.h"
	#endif
#elif defined(DC_PLATFORM_LINUX)
	#include "linux/window/LinuxGLRenderWindow.h"
#elif defined(DC_PLATFORM_ANDROID)
	#include "android/window/AndroidGLESRenderWindow.h"
#elif defined(DC_PLATFORM_WASM)
	#include "wasm/window/WASMGLRenderWindow.h"
#elif defined(DC_PLATFORM_MAC)
	#include "mac/window/MacGLRenderWindow.h"
#elif defined(DC_PLATFORM_IOS)
	#include "ios/window/iOSGLRenderWindow.h"
#endif
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_REFECTION_TYPE(WindowManager);
void WindowManager::Initialize()
{
#if defined(DC_PLATFORM_WIN32)
	#if defined(DC_GRAPHICS_API_DX9) || defined(DC_GRAPHICS_API_DX11) || defined(DC_GRAPHICS_API_DX12)
		Win32DXRenderWindow::Initialize();
	#else
		GLFWRenderWindow::Initialize();
	#endif
#elif defined(DC_PLATFORM_LINUX) || defined(DC_PLATFORM_WASM) || defined(DC_PLATFORM_MAC) || defined(DC_PLATFORM_IOS)
	GLFWRenderWindow::Initialize();
#elif defined(DC_PLATFORM_ANDROID)
	//undo
#else
	#error "Unknown PlatformType"
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
	#if defined(DC_GRAPHICS_API_DX9) || defined(DC_GRAPHICS_API_DX11) || defined(DC_GRAPHICS_API_DX12)
		Win32DXRenderWindow::Destroy();
	#else
		GLFWRenderWindow::Destroy();
	#endif
#elif defined(DC_PLATFORM_LINUX) || defined(DC_PLATFORM_WASM) || defined(DC_PLATFORM_MAC) || defined(DC_PLATFORM_IOS)
	GLFWRenderWindow::Destroy();
#elif defined(DC_PLATFORM_ANDROID)
	//undo
#else
#error "Unknown PlatformType"
#endif
}
RenderWindow* WindowManager::Create(WindowDesc& info)
{
	RenderWindow* window = nullptr;
#if defined(DC_PLATFORM_WIN32)
	#if defined(DC_GRAPHICS_API_DX9) || defined(DC_GRAPHICS_API_DX11) || defined(DC_GRAPHICS_API_DX12)
		window = Win32DXRenderWindow::Create();
	#elif defined(DC_GRAPHICS_API_OPENGL)
		window = Win32GLRenderWindow::Create();
	#elif defined(DC_GRAPHICS_API_OPENGLES3)
		window = Win32GLESRenderWindow::Create();
	#elif defined(DC_GRAPHICS_API_VULKAN)
		window = Win32VKRenderWindow::Create();
	#else
		Debuger::Error("Win32Platform::CreateRenderWindow - unknow:%d", type);
	#endif
#elif defined(DC_PLATFORM_LINUX)
	window = LinuxGLRenderWindow::Create();
#elif defined(DC_PLATFORM_ANDROID)
	window = AndroidGLESRenderWindow::Create();
#elif defined(DC_PLATFORM_WASM)
	window = WASMGLRenderWindow::Create();
#elif defined(DC_PLATFORM_MAC)
	window = MacGLRenderWindow::Create();
#elif defined(DC_PLATFORM_IOS)
	window = iOSGLRenderWindow::Create();
#else
#error "Unknown PlatformType"
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
		Screen::_width = window->GetWidth();
		Screen::_height = window->GetHeight();
		Screen::_fullscreen = window->IsFullscreen();
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
	CloseWindow(window);
}
void WindowManager::CloseWindow(RenderWindow* window)
{
	if (!window) 
	{
		Debuger::Error("WindowManager::CloseWindow window is null");
		return;
	}
	uint64 id = (uint64)window->GetHwnd();
	_renderWindows[int(window->GetTargetDisplay())] = nullptr;
	_mapRenderWindows.Remove(id);
	SAFE_DELETE(window);

	//if (_mapRenderWindows.Size() == 0)
	//{
	//	Application::Quit();
	//}
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
#if defined(DC_PLATFORM_WIN32)
	#if defined(DC_GRAPHICS_API_DX9) || defined(DC_GRAPHICS_API_DX11) || defined(DC_GRAPHICS_API_DX12)
		Win32DXRenderWindow::PeekMessage();
	#else
		GLFWRenderWindow::PeekMessage();
#endif
#elif defined(DC_PLATFORM_LINUX) || defined(DC_PLATFORM_WASM) || defined(DC_PLATFORM_MAC) || defined(DC_PLATFORM_IOS)
	GLFWRenderWindow::PeekMessage();
#elif defined(DC_PLATFORM_ANDROID)
	//undo
#else
#error "Unknown PlatformType"
#endif

	for (const auto& window : _renderWindows)
	{
		if (window)window->Update();
	}
}
void WindowManager::Draws()
{
	DC_PROFILE_FUNCTION;
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
				Screen::_width = _renderWindows[0]->GetWidth();
				Screen::_height = _renderWindows[0]->GetHeight();
				Screen::_fullscreen = _renderWindows[0]->IsFullscreen();
			}
		}
	}
}
DC_END_NAMESPACE