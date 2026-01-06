 
/*****************************************************************************
* Author： hannibal
* Date：2020/8/18
* Description：egl
*****************************************************************************/
#pragma once
#if defined(DC_GRAPHICS_API_OPENGLES3)
#include "runtime/graphics/opengl/GLDefine.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
// egl
class ENGINE_DLL EGL_Context final : public object
{
public:
	static bool Initialize(EGLNativeWindowType hWnd, int gl_version, int antiAlias);
	static void Destroy();
	static void Draw();

private:
	static bool CreateContext(EGLNativeWindowType hWnd, int gl_version, int antiAlias);
	static void DestroyContext();

private:
	inline static EGLDisplay _display = EGL_NO_DISPLAY;
	inline static EGLContext _context = EGL_NO_CONTEXT;
	inline static EGLSurface _surface = EGL_NO_SURFACE;
};
DC_END_NAMESPACE
#endif