#include "PlatformBase.h"
#include "platform/PlatformDefine.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_REFECTION_TYPE(PlatformBase);
bool PlatformBase::Is64BitPlatform()
{
#ifdef PLATFORM_64BITS
	return true;
#else
	return false;
#endif
}
PlatformType PlatformBase::GetPlatformType()
{
#if defined(DC_PLATFORM_WIN32)
	return PlatformType::Win32;
#elif defined(DC_PLATFORM_LINUX)
	return PlatformType::Linux;
#elif defined(DC_PLATFORM_ANDROID)
	return PlatformType::Android;
#elif defined(DC_PLATFORM_MAC)
	return PlatformType::Mac;
#elif defined(DC_PLATFORM_IOS)
	return PlatformType::iOS;
#else
	#error "Unknown PlatformType"
#endif
}
RendererAPI PlatformBase::GetRendererAPI()
{
#if defined(DC_GRAPHICS_API_DX9)
	return RendererAPI::DirectX9;
#elif defined(DC_GRAPHICS_API_DX11)
	return RendererAPI::DirectX11;
#elif defined(DC_GRAPHICS_API_OPENGL)
	return RendererAPI::OpenGL;
#elif defined(DC_GRAPHICS_API_OPENGLES3)
	return RendererAPI::OpenGLES;
#elif defined(DC_GRAPHICS_API_METAL)
	return RendererAPI::Metal;
#else
	#error "Unknown RendererAPI"
#endif
}
void PlatformBase::AdjustWindowRect(int& left, int& top, int& width, int& height, int screen_width, int screen_height, bool full_screen)
{
	//限定窗口大小不超过屏幕尺寸
	if (!full_screen)
	{
		if (width > screen_width || width <= 0) width = screen_width;
		if (height > screen_height || height <= 0) height = screen_height;
	}
	else
	{
		width = screen_width;
		height = screen_height;
	}

	//某个方向的位置为负数则使窗口剧中
	if (full_screen)
	{
		left = top = 0;
	}
	else
	{
		if (left < 0)left = (screen_width - width) / 2;
		if (top < 0)top = (screen_height - height) / 2;
	}
}
DC_END_NAMESPACE
