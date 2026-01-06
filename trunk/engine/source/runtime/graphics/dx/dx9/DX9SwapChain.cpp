#include "DX9SwapChain.h"
#include "DX9Device.h"
#include "DX9Caps.h"
#include "core/Utility.h"
#include "runtime/graphics/null/Texture.h"
#include "runtime/Application.h"
#include "platform/RenderWindow.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(DX9SwapChain, SwapChain);
DX9SwapChain::DX9SwapChain()
{
	GetDX9Device()->AddEventListener(LostDeviceEvt, this, MakeEventHandler(DX9SwapChain::HandleLostDevice));
	GetDX9Device()->AddEventListener(ResetDeviceEvt, this, MakeEventHandler(DX9SwapChain::HandleResetDevice));

	_parameters.BackBufferCount = DX9BackBufferCount;
	_parameters.BackBufferFormat = DX9ConvertToAlphaFormat(DX9GetBackbufferFormat(DX9RenderTargetFormat));
	_parameters.EnableAutoDepthStencil = FALSE;
}
DX9SwapChain::~DX9SwapChain()
{
	GetDX9Device()->RemoveEventListener(LostDeviceEvt, this, MakeEventHandler(DX9SwapChain::HandleLostDevice));
	GetDX9Device()->RemoveEventListener(ResetDeviceEvt, this, MakeEventHandler(DX9SwapChain::HandleResetDevice));
	SAFE_RELEASE(_renderTarget);
	SAFE_RELEASE(_depthStencil);
	SAFE_RELEASE(_swapChain);
}
bool DX9SwapChain::CreateSwapChain(RenderWindow* window)
{
	_msaa = DX9Caps::GetMSAAQualityLevel(window->GetAntiAlias(), DX9GetBackbufferFormat(DX9RenderTargetFormat), true);
	_parameters.BackBufferWidth = window->GetWidth();
	_parameters.BackBufferHeight = window->GetHeight();
	_parameters.hDeviceWindow = (HWND)window->GetHwnd();
	_parameters.Windowed = !window->IsFullscreen();
	_parameters.MultiSampleType = (D3DMULTISAMPLE_TYPE)_msaa.Count;
	_parameters.MultiSampleQuality = (DWORD)_msaa.Quality;

	this->CreateOrReset();
	return base::CreateSwapChain(window);
}
void DX9SwapChain::Resize(const WindowResizeDesc& desc)
{
	_parameters.BackBufferWidth = desc.width;
	_parameters.BackBufferHeight = desc.height;
	_parameters.hDeviceWindow = (HWND)desc.hWnd;
	_parameters.Windowed = !desc.fullscreen;

	SAFE_RELEASE(_renderTarget);
	SAFE_RELEASE(_depthStencil);
	SAFE_RELEASE(_swapChain);
	//改变窗口大小会导致设备丢失，这里不处理，等待设备恢复
	base::Resize(desc);
}
void DX9SwapChain::CreateOrReset()
{
	_parameters.SwapEffect = _parameters.MultiSampleType != D3DMULTISAMPLE_NONE ? D3DSWAPEFFECT_DISCARD : D3DSWAPEFFECT_COPY;
	_parameters.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	_parameters.FullScreen_RefreshRateInHz = 0;
	DX9HR(GetDX9Device()->GetDevice()->CreateAdditionalSwapChain(&_parameters, &_swapChain));

	//render target
	DX9HR(_swapChain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &_renderTarget));

	//depth stencil
	DX9HR(GetDX9Device()->GetDevice()->CreateDepthStencilSurface(
		_parameters.BackBufferWidth,
		_parameters.BackBufferHeight,
		DX9GetDepthStencilFormat(DX9DepthStencilFormat, TextureFlag::DEPTH_AND_STENCIL),
		_parameters.MultiSampleType,
		_parameters.MultiSampleQuality,
		false,
		&_depthStencil,
		NULL));
}
void DX9SwapChain::BeginFrame(RenderFrameDesc& desc)
{
	desc.targetBuffer = _renderTarget;
	desc.depthStencilBuffer = _depthStencil;
	base::BeginFrame(desc);
}
void DX9SwapChain::Present(uint sync)
{
	HRESULT hr = _swapChain->Present(NULL, NULL, NULL, NULL, 0);
	if (hr == D3DERR_DEVICELOST || hr == D3DERR_DRIVERINTERNALERROR)
	{
		GetDX9Device()->SetDeviceInLostState(true);
		GetDX9Device()->HandleLostDevice();
	}
}	
bool DX9SwapChain::Copy(Texture* texture)const 
{
	HRESULT hr = E_FAIL;
	IDirect3DSurface9* backbuffer = nullptr;
	IDirect3DSurface9* surface = nullptr;
	do 
	{
		hr = GetDX9Device()->GetDevice()->GetRenderTarget(0, &backbuffer);
		if (FAILED(hr))break;

		D3DSURFACE_DESC rt_desc;
		hr = backbuffer->GetDesc(&rt_desc);
		if (FAILED(hr))break;

		IDirect3DSurface9* resolved_surface = nullptr;
		if (rt_desc.MultiSampleType != D3DMULTISAMPLE_NONE)
		{
			hr = GetDX9Device()->GetDevice()->CreateRenderTarget(rt_desc.Width, rt_desc.Height, rt_desc.Format, D3DMULTISAMPLE_NONE, 0, FALSE, &resolved_surface, NULL);
			if (FAILED(hr))break;
			hr = GetDX9Device()->GetDevice()->StretchRect(backbuffer, NULL, resolved_surface, NULL, D3DTEXF_NONE);
			if (FAILED(hr))break;

			SAFE_RELEASE(backbuffer);
			backbuffer = resolved_surface;
		}

		hr = GetDX9Device()->GetDevice()->CreateOffscreenPlainSurface(rt_desc.Width, rt_desc.Height, rt_desc.Format, D3DPOOL_SYSTEMMEM, &surface, NULL);
		if (FAILED(hr))break;
		hr = GetDX9Device()->GetDevice()->GetRenderTargetData(backbuffer, surface);
		if (FAILED(hr))break;

		D3DLOCKED_RECT rect;
		hr = surface->LockRect(&rect, nullptr, D3DLOCK_READONLY);
		if (FAILED(hr))break;
		{//复制数据
			TextureLockDesc lock_desc;
			byte* dst_by = texture->Lock(lock_desc);
			Memory::Copy(dst_by, rect.pBits, TotalBytes(texture->GetFormat(), rt_desc.Width, rt_desc.Height));
			texture->Unlock(lock_desc);
		}
		surface->UnlockRect();
	} while (false);

	SAFE_RELEASE(backbuffer);
	SAFE_RELEASE(surface);
	return hr == S_OK;
}
void DX9SwapChain::HandleLostDevice()
{
	SAFE_RELEASE(_renderTarget);
	SAFE_RELEASE(_depthStencil);
	SAFE_RELEASE(_swapChain);
}
void DX9SwapChain::HandleResetDevice()
{
	this->CreateOrReset();
}
DC_END_NAMESPACE