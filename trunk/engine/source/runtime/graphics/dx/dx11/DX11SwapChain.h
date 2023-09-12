
/*****************************************************************************
* Author： hannibal
* Description：
*****************************************************************************/
#pragma once

#include "runtime/graphics/null/SwapChain.h"
#include "DX11Define.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class DX11SwapChain Final : public SwapChain
{
	friend class DX11Device;
	DEFAULT_CREATE(DX11SwapChain);
	FRIEND_CONSTRUCT_DESTRUCT(DX11SwapChain);
	DISALLOW_COPY_ASSIGN(DX11SwapChain);
	BEGIN_DERIVED_REFECTION_TYPE(DX11SwapChain, SwapChain)
	END_DERIVED_REFECTION_TYPE;

	DX11SwapChain();
	~DX11SwapChain();

public:
	virtual bool	CreateSwapChain(RenderWindow* window)override;
	virtual void	Resize(const WindowResizeDesc& desc)override;
	virtual void	BeginFrame(RenderFrameDesc& desc)override;
	virtual void	Present(uint sync)override;

	virtual bool	Copy(Texture* texture)const override;

private:
	void			CreateBackbufferResources(uint width, uint height);
	void 			ReleaseBackbufferResources();

private:
	IDXGISwapChain*         _swapChain = nullptr;
	ID3D11Texture2D*		_renderTarget = nullptr;
	ID3D11RenderTargetView* _renderTargetView = nullptr;
	ID3D11Texture2D*        _depthStencil = nullptr;
	ID3D11DepthStencilView* _depthStencilView = nullptr;
};
DC_END_NAMESPACE