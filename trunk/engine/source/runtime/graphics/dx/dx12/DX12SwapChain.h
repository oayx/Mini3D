
/*****************************************************************************
* Author： hannibal
* Description：
*****************************************************************************/
#pragma once

#include "runtime/graphics/null/SwapChain.h"
#include "DX12Define.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class DX12SwapChain final : public SwapChain
{
	friend class DX12Device;
	DEFAULT_CREATE(DX12SwapChain);
	FRIEND_CONSTRUCT_DESTRUCT(DX12SwapChain);
	DISALLOW_COPY_ASSIGN(DX12SwapChain);
	BEGIN_DERIVED_REFECTION_TYPE(DX12SwapChain, SwapChain)
	END_REFECTION_TYPE;

	DX12SwapChain();
	~DX12SwapChain();

public:
	virtual bool	CreateSwapChain(RenderWindow* window)override;
	virtual void	Resize(const WindowResizeDesc& desc)override;
	virtual void	BeginFrame(RenderFrameDesc& desc)override;
	virtual void	Present(uint sync)override;

	virtual bool	Copy(Texture* texture)const override { return false; }

public:
	IDXGISwapChain3*GetSwapChain() { return _swapChain; }
	ID3D12Resource*	GetBackBuffer(int index)const { return _swapChainBuffer[index]; }
	ID3D12Resource*	GetCurrentBackBuffer()const;

	D3D12_CPU_DESCRIPTOR_HANDLE GetBackBufferView(int index)const { return _renderTargetDescriptor[index]; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferView()const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView()const { return _depthStencilDescriptor; }

private:
	void			CreateDescriptorHeaps();
	void			CreateBackbufferResources(uint width, uint height, int antiAlias);
	void			ReleaseBackbufferResources();

private:
	IDXGISwapChain3*		_swapChain = nullptr;

	ID3D12Resource*			_swapChainBuffer[DX12BackBufferCount] = {};
	ID3D12Resource*			_depthStencilBuffer = nullptr;
	D3D12_CPU_DESCRIPTOR_HANDLE _renderTargetDescriptor[DX12BackBufferCount] = {};
	D3D12_CPU_DESCRIPTOR_HANDLE _depthStencilDescriptor;

	ID3D12DescriptorHeap*	_rtvHeap = nullptr;		//Render Target View
	ID3D12DescriptorHeap*	_dsvHeap = nullptr;		//Depth And Stencil View
};
DC_END_NAMESPACE

