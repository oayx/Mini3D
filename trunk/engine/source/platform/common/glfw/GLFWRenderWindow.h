 
/*****************************************************************************
* Author： hannibal
* Date：2020/3/9
* Description： opengl window(windows+linux)
*****************************************************************************/
#pragma once
#include "runtime/graphics/opengl/GLDefine.h"
#include "platform/RenderWindow.h"

#if defined(DC_PLATFORM_WIN32)
	#define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(DC_PLATFORM_LINUX)
	#define GLFW_EXPOSE_NATIVE_X11
#endif
#include <glfw/glfw3.h>
#include <glfw/glfw3native.h>

DC_BEGIN_NAMESPACE
/********************************************************************/
class GLFWRenderWindow : public RenderWindow
{
	friend class WindowManager;
	DISALLOW_COPY_ASSIGN(GLFWRenderWindow);
	BEGIN_DERIVED_REFECTION_TYPE(GLFWRenderWindow, RenderWindow)
	END_DERIVED_REFECTION_TYPE;

protected:
	GLFWRenderWindow();
	~GLFWRenderWindow();

public:
	virtual bool Create(WindowDesc& info)override;
	virtual void Resize(WindowResizeDesc& desc);
	virtual int  OnWinMsg(uint uMsg, int64 param1, int64 param2, int64 param3, int64 param4)override;
	virtual void Update()override;
	virtual void Draw()override;
	virtual void* GetHwnd()override;
	virtual GLFWwindow* GetGLFWwindow()const { return m_hWnd; }

private:
	static void Initialize();
	static void Destroy();
	static void PeekMessage();

protected:
	GLFWwindow* m_hWnd = nullptr;

	bool		m_isFirstMove = true;
	bool		m_isIgnoreSizeChange = false;//忽略大小改变消息
};
DC_END_NAMESPACE
