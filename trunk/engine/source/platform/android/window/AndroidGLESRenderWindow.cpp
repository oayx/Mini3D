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
	EGL_Context::Destroy();
}
bool AndroidGLESRenderWindow::Create(WindowDesc& info)
{
	base::Create(info);

	m_hWnd = (EGLNativeWindowType)info.hWnd;
	return EGL_Context::Initialize(m_hWnd, 3, _antiAlias);
}
void AndroidGLESRenderWindow::Resize(WindowResizeDesc& desc)
{
	base::Resize(desc);
	EGL_Context::Initialize(m_hWnd, 3, _antiAlias);
}
void AndroidGLESRenderWindow::Draw()
{
	base::Draw();
	EGL_Context::Draw();
}
DC_END_NAMESPACE
#endif