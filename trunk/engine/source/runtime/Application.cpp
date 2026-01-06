#include "Application.h"
#include "Screen.h"
#include "core/file/Path.h"
#include "core/image/ImageManager.h"
#include "core/time/ClockTime.h"
#include "core/time/Time.h"
#include "core/time/Timer.h"
#include "runtime/audio/AudioManager.h"
#include "runtime/font/FontAtlas.h"
#include "runtime/font/FontManager.h"
#include "runtime/input/Input.h"
#include "runtime/lua/LuaManager.h"
#include "runtime/mesh/MeshManager.h"
#include "runtime/particle/Particle.h"
#include "runtime/physics/Physics.h"
#include "runtime/physics2d/Physics2d.h"
#include "runtime/project/ProjectManager.h"
#include "runtime/profiler/Profiler.h"
#include "runtime/renderable/Gizmos.h"
#include "runtime/graphics/Graphics.h"
#include "runtime/graphics/Material.h"
#include "runtime/graphics/Shader.h"
#include "runtime/resources/AssetsManager.h"
#include "runtime/scene/LayerMask.h"
#include "runtime/scene/TagManager.h"
#include "runtime/thread/Thread.h"
#include "runtime/thread/ThreadScheduler.h"
#include "runtime/tween/TweenManager.h"
#include "runtime/ui/GUI.h"
#include "runtime/ui/UIAtlas.h"
#include "runtime/video/VideoDecoder.h"
#include "platform/PlatformBase.h"
#include "platform/WindowManager.h"
#include "editor/common/EditorIcon.h"
#include "editor/EditorAppliction.h"
#include "platform/BaseMain.h"

#if defined(DC_GRAPHICS_API_DX9)
	#include "runtime/graphics/dx/dx9/DX9Device.h"
#elif defined(DC_GRAPHICS_API_DX11)
	#include "runtime/graphics/dx/dx11/DX11Device.h"
#elif defined(DC_GRAPHICS_API_DX12)
	#include "runtime/graphics/dx/dx12/DX12Device.h"
#elif defined(DC_GRAPHICS_API_OPENGL) || defined(DC_GRAPHICS_API_OPENGLES3)
	#include "runtime/graphics/opengl/GLDevice.h"
#elif defined(DC_GRAPHICS_API_VULKAN)
	#include "runtime/graphics/vulkan/VKDevice.h"
#endif

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(Application, Object);
void Application::Initialize(bool editor)
{
	DC_PROFILE_FUNCTION;
	Debuger::Log("Application::Initialize begin");

	_isEditor = editor;

	//使用UTF8：可以让 C 标准库（如 printf、fopen）处理 UTF-8 字符串，但它不会影响 Win32（Windows API）层面的字符串编码。
	setlocale(LC_ALL, ".UTF-8");
#if defined(DC_PLATFORM_WIN32)
	::SetConsoleOutputCP(CP_UTF8);//解决控制台中文乱码
#endif

	//最先初始化
	Debuger::Initialize();

	//目录设置
#if defined(DC_PLATFORM_WIN32) || defined(DC_PLATFORM_LINUX) || defined(DC_PLATFORM_MAC)
	Path::SetCurrentProcessDirectory(".");
	SetPersistentDataPath(".");
#elif defined(DC_PLATFORM_WASM)
	SetPersistentDataPath(".");//通过打包命令生成的虚拟目录-DCMAKE_EXE_LINKER_FLAGS="--preload-file ../template/data/assets@/"
#endif
	Debuger::Log("appliction current path:%s", Path::GetCurrentProcessDirectory().c_str());

	Math::RandomInit(); 
	Time::Initialize(); 
	Input::Initialize(); 
	Platform::Initialize(); 
	AssetsManager::Initialize(); 
	WindowManager::Initialize(); 
#if !defined(DC_PLATFORM_WASM)
	ThreadPools::Initialize(10); 
#endif
	SceneManager::Initialize(); 
	VideoDecoder::Initialize(); 
	AudioManager::Initialize(); 
	FontManager::Initialize(); 
	GUIContext::Initialize(); 
	LuaManager::Initialize(); 
	Gizmos::Initialize(); 
	ProjectManager::Initialize(); 

#if defined(DC_GRAPHICS_API_DX9)
	_graphicsDevice = DX9Device::Create();
#elif defined(DC_GRAPHICS_API_DX11)
	_graphicsDevice = DX11Device::Create();
#elif defined(DC_GRAPHICS_API_DX12)
	_graphicsDevice = DX12Device::Create();
#elif defined(DC_GRAPHICS_API_OPENGL) || defined(DC_GRAPHICS_API_OPENGLES3)
	_graphicsDevice = GLDevice::Create();
#elif defined(DC_GRAPHICS_API_VULKAN)
	_graphicsDevice = VKDevice::Create();
#else
	#error "Unknown RendererAPI"
#endif

	if (Application::IsEditor())
	{
		SetPlay(false);
		EditorAppliction::Initialize();
	}

	Type::Output();
	Debuger::Log("Application::Initialize end");
}
void Application::Shutdown()
{
	DC_PROFILE_FUNCTION;
	SceneManager::Destroy();

	if (Application::IsEditor())
	{
		EditorAppliction::Destroy();
	}

	ProjectManager::Destroy();//需要在SceneManager之后执行
	Gizmos::Destroy();
	LuaManager::Destroy(); 
	TweenManager::Destroy();
	Graphics::Destroy();
	GUIContext::Destroy();
	VideoDecoder::Destroy();
	AudioManager::Destroy();
	MeshManager::Destroy();
	MaterialManager::Destroy();
	UIAtlasManager::Destroy();
	ParticlePools::Destroy();
	FontManager::Destroy();
	FontAtlasManager::Destroy();
	EditorIconCache::Destroy();
	ImageManager::Destroy();
	ShaderSerializePools::Destroy();
	ThreadScheduler::Destroy();
	SAFE_RELEASE(_graphicsDevice);
	WindowManager::Destroy();
	AssetsManager::Destroy();

	//放后面
	Platform::Destroy();
	AutoreleasePool::Destroy();
	ThreadPools::Destroy();
	Debuger::Destroy();
	Type::Destroy();
}
void Application::Start()
{
	Debuger::Log("Application::Start");
#if defined(DC_PLATFORM_ANDROID)
	//undo，由android驱动
#elif defined(DC_PLATFORM_WASM)
	wasm_set_main_loop([](){Application::MainLoop(0); }, 0);
#else
	while (!_isClosing)
	{
		int fps = 60;
		RenderWindow* window = WindowManager::GetWindow(TargetDisplay::Display1);
		if (window)fps = window->GetFPS();
		MainLoop(fps);
	}
	Application::Shutdown();
#endif
}
void Application::Quit()
{
	DC_PROFILE_FUNCTION;
	Debuger::Log("Application::Quit");
	_isClosing = true;
#if defined(DC_PLATFORM_ANDROID)
	android_quit_application();
#elif defined(DC_PLATFORM_WASM)
	wasm_quit_application();
#else

#endif
}
void Application::Pause(bool pause)
{
	Debuger::Log("pause:%s", pause ? "true" : "false");
}
void Application::MainLoop(int fps)
{
	//Debuger::Log("Application::MainLoop");
	static ClockTime timer;
	static int64 dwTickCount = 0;
	static int64 dwLastTickCount = timer.micro();
	dwTickCount = timer.micro();
	fps = fps <= 0 ? 1000 : fps;
	int64 SKIP_TICKS = (int64)(1000000.0f / fps);//转换成微妙计算

	WindowManager::Update();//这个一直要放外部执行

	if (dwTickCount - dwLastTickCount <= SKIP_TICKS)
	{
		Thread::Sleep(0);//这里用0会比较准确，否则会有1毫秒的误差
	}
	else
	{
		DC_PROFILE_FUNCTION;
		{
			PreUpdate();
			Update();

			WindowManager::Draws();

			LateUpdate();
			dwLastTickCount = dwTickCount;
		}
		_frameTime = timer.micro() - dwTickCount;
		DC_PROFILE_FRAME;
	}
}
void Application::PreUpdate()
{
	DC_PROFILE_FUNCTION;
	//put the top
	Time::Update();
	Timer::Update();
	TweenManager::Update();
}
void Application::Update()
{
	DC_PROFILE_FUNCTION;
	ThreadScheduler::Update();
	Physics::Update();
	Physics2d::Update();
	EditorAppliction::Update();
	SceneManager::Update();
}
void Application::LateUpdate()
{
	DC_PROFILE_FUNCTION;
	//清除输入
	Input::Reset();
	//put the last
	AutoreleasePool::Update();
}
bool Application::CreateRenderWindow(WindowDesc& info)
{
	DC_PROFILE_FUNCTION;
	CHECK_RETURN_PTR_FALSE(int(info.display) >= 0 && info.display < TargetDisplay::Max);
	Debuger::Log("CreateRenderWindow name:%s, width:%d, height:%d, fullscreen:%s", info.name.c_str(), info.width, info.height, info.flags & WindowFlag::FullScreen ? "true" : "false");
	
	//create window
	RenderWindow* window = WindowManager::Create(info);
	if (window == nullptr)return false;

	//渲染设备
	if (window->GetTargetDisplay() == TargetDisplay::Display1)
	{
		_graphicsDevice->CreateDevice(window);
		_graphicsDevice->CreateRenderContent();
	}
	//交换链
	_graphicsDevice->CreateSwapChain(window);

	return true;
}
bool Application::DestroyRenderWindow(WindowDesc& info)
{
	DC_PROFILE_FUNCTION;
	CHECK_RETURN_PTR_FALSE(int(info.display) >= 0 && info.display < TargetDisplay::Max);
	Debuger::Log("DestroyRenderWindow name:%s", info.name.c_str());

	RenderWindow* window = WindowManager::GetWindow(info.display);
	if (window == nullptr)return false;

	//渲染设备
	if (window->GetTargetDisplay() == TargetDisplay::Display1)
	{
		_graphicsDevice->DestroyDevice(window);
		_graphicsDevice->DestroyRenderContent();
	}
	//交换链
	_graphicsDevice->DestroySwapChain(window);

	//关闭窗口
	WindowManager::CloseWindow(window);
	return true;
}
void Application::Resize(WindowResizeDesc& desc)
{
	DC_PROFILE_FUNCTION;
	//优先调整窗口
	WindowManager::Resize(desc);

	_graphicsDevice->Resize(desc);
	SceneManager::Resize(desc);

	if (Application::IsEditor())
	{
		EditorAppliction::Resize(desc);
	}
}
void Application::SetAssetsPath(const String& path) 
{ 
	_readRootPath = path;
	Debuger::Log("SetAssetsPath:%s", _readRootPath.c_str());
}
void Application::SetPersistentDataPath(const String& path) 
{ 
	_saveRootPath = path;
	Debuger::Log("SetPersistentDataPath:%s", _saveRootPath.c_str());
}
DC_END_NAMESPACE