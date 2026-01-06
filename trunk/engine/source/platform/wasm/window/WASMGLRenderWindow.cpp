#include "WASMGLRenderWindow.h"
#include "runtime/Application.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(WASMGLRenderWindow, GLFWRenderWindow);
WASMGLRenderWindow::WASMGLRenderWindow()
{

}
WASMGLRenderWindow::~WASMGLRenderWindow()
{
}
bool WASMGLRenderWindow::Create(WindowDesc& info)
{
	bool ret = base::Create(info);
	if (ret)
	{
	}

	return ret;
}
DC_END_NAMESPACE
