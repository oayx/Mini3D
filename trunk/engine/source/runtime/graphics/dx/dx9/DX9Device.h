
/*****************************************************************************
* Author： hannibal
* Date：2009年11月24日
* Description： d3d设备
*****************************************************************************/
#pragma once

#include "DX9Define.h"
#include "runtime/graphics/null/GraphicsDevice.h"

DC_BEGIN_NAMESPACE
class VideoModeList;
/********************************************************************/
class DX9Device Final : public GraphicsDevice
{
	friend class Application;
	DEFAULT_CREATE(DX9Device);
	FRIEND_CONSTRUCT_DESTRUCT(DX9Device);
	BEGIN_DERIVED_REFECTION_TYPE(DX9Device, GraphicsDevice)
	END_DERIVED_REFECTION_TYPE;

	DX9Device();
	~DX9Device();

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
	bool 					SetD3DRenderState(D3DRENDERSTATETYPE render_type, DWORD value);
	bool 					SetD3DSamplerState(DWORD sampler, D3DSAMPLERSTATETYPE sampler_type, DWORD value);
	bool 					SetD3DTextureStageState(DWORD stage, D3DTEXTURESTAGESTATETYPE type, DWORD value);
	bool 					ClearD3DTexture(int stage);
	bool 					SetD3DTexture(int stage, Texture *texture);
	bool 					SetD3DTexture(int stage, IDirect3DTexture9* texture);

public:
	LPDIRECT3D9				GetDirect3D()const{ return _d3DInstance; }
	LPDIRECT3DDEVICE9		GetDevice()const{ return _d3DDevice; }
	const D3DDISPLAYMODE&	GetDisplayMode()const { return _displayMode; }
	uint					GetAdapterIndex()const { return _d3DAdapter; }
	D3DDEVTYPE				GetDevType()const { return _d3DDevType; }

	bool					IsDeviceInLostState()const { return _deviceInLostState; }
	void					SetDeviceInLostState(bool b);
	void					HandlePreLostDevice();
	void					HandleLostDevice();
	void 					HandleResetDevice();
	void					CheckDeviceLost();

	void 					GetAdapterDisplaySize(int &width, int &height);

private:
	LPDIRECT3D9				_d3DInstance = nullptr;
	LPDIRECT3DDEVICE9		_d3DDevice = nullptr;

	uint					_d3DAdapter = D3DADAPTER_DEFAULT;
	D3DDEVTYPE				_d3DDevType = D3DDEVTYPE_HAL;
	D3DPRESENT_PARAMETERS	_d3DParameters = {};

	D3DDISPLAYMODE			_displayMode;					//当前显示器分辨率
	VideoModeList*			_videoList;					//分离率列表

	bool					_deviceInLostState = false;	//设备丢失
};//DX9Device
/********************************************************************/
// Description：支持的分辨率
class VideoModeList : public Object
{
	typedef Vector<D3DDISPLAYMODE> VecVideoMode;
	DEFAULT_CREATE(VideoModeList);
	FRIEND_CONSTRUCT_DESTRUCT(VideoModeList);
public:
	VideoModeList(LPDIRECT3D9 d3d);
	~VideoModeList();

private:
	void Enumerate(LPDIRECT3D9 d3d);

private:
	VecVideoMode _videoModes;
};//VideoModeList
DC_END_NAMESPACE