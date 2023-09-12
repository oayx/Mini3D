#include "DX11SwapChain.h"
#include "DX11Caps.h"
#include "DX11Device.h"
#include "runtime/graphics/null/Texture.h"
#include "runtime/Application.h"
#include "platform/RenderWindow.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(DX11SwapChain, SwapChain);
DX11SwapChain::DX11SwapChain()
{

}
DX11SwapChain::~DX11SwapChain()
{
	if (_swapChain != nullptr)
	{
		//如果在全屏模式，可能导致关闭失败，先切换到窗口模式
		_swapChain->SetFullscreenState(FALSE, NULL);
	}
	ReleaseBackbufferResources();
	SAFE_RELEASE(_swapChain);
}
bool DX11SwapChain::CreateSwapChain(RenderWindow* window)
{
	HWND hWnd = (HWND)window->GetHwnd();
	int antiAlias = window->GetAntiAlias();
	uint width = (uint)window->GetWidth();
	uint height = (uint)window->GetHeight();
	bool fullscreen = window->IsFullscreen();
	uint refreshRate = window->GetFPS();
	_msaa = DX11Caps::GetMSAAQualityLevel(antiAlias, DX11GetTextureFormat(DXRenderTargetFormat, false));

	ReleaseBackbufferResources();

	Debuger::Log("MSAA Count:%d Quality:%d", _msaa.Count, _msaa.Quality);

	//创建
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.RefreshRate.Numerator = refreshRate;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DX11GetTextureFormat(DXRenderTargetFormat, false);
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.SampleDesc.Count = _msaa.Count;
	sd.SampleDesc.Quality = (_msaa.Count > 1 && _msaa.Quality > 0) ? _msaa.Quality - 1 : 0;
	sd.OutputWindow = hWnd;
	sd.Windowed = true;//建议设置true，然后通过SetFullscreenState实现全屏
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	HR(GetDX11Device()->GetDXGIFactory()->CreateSwapChain(GetDX11Device()->GetDevice(), &sd, &_swapChain));

	// 禁用Alt+Enter全屏切换
	HR(GetDX11Device()->GetDXGIFactory()->MakeWindowAssociation(hWnd, DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER));
	//全屏
	_swapChain->SetFullscreenState(fullscreen, NULL);

	CreateBackbufferResources(width, height);

	return base::CreateSwapChain(window);
}
void DX11SwapChain::Resize(const WindowResizeDesc& desc)
{
	CHECK_RETURN_PTR_VOID(_swapChain);
	this->ReleaseBackbufferResources();

	DXGI_SWAP_CHAIN_DESC sd = {};
	_swapChain->GetDesc(&sd);

	DXGI_MODE_DESC mode = {};
	mode.Width = desc.width;
	mode.Height = desc.height;
	mode.RefreshRate.Numerator = sd.BufferDesc.RefreshRate.Numerator;
	mode.RefreshRate.Denominator = 1;
	mode.Format = sd.BufferDesc.Format;
	mode.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	mode.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//顺序不要调整，否则会出现全屏和窗口切换失败问题
	HR(_swapChain->ResizeTarget(&mode));
	HR(_swapChain->ResizeBuffers(sd.BufferCount, desc.width, desc.height, sd.BufferDesc.Format, sd.Flags));
	HR(_swapChain->SetFullscreenState(desc.fullscreen, NULL));
	if (desc.fullscreen)
	{
		mode.RefreshRate.Numerator = 0;
		mode.RefreshRate.Denominator = 0;
		_swapChain->ResizeTarget(&mode);
	}

	this->CreateBackbufferResources(desc.width, desc.height);
	base::Resize(desc);
}
void DX11SwapChain::BeginFrame(RenderFrameDesc& desc)
{
	DC_PROFILE_FUNCTION();
	desc.target_buffer = _renderTargetView;
	desc.depth_stencil_buffer = _depthStencilView;
	base::BeginFrame(desc);
}
void DX11SwapChain::Present(uint sync)
{
	DC_PROFILE_FUNCTION();
	if (_swapChain != nullptr)
	{
		HR(_swapChain->Present(sync, 0));
	}
}
bool DX11SwapChain::Copy(Texture* texture)const
{
	if (!_renderTarget || !texture || !texture->IsStaging())return false;

	ID3D11Texture2D* backbuffer_texture;
	HR(_renderTarget->QueryInterface(IID_PPV_ARGS(&backbuffer_texture)));

	D3D11_TEXTURE2D_DESC desc;
	backbuffer_texture->GetDesc(&desc);
	if (desc.SampleDesc.Count > 1)
	{//如果是MSAA：先渲染到一张普通纹理，然后通过CopyResource复制到D3D11_USAGE_STAGING纹理
		desc.SampleDesc.Count = 1;// 需要去掉msaa
		desc.SampleDesc.Quality = 0;

		ID3D11Texture2D* temp_texture;
		HR(GetDX11Device()->GetDevice()->CreateTexture2D(&desc, nullptr, &temp_texture));
		DXGI_FORMAT format = DX11EnsureNotTypeless(desc.Format);

		UINT support_format = 0;
		HR(GetDX11Device()->GetDevice()->CheckFormatSupport(format, &support_format));
		if (!(support_format & D3D11_FORMAT_SUPPORT_MULTISAMPLE_RESOLVE))
		{
			SAFE_RELEASE(temp_texture);
			return false;
		}

		for (UINT item = 0; item < desc.ArraySize; ++item)
		{
			for (UINT level = 0; level < desc.MipLevels; ++level)
			{
				UINT index = D3D11CalcSubresource(level, item, desc.MipLevels);
				GetDX11Device()->GetContent()->ResolveSubresource(temp_texture, index, backbuffer_texture, index, format);
			}
		}

		GetDX11Device()->GetContent()->CopyResource((ID3D11Resource*)texture->GetTexture(), temp_texture);
		SAFE_RELEASE(temp_texture);
	}
	else
	{
		GetDX11Device()->GetContent()->CopyResource((ID3D11Resource*)texture->GetTexture(), _renderTarget);
	}
	return true;
}
void DX11SwapChain::CreateBackbufferResources(uint width, uint height)
{
	{//render target
		HR(_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)(&_renderTarget)));

		// Create a render target view
		D3D11_RENDER_TARGET_VIEW_DESC rt_desc = {};
		rt_desc.Format = DX11GetShaderViewFormat(DXRenderTargetFormat, false);
		rt_desc.ViewDimension = _msaa.Count > 1 ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;
		rt_desc.Texture2D.MipSlice = 0;
		HR(GetDX11Device()->GetDevice()->CreateRenderTargetView(_renderTarget, &rt_desc, &_renderTargetView));
	}

	{//depth stencil
		// Create depth stencil texture
		D3D11_TEXTURE2D_DESC depth_desc = {};
		depth_desc.Width = width;
		depth_desc.Height = height;
		depth_desc.MipLevels = 1;
		depth_desc.ArraySize = 1;
		depth_desc.Format = DX11GetDepthTextureFormat(DXDepthStencilFormat);
		depth_desc.SampleDesc.Count = _msaa.Count;
		depth_desc.SampleDesc.Quality = 0;
		depth_desc.Usage = D3D11_USAGE_DEFAULT;
		depth_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depth_desc.CPUAccessFlags = 0;
		depth_desc.MiscFlags = 0;
		HR(GetDX11Device()->GetDevice()->CreateTexture2D(&depth_desc, NULL, &_depthStencil));

		// Create the depth stencil view
		D3D11_DEPTH_STENCIL_VIEW_DESC depth_view_desc = {};
		depth_view_desc.Format = DX11GetDepthTextureViewFormat(DXDepthStencilFormat);
		depth_view_desc.ViewDimension = _msaa.Count > 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
		depth_view_desc.Texture2D.MipSlice = 0;
		HR(GetDX11Device()->GetDevice()->CreateDepthStencilView(_depthStencil, &depth_view_desc, &_depthStencilView));
	}
	//GetDX11Device()->GetContent()->OMSetRenderTargets(1, &_renderTargetView, _depthStencilView);
}
void DX11SwapChain::ReleaseBackbufferResources()
{
	SAFE_RELEASE(_renderTarget);
	SAFE_RELEASE(_renderTargetView);
	SAFE_RELEASE(_depthStencil);
	SAFE_RELEASE(_depthStencilView);
}
DC_END_NAMESPACE