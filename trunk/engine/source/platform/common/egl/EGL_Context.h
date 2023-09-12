 
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
class ENGINE_DLL EGL_Context Final : public object
{
public:
	static bool Initialize(EGLNativeWindowType hWnd, int gl_version, int antiAlias);
	static void Destroy();
	static void Draw();

private:
	static bool CreateContext(EGLNativeWindowType hWnd, int gl_version, int antiAlias);
	static void DestroyContext();

private:
	static EGLDisplay m_display;
	static EGLContext m_context;
	static EGLSurface m_surface;
};
DC_END_NAMESPACE
#endif