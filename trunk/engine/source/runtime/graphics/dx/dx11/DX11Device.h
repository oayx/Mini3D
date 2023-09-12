 
/*****************************************************************************
* Author： hannibal
* Date：2020/3/30
* Description： d3d设备
*****************************************************************************/
#pragma once

#include "runtime/graphics/null/GraphicsDevice.h"
#include "DX11Define.h"
#include "DX11SwapChain.h"

DC_BEGIN_NAMESPACE
class DX11SwapChain;
/********************************************************************/
// d3d设备
class DX11Device Final : public GraphicsDevice
{
	friend class Application;
	DEFAULT_CREATE(DX11Device);
	FRIEND_CONSTRUCT_DESTRUCT(DX11Device);
	BEGIN_DERIVED_REFECTION_TYPE(DX11Device, GraphicsDevice)
	END_DERIVED_REFECTION_TYPE;

	DX11Device();
	~DX11Device();

public:
	virtual bool			CreateDevice(RenderWindow* window)override;
	virtual bool			CreateSwapChain(RenderWindow* window)override;
	virtual void			CreateRenderContent()override;
	virtual void			Resize(const WindowResizeDesc& desc)override;

	virtual CGProgram*			CreateShader()override;
	virtual Texture*		CreateTexture(const TextureDesc& desc)override;
	virtual RenderTexture*	CreateRenderTexture(const TextureDesc& desc)override;
	virtual FinalProcess*	CreateFinalProcess()override;
	virtual HardwareVertexBuffer* CreateVertexBuffer()override;
	virtual HardwareIndexBuffer* CreateIndexBuffer()override;

public:
	ID3D11Device*			GetDevice()const { return _d3Device; }
	ID3D11DeviceContext*	GetContent()const { return _immediateContext; }
	IDXGIFactory*			GetDXGIFactory()const { return _dXGIFactory; }

private:
	IDXGIFactory*			_dXGIFactory = nullptr;
	ID3D11Device*           _d3Device = nullptr;
	ID3D11DeviceContext*    _immediateContext = nullptr;
};//DX11Device
DC_END_NAMESPACE
