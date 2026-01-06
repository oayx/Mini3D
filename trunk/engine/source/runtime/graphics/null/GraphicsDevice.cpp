#include "GraphicsDevice.h"
#include "GPUAdapter.h"
#include "SwapChain.h"
#include "RenderContent.h"
#include "Texture.h"
#include "runtime/input/Input.h"
#include "runtime/Application.h"

DC_BEGIN_NAMESPACE
GraphicsCaps* GetGraphicsCaps()
{
	return Application::GetGraphics()->GetCaps();
}
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(GraphicsDevice, Object);
IMPL_DERIVED_REFECTION_TYPE(GraphicsCaps, Object);
IMPL_DERIVED_REFECTION_TYPE(GPUAdapterInfo, Object);
IMPL_DERIVED_REFECTION_TYPE(GPUAdapter, Object);
GraphicsDevice::GraphicsDevice()
{
	_swapChains.Resize((int)TargetDisplay::Max, nullptr);
}
GraphicsDevice::~GraphicsDevice()
{
	SAFE_DELETE(_graphicsCaps);
}
void GraphicsDevice::Release()
{
	SAFE_DELETE(_renderContent);
	SAFE_DELETE(_gpuAdapter);

	for (auto swap_chain : _swapChains)
	{
		SAFE_DELETE(swap_chain);
	}
	_swapChains.Clear();

	base::Release();
}
void GraphicsDevice::DestroyDevice(RenderWindow* window)
{
	//undo
}
void GraphicsDevice::DestroySwapChain(RenderWindow* window)
{
	SAFE_DELETE(_swapChains[int(window->GetTargetDisplay())]);
	_swapChains[int(window->GetTargetDisplay())] = nullptr;
}
void GraphicsDevice::DestroyRenderContent()
{
	SAFE_DELETE(_renderContent);
}
void GraphicsDevice::Resize(const WindowResizeDesc& desc)
{
	if (_swapChains[0])_swapChains[0]->Resize(desc);
	if (_renderContent)_renderContent->Resize(desc);
}
void GraphicsDevice::PreRender(RenderWindow* window)
{
	_currSwapChain = _swapChains[(int)window->GetTargetDisplay()];
	if (_renderContent)_renderContent->PreRender(window);
}
void GraphicsDevice::PostRender(RenderWindow* window)
{
	if (_renderContent)_renderContent->PostRender();
}
void GraphicsDevice::Present(RenderWindow* window)
{
	DC_PROFILE_FUNCTION;
	if (!_renderContent || !_currSwapChain)
		return;

	_renderContent->Present(window->GetSync());
	_currSwapChain->Present(window->GetSync());

	if (Input::GetKeyUp(KeyCode::PrintScreen))
	{
		TextureDesc desc;
		desc.type = TextureType::D2; desc.usage = GPUResourceUsage::Staging; desc.width = _currSwapChain->GetWidth(); desc.height = _currSwapChain->GetHeight();
		Texture* texture = Texture::Create(desc);
		_currSwapChain->Copy(texture);
		texture->SaveToFile("bg.png", ImageType::PNG);
	}

	_currSwapChain = nullptr;
}
DC_END_NAMESPACE