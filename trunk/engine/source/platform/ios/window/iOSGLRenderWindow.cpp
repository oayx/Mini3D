#include "iOSGLRenderWindow.h"
#include "runtime/Application.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(iOSGLRenderWindow, GLFWRenderWindow);
iOSGLRenderWindow::iOSGLRenderWindow()
{

}
iOSGLRenderWindow::~iOSGLRenderWindow()
{
}
bool iOSGLRenderWindow::Create(WindowDesc& info)
{
	bool ret = base::Create(info);
	if (ret)
	{
	}

	return ret;
}
DC_END_NAMESPACE
