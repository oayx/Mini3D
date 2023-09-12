
/*****************************************************************************
* Author： hannibal
* Description：
*****************************************************************************/
#pragma once

#include "runtime/graphics/null/SwapChain.h"
#include "DX9Define.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class DX9SwapChain Final : public SwapChain
{
	friend class DX9Device;
	DEFAULT_CREATE(DX9SwapChain);
	FRIEND_CONSTRUCT_DESTRUCT(DX9SwapChain);
	DISALLOW_COPY_ASSIGN(DX9SwapChain);
	BEGIN_DERIVED_REFECTION_TYPE(DX9SwapChain, SwapChain)
	END_DERIVED_REFECTION_TYPE;

	DX9SwapChain();
	~DX9SwapChain();

public:
	virtual bool CreateSwapChain(RenderWindow* window)override;
	virtual void Resize(const WindowResizeDesc& desc)override;
	virtual void BeginFrame(RenderFrameDesc& desc)override;
	virtual void Present(uint sync)override;

	virtual bool Copy(Texture* texture)const override;
	virtual int  GetWidth()const { return _parameters.BackBufferWidth; }
	virtual int  GetHeight()const { return _parameters.BackBufferHeight; }

private:
	void CreateOrReset();
	void HandleLostDevice();
	void HandleResetDevice();

private:
	IDirect3DSwapChain9*	_swapChain = nullptr;
	IDirect3DSurface9*		_renderTarget = nullptr;
	IDirect3DSurface9*		_depthStencil = nullptr;
	D3DPRESENT_PARAMETERS	_parameters = {};
};
DC_END_NAMESPACE