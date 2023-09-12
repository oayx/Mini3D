#include "EGL_Context.h"
#include "runtime/Application.h"
#if defined(DC_GRAPHICS_API_OPENGLES3)
 
DC_BEGIN_NAMESPACE
/********************************************************************/
EGLDisplay EGL_Context::m_display = EGL_NO_DISPLAY;
EGLContext EGL_Context::m_context = EGL_NO_CONTEXT;
EGLSurface EGL_Context::m_surface = EGL_NO_SURFACE;
bool EGL_Context::Initialize(EGLNativeWindowType hWnd, int gl_version, int antiAlias)
{
	//1:得到默认的显示设备（就是窗口）
	m_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if (m_display == EGL_NO_DISPLAY)
	{
		Debuger::Log("eglGetDisplay error:%d", eglGetError());
		return false;
	}

	//2:初始化默认显示设备
	EGLint major;//返回主版本号  
	EGLint minor;//返回次版本号  
	if (!eglInitialize(m_display, &major, &minor))
	{
		Debuger::Log("eglInitialize error:%d", eglGetError());
		return false;
	}
	Debuger::Log("egl version:%d.%d", major, minor);

	return CreateContext(hWnd, gl_version, antiAlias);
}
void EGL_Context::Destroy()
{
	Debuger::Log("EGL_Context::Shutdown");

	if (m_display != EGL_NO_DISPLAY)
	{//当前的Display 不等于null  
		//清除绑定的 Surface Context
		eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		if (m_context != EGL_NO_CONTEXT)
		{//不等于空Context,销毁上下文  
			eglDestroyContext(m_display, m_context);
		}

		if (m_surface != EGL_NO_SURFACE)
		{//不等于空Surface,销毁Surface  
			eglDestroySurface(m_display, m_surface);
		}
		//终止Dispay  
		eglTerminate(m_display);
	}
	eglReleaseThread();
	//把 Display Context Surface 设置成初始化  
	m_display = EGL_NO_DISPLAY;
	m_context = EGL_NO_CONTEXT;
	m_surface = EGL_NO_SURFACE;
}
void EGL_Context::Draw()
{
	if (m_display != EGL_NO_DISPLAY && m_surface != EGL_NO_SURFACE)
	{
		if (!eglSwapBuffers(m_display, m_surface))
		{
			Debuger::Log("eglSwapBuffers error:%d", eglGetError());
		}
	}
}
bool EGL_Context::CreateContext(EGLNativeWindowType hWnd, int gl_version, int antiAlias)
{
	if (hWnd == nullptr)
	{
		Debuger::Error("EGL_Context::CreateContext - hWnd is null");
		return false;
	}

	//1. 创建 Surface
	EGLConfig config;
	while (antiAlias >= 0)
	{
		//设置显示设备的属性
		const EGLint attribs[] = {
			EGL_SURFACE_TYPE,	EGL_WINDOW_BIT,//把数据画到窗口上  
			EGL_BLUE_SIZE,		8,	//R 占8个比特位  
			EGL_GREEN_SIZE,		8,	//G 占8个比特位  
			EGL_RED_SIZE,		8,	//B 占8个比特位  
			EGL_ALPHA_SIZE,		8,	//B 占8个比特位  
			EGL_DEPTH_SIZE,		24,	//深度值 占24个比特位  画图的深度值 （前后顺序的层）  
			EGL_STENCIL_SIZE,	8,
			EGL_SAMPLES,		antiAlias,	//MSAA
			EGL_RENDERABLE_TYPE,EGL_OPENGL_ES2_BIT,
			EGL_NONE			//这个结束了  
		};
		//让EGL为你选择一个配置  
		EGLint numConfigs(0);
		if (eglChooseConfig(m_display, attribs, &config, 1, &numConfigs))
		{
			//创建 Surface(实际上就是一个FrameBuffer)
			m_surface = eglCreateWindowSurface(m_display, config, hWnd, NULL);
			if (m_surface != EGL_NO_CONTEXT)
			{
				break;
			}
			else
			{
				Debuger::Log("eglCreateWindowSurface error:%d", eglGetError());
				switch (eglGetError())
				{
				case EGL_BAD_MATCH:break;
				case EGL_BAD_CONFIG:break;
				case EGL_BAD_NATIVE_WINDOW:break;
				case EGL_BAD_ALLOC:break;
				}
			}
		}

		antiAlias--;
	}
	if (m_surface == EGL_NO_CONTEXT)
	{
		Debuger::Error("eglCreateWindowSurface");
		return false;
	}

	//2. 创建上下文对象  
#if defined(DC_GRAPHICS_API_OPENGLES3)
	int gles_version[] = { 3,2,3,1,3,0,2,0 };
#else
	int gles_version[] = { 2,0 };
#endif
	for (int i = 0; i < ARRAY_SIZE(gles_version); i += 2)
	{
		EGLint attr[] = { EGL_CONTEXT_CLIENT_VERSION, gles_version[i], EGL_CONTEXT_MINOR_VERSION, gles_version[i + 1],  EGL_NONE };
		m_context = eglCreateContext(m_display, config, EGL_NO_CONTEXT, attr);
		if (m_context == EGL_NO_SURFACE)
		{
			Debuger::Warning("eglCreateContext glesversion:%d.%d.error:%d", gles_version[i], gles_version[i + 1],eglGetError());
		}
		else
		{
			Debuger::Log("gles_version:%d.%d", gles_version[i], gles_version[i + 1]);
			break;
		}
	}
	if (m_context == EGL_NO_SURFACE)
	{
		Debuger::Error("eglCreateContext error");
	}

	//3. 绑定eglContext和surface到display
	if (!eglMakeCurrent(m_display, m_surface, m_surface, m_context))
	{
		Debuger::Log("eglMakeCurrent error:%d", eglGetError());
		return false;
	}
	return true;
}
void EGL_Context::DestroyContext()
{
	//清除绑定的 Surface Context   
	eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	if (m_context != EGL_NO_CONTEXT)
	{//不等于空Context,销毁上下文  
		eglDestroyContext(m_display, m_context);
	}

	if (m_surface != EGL_NO_SURFACE)
	{//不等于空Surface,销毁Surface  
		eglDestroySurface(m_display, m_surface);
	}
	m_context = EGL_NO_CONTEXT;
	m_surface = EGL_NO_SURFACE;
}
DC_END_NAMESPACE
#endif

/*
#define EGL_BUFFER_SIZE				0x3020 // 颜色缓冲区深度（包括RGBA）
#define EGL_ALPHA_SIZE				0x3021 // 颜色缓冲区中alpha位数
#define EGL_BLUE_SIZE				0x3022 // 颜色缓冲区中blue位数
#define EGL_GREEN_SIZE				0x3023 // 颜色缓冲区green位数
#define EGL_RED_SIZE				0x3024 // 颜色缓冲区red位数
#define EGL_DEPTH_SIZE				0x3025 // 深度缓冲区位数
#define EGL_STENCIL_SIZE			0x3026 // stencil缓冲区位数
#define EGL_CONFIG_CAVEAT			0x3027 // 配置说明（EGL_NONE、EGL_SLOW_CONFIG、EGL_NON_CONFORMANT_CONFIG）
#define EGL_CONFIG_ID				0x3028 // 配置ID
#define EGL_LEVEL					0x3029 // frame buffer级别（默认为0或表示overlay的整数和underlay的负数）
#define EGL_MAX_PBUFFER_HEIGHT		0x302A // Surface为pixel buffer时的最大高度（像素）
#define EGL_MAX_PBUFFER_PIXELS		0x302B // Surface为pixel buffer时的最大尺寸（像素）
#define EGL_MAX_PBUFFER_WIDTH		0x302C // Surface为pixel buffer时的最大宽度（像素）
#define EGL_NATIVE_RENDERABLE		0x302D // 本地渲染API是否可以渲染Surface
#define EGL_NATIVE_VISUAL_ID        0x302E // 本地可视ID
#define EGL_NATIVE_VISUAL_TYPE		0x302F // 本地可视类型
#define EGL_SAMPLES					0x3031 // 每个像素采样个数
#define EGL_SAMPLE_BUFFERS			0x3032 // 多重采样缓冲区位数
#define EGL_SURFACE_TYPE			0x3033 // 支持的EGL Surface的掩码
#define EGL_TRANSPARENT_TYPE        0x3034 // 透明类型（EGL_NONE、EGL_TRANSPARENT_RGB）
#define EGL_TRANSPARENT_BLUE_VALUE	0x3035 // 透明的blue值
#define EGL_TRANSPARENT_GREEN_VALUE	0x3036 // 透明的green值
#define EGL_TRANSPARENT_RED_VALUE	0x3037 // 透明的red值
#define EGL_NONE					0x3038 // 固定用于属性列表结尾
#define EGL_BIND_TO_TEXTURE_RGB		0x3039 // 颜色缓冲区是否可以绑定RGB纹理
#define EGL_BIND_TO_TEXTURE_RGBA    0x303A // 颜色缓冲区是否可以绑定RGBA纹理
#define EGL_MIN_SWAP_INTERVAL		0x303B // eglSwapInterval参数的最小值
#define EGL_MAX_SWAP_INTERVAL       0x303C // eglSwapInterval参数的最大值
#define EGL_LUMINANCE_SIZE			0x303D // luminance缓冲区中luminance位数
#define EGL_ALPHA_MASK_SIZE			0x303E // alpha屏蔽缓冲区位数
#define EGL_COLOR_BUFFER_TYPE       0x303F // 颜色缓冲区类型（EGL_RGB_BUFFER、EGL_LUMINANCE_BUFFER）
#define EGL_RENDERABLE_TYPE			0x3040 // 支持的客户API上下文的掩码
#define EGL_MATCH_NATIVE_PIXMAP		0x3041
#define EGL_CONFORMANT				0x3042 // 客户API上下文与配置的一致性的掩码
*/