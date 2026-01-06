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
	return PlatformType::Windows;
#elif defined(DC_PLATFORM_LINUX)
	return PlatformType::Linux;
#elif defined(DC_PLATFORM_ANDROID)
	return PlatformType::Android;
#elif defined(DC_PLATFORM_MAC)
	return PlatformType::Mac;
#elif defined(DC_PLATFORM_IOS)
	return PlatformType::iOS;
#elif defined(DC_PLATFORM_WASM)
	return PlatformType::WASM;
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
#elif defined(DC_GRAPHICS_API_DX12)
	return RendererAPI::DirectX12;
#elif defined(DC_GRAPHICS_API_OPENGL)
	return RendererAPI::OpenGL;
#elif defined(DC_GRAPHICS_API_OPENGLES3)
	return RendererAPI::OpenGLES;
#elif defined(DC_GRAPHICS_API_VULKAN)
	return RendererAPI::Vulkan;
#elif defined(DC_GRAPHICS_API_METAL)
	return RendererAPI::Metal;
#elif defined(DC_PLATFORM_WASM)
	return PlatformType::OpenGLES;
#else
	#error "Unknown RendererAPI"
#endif
}
void PlatformBase::AdjustWindowRect(int& left, int& top, int& width, int& height, int screenWidth, int screenHeight, bool fullScreen)
{
	//限定窗口大小不超过屏幕尺寸
	if (!fullScreen)
	{
		if (width > screenWidth || width <= 0) width = screenWidth;
		if (height > screenHeight || height <= 0) height = screenHeight;
	}
	else
	{
		width = screenWidth;
		height = screenHeight;
	}

	//某个方向的位置为负数则使窗口剧中
	if (fullScreen)
	{
		left = top = 0;
	}
	else
	{
		if (left < 0)left = (screenWidth - width) / 2;
		if (top < 0)top = (screenHeight - height) / 2;
	}
}
DC_END_NAMESPACE
