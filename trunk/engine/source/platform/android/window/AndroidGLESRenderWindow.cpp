#if defined(DC_GRAPHICS_API_OPENGLES3)
#include "AndroidGLESRenderWindow.h"
#include "runtime/Application.h"
#include "platform/common/egl/EGL_Context.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(AndroidGLESRenderWindow, RenderWindow);
AndroidGLESRenderWindow::AndroidGLESRenderWindow()
{ 

}
AndroidGLESRenderWindow::~AndroidGLESRenderWindow()
{
	//EGL_Context::Destroy();
}
bool AndroidGLESRenderWindow::Create(WindowDesc& info)
{
	base::Create(info);

	_hWnd = (EGLNativeWindowType)info.hWnd;
	//android直接使用GLSurfaceView
	//return EGL_Context::Initialize(_hWnd, 3, _antiAlias);
	return true;
}
void AndroidGLESRenderWindow::Resize(WindowResizeDesc& desc)
{
	base::Resize(desc);
}
void AndroidGLESRenderWindow::Draw()
{
	base::Draw();
	//EGL_Context::Draw();
}
DC_END_NAMESPACE
#endif