
#include "DX12SwapChain.h"
#include "DX12Caps.h"
#include "DX12Device.h"
#include "DX12RenderContent.h"
#include "runtime/graphics/null/Texture.h"
#include "runtime/Application.h"
#include "platform/RenderWindow.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(DX12SwapChain, SwapChain);
DX12SwapChain::DX12SwapChain()
{
	for (int i = 0; i < DX12BackBufferCount; ++i)
	{
		_swapChainBuffer[i] = nullptr;
	}
	this->CreateDescriptorHeaps();
}
DX12SwapChain::~DX12SwapChain()
{
	if (_swapChain != nullptr)
	{//如果在全屏模式，可能导致关闭失败，先切换到窗口模式
		_swapChain->SetFullscreenState(FALSE, NULL);
	}

	for (int i = 0; i < DX12BackBufferCount; ++i)
	{
		SAFE_RELEASE(_swapChainBuffer[i]);
	}
	SAFE_RELEASE(_depthStencilBuffer);

	SAFE_RELEASE(_rtvHeap);
	SAFE_RELEASE(_dsvHeap);
	SAFE_RELEASE(_swapChain);
}
bool DX12SwapChain::CreateSwapChain(RenderWindow* window)
{
	HWND hWnd = (HWND)window->GetHwnd();
	int antiAlias = window->GetAntiAlias();
	uint width = (uint)window->GetWidth();
	uint height = (uint)window->GetHeight();
	bool fullscreen = window->IsFullscreen();
	uint refreshRate = window->GetFPS();
	_msaa = DX12Caps::GetMSAAQualityLevel(antiAlias, DX10GetTextureFormat(DX12RenderTargetFormat, false));

	DXGI_SWAP_CHAIN_DESC1 sd = {};
	{
		sd.BufferCount = DX12BackBufferCount;
		sd.Width = width;
		sd.Height = height;
		sd.Format = DX10GetTextureFormat(DX12RenderTargetFormat, false);
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		sd.Scaling = DXGI_SCALING_STRETCH;
		sd.Stereo = FALSE;
	}
	IDXGISwapChain1* swap_chain = nullptr;
	DX_ERROR(GetDX12Device()->GetDXGIFactory()->CreateSwapChainForHwnd(GetDX12Device()->GetCommandQueue(), hWnd,  &sd, NULL, NULL, &swap_chain));
	DX_ERROR(swap_chain->QueryInterface(IID_PPV_ARGS(&_swapChain)));
	_swapChain->SetMaximumFrameLatency(DX12BackBufferCount);
	//_swapChain->SetColorSpace1(DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020);
	SAFE_RELEASE(swap_chain);

	//禁用Alt+Enter全屏切换
	DX_ERROR(GetDX12Device()->GetDXGIFactory()->MakeWindowAssociation(hWnd, DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER));

	this->CreateBackbufferResources(width, height, antiAlias);
	return base::CreateSwapChain(window);
}
void DX12SwapChain::Resize(const WindowResizeDesc& desc)
{
	CHECK_RETURN_PTR_VOID(_swapChain);
	this->ReleaseBackbufferResources();

	DXGI_SWAP_CHAIN_DESC sd;
	_swapChain->GetDesc(&sd);

	DX_ERROR(_swapChain->ResizeBuffers(DX12BackBufferCount,desc.width, desc.height, sd.BufferDesc.Format, sd.Flags));
	this->CreateBackbufferResources(desc.width, desc.height, sd.SampleDesc.Count);

	base::Resize(desc);
}	
void DX12SwapChain::BeginFrame(RenderFrameDesc& desc)
{
	auto resource_barrier = CD3DX12_RESOURCE_BARRIER::Transition(this->GetCurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	GetDX12Device()->GetCommandList()->ResourceBarrier(1, &resource_barrier);
	resource_barrier = CD3DX12_RESOURCE_BARRIER::Transition(_depthStencilBuffer, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);
	GetDX12Device()->GetCommandList()->ResourceBarrier(1, &resource_barrier);

	desc.targetBuffer = &_renderTargetDescriptor[_swapChain->GetCurrentBackBufferIndex()];
	desc.depthStencilBuffer = &_depthStencilDescriptor;
	base::BeginFrame(desc);
}
void DX12SwapChain::Present(uint sync)
{
	auto resource_barrier = CD3DX12_RESOURCE_BARRIER::Transition(this->GetCurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	GetDX12Device()->GetCommandList()->ResourceBarrier(1, &resource_barrier);
	resource_barrier = CD3DX12_RESOURCE_BARRIER::Transition(_depthStencilBuffer, D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_COMMON);
	GetDX12Device()->GetCommandList()->ResourceBarrier(1, &resource_barrier);

	if (_swapChain != nullptr)
	{
		DX_ERROR(_swapChain->Present(sync, 0));
	}

	GetDX12Device()->FlushCommandQueue();
}
ID3D12Resource* DX12SwapChain::GetCurrentBackBuffer()const
{
	UINT index = _swapChain->GetCurrentBackBufferIndex();
	return GetBackBuffer(index);
}
D3D12_CPU_DESCRIPTOR_HANDLE DX12SwapChain::GetCurrentBackBufferView()const
{
	UINT index = _swapChain->GetCurrentBackBufferIndex();
	return GetBackBufferView(index);
}
void DX12SwapChain::CreateDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = DX12BackBufferCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 1;
	DX_ERROR(GetDX12Device()->GetDevice()->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&_rtvHeap)));

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	DX_ERROR(GetDX12Device()->GetDevice()->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&_dsvHeap)));

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = _rtvHeap->GetCPUDescriptorHandleForHeapStart();
	for (UINT i = 0; i < DX12BackBufferCount; i++)
	{
		_renderTargetDescriptor[i] = rtvHandle;
		rtvHandle.ptr += GetDX12Device()->GetRtvDescriptorSize();
	}

	_depthStencilDescriptor = _dsvHeap->GetCPUDescriptorHandleForHeapStart();
}
void DX12SwapChain::CreateBackbufferResources(uint width, uint height, int antiAlias)
{
	{//render target
		for (UINT i = 0; i < DX12BackBufferCount; i++)
		{
			DX_ERROR(_swapChain->GetBuffer(i, IID_PPV_ARGS(&_swapChainBuffer[i])));
			GetDX12Device()->GetDevice()->CreateRenderTargetView(_swapChainBuffer[i], nullptr, _renderTargetDescriptor[i]);
		}
	}

	{//depth stencil
		// Create the depth/stencil buffer and view.
		D3D12_RESOURCE_DESC res_desc = {};
		res_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		res_desc.Alignment = 0;
		res_desc.Width = width;
		res_desc.Height = height;
		res_desc.DepthOrArraySize = 1;
		res_desc.MipLevels = 1;
		res_desc.Format = DX10GetTextureFormat(DX12DepthStencilFormat, false);
		res_desc.SampleDesc.Count = 1;
		res_desc.SampleDesc.Quality = 0;
		res_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		res_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE opt_clear = {};
		opt_clear.Format = DX10GetTextureViewFormat(DX12DepthStencilFormat, false);
		opt_clear.DepthStencil.Depth = 1.0f;
		opt_clear.DepthStencil.Stencil = 0;
		auto heap_properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		DX_ERROR(GetDX12Device()->GetDevice()->CreateCommittedResource(
			&heap_properties,
			D3D12_HEAP_FLAG_NONE,
			&res_desc,
			D3D12_RESOURCE_STATE_COMMON,
			&opt_clear,
			IID_PPV_ARGS(&_depthStencilBuffer)));

		// Create descriptor to mip level 0 of entire resource using the format of the resource.
		D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc = {};
		dsv_desc.Flags = D3D12_DSV_FLAG_NONE;
		dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsv_desc.Format = DX10GetTextureViewFormat(DX12DepthStencilFormat, false);
		dsv_desc.Texture2D.MipSlice = 0;
		GetDX12Device()->GetDevice()->CreateDepthStencilView(_depthStencilBuffer, &dsv_desc, GetDepthStencilView());
	}
}
void DX12SwapChain::ReleaseBackbufferResources()
{
	for (uint i = 0; i < DX12BackBufferCount; i++)
	{ 
		SAFE_RELEASE(_swapChainBuffer[i]);
	}
}
DC_END_NAMESPACE

