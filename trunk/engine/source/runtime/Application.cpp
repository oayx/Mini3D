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
#include "BaseMain.h"

#if defined(DC_GRAPHICS_API_DX9)
	#include "runtime/graphics/dx/dx9/DX9Device.h"
#elif defined(DC_GRAPHICS_API_DX11)
	#include "runtime/graphics/dx/dx11/DX11Device.h"
#elif defined(DC_GRAPHICS_API_OPENGL) || defined(DC_GRAPHICS_API_OPENGLES3)
	#include "runtime/graphics/opengl/GLDevice.h"
#endif

DC_BEGIN_NAMESPACE
/********************************************************************/
GraphicsDevice* Application::_graphicsDevice = nullptr;
String Application::_readRootPath = "";
String Application::_saveRootPath = "";
bool Application::_isClosing = false;
bool Application::_isEditor = false;
bool Application::_isPlay = true;
bool Application::_isPause = false;
int64 Application::_frameTime = 0;
IMPL_DERIVED_REFECTION_TYPE(Application, Object);
void Application::Initialize(bool editor)
{
	_isEditor = editor;

	//最先初始化
	Debuger::Initialize();

	//目录设置
#if defined(DC_PLATFORM_WIN32) || defined(DC_PLATFORM_LINUX)
	Path::SetCurrentProcessDirectory(".");
	SetPersistentDataPath(".");
#endif
	Debuger::Log("appliction current path:%s", Path::GetCurrentProcessDirectory().c_str());

	Math::RandomInit();
	Time::Initialize();
	Input::Initialize();
	Platform::Initialize();
	AssetsManager::Initialize();
	WindowManager::Initialize();
	ThreadPools::Initialize(10);
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
#elif defined(DC_GRAPHICS_API_OPENGL) || defined(DC_GRAPHICS_API_OPENGLES3)
	_graphicsDevice = GLDevice::Create();
#else
	#error "Unknown RendererAPI"
#endif

	if (Application::IsEditor())
	{
		SetPlay(false);
		EditorAppliction::Initialize();
	}

	Type::Output();
}
void Application::Shutdown()
{
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
#if defined(DC_PLATFORM_ANDROID)
	//undo
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
	_isClosing = true;
#if defined(DC_PLATFORM_ANDROID)
	java_quit_application();
#else

#endif
}
void Application::Pause(bool pause)
{
	Debuger::Log("pause:%s", pause ? "true" : "false");
}
void Application::MainLoop(int fps)
{
	DC_PROFILE_FUNCTION();
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
		{
			PreUpdate();
			Update();

			WindowManager::Draws();

			LateUpdate();
			dwLastTickCount = dwTickCount;
		}
		_frameTime = timer.micro() - dwTickCount;
	}
}
void Application::PreUpdate()
{
	DC_PROFILE_FUNCTION();
	//put the top
	Time::Update();
	Timer::Update();
	TweenManager::Update();
}
void Application::Update()
{
	DC_PROFILE_FUNCTION();
	ThreadScheduler::Update();
	Physics::Update();
	Physics2d::Update();
	EditorAppliction::Update();
	SceneManager::Update();
}
void Application::LateUpdate()
{
	DC_PROFILE_FUNCTION();
	//清除输入
	Input::Reset();
	//put the last
	AutoreleasePool::Clear();
}
bool Application::CreateRenderWindow(WindowDesc& info)
{
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
void Application::Resize(WindowResizeDesc& desc)
{
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
