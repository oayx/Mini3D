#include "MacGLRenderWindow.h"
#include "runtime/Application.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(MacGLRenderWindow, GLFWRenderWindow);
MacGLRenderWindow::MacGLRenderWindow()
{

}
MacGLRenderWindow::~MacGLRenderWindow()
{
}
bool MacGLRenderWindow::Create(WindowDesc& info)
{
	bool ret = base::Create(info);
	if (ret)
	{
#if defined(DC_GRAPHICS_API_OPENGL)
		if (!gladLoadGL())
		{
			Debuger::Error("MacGLRenderWindow::Create - gladLoadGL error");
			return false;
		}
		Debuger::Debug("MacGLRenderWindow::Create - gladLoadGL succeed");
#endif
	}

	return ret;
}
DC_END_NAMESPACE
