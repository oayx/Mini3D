#include "RenderWindow.h"
#include "core/event/Delegate.h"
#include "runtime/graphics/null/RenderContent.h"
#include "runtime/input/Input.h"
#include "runtime/scene/SceneManager.h"
#include "runtime/Application.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(RenderWindow, Object);
bool RenderWindow::Create(WindowDesc& info)
{
	_winName = info.name; 
	_position.Set(info.x, info.y);
	_winSize.Set((float)info.width, (float)info.height); 
	m_fullscreen = info.flags & WindowFlag::FullScreen;
	m_mouseClip = info.flags & WindowFlag::MouseClip;
	m_fps = info.fps; 
	_antiAlias = info.antiAlias < 1 ? 1 : info.antiAlias; 
	m_sync = info.sync;
	_targetDisplay = info.display;
	Debuger::Log("Create - pos: %d * %d, size %d * %d, fullscreen %s", info.x, info.y, info.width, info.height, (info.flags & WindowFlag::FullScreen ? "yes" : "no"));

	this->OnFocus(true); 
	return true;
}
void RenderWindow::Resize(WindowResizeDesc& desc)
{ 
	_winSize.Set((float)desc.width, (float)desc.height); 
	m_fullscreen = desc.fullscreen; 
	Debuger::Log("resize width:%d, height:%d, fullscreen:%s", desc.width, desc.height, desc.fullscreen ? "yes" : "no");
}
void RenderWindow::Draw()
{
	SceneManager::Render(this);
}
void RenderWindow::OnFocus(bool focused)
{
	m_isPaused = !focused;
	Application::Pause(m_isPaused);
	if (m_isPaused)
	{
		Input::Initialize();
	}
}
DC_END_NAMESPACE
