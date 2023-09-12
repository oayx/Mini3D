#include "LinuxGLRenderWindow.h"
#include "runtime/Application.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(LinuxGLRenderWindow, GLFWRenderWindow);
LinuxGLRenderWindow::LinuxGLRenderWindow()
{

}
LinuxGLRenderWindow::~LinuxGLRenderWindow()
{
}
bool LinuxGLRenderWindow::Create(WindowDesc& info)
{
	bool ret = base::Create(info);
	if (ret)
	{
#if defined(DC_GRAPHICS_API_OPENGL)
		if (!gladLoadGL())
		{
			Debuger::Error("LinuxGLRenderWindow::Create - gladLoadGL");
			return false;
		}
#endif
	}

	return ret;
}
DC_END_NAMESPACE
