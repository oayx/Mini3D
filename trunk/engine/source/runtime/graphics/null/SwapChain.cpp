#include "SwapChain.h"
#include "runtime/Application.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(SwapChain, Object);
bool SwapChain::CreateSwapChain(RenderWindow* window) 
{ 
	_backbufferWidth = window->GetWidth(); 
	_backbufferHeight = window->GetHeight();
	return true;
}
void SwapChain::Resize(const WindowResizeDesc& desc)
{ 
	_backbufferWidth = desc.width; 
	_backbufferHeight = desc.height; 
}
void SwapChain::BeginFrame(RenderFrameDesc& desc)
{
	DC_PROFILE_FUNCTION;
	Application::GetGraphics()->GetRenderContent()->BeginFrame(desc);
}
void SwapChain::EndFrame()
{
	DC_PROFILE_FUNCTION;
	Application::GetGraphics()->GetRenderContent()->EndFrame();
}
DC_END_NAMESPACE