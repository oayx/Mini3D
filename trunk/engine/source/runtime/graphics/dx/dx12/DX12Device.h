 
/*****************************************************************************
* Author： hannibal
* Date：2020/3/30
* Description： d3d设备
*****************************************************************************/
#pragma once

#include "runtime/graphics/dx/DXGPUAdapter.h"
#include "runtime/graphics/null/GraphicsDevice.h"
#include "DX12Define.h"
#include "DX12SwapChain.h"

DC_BEGIN_NAMESPACE
class DX12SwapChain;
/********************************************************************/
class DX12Device final : public GraphicsDevice
{
	friend class Application;
	typedef Vector<DXGPUAdapterInfo*> DXGPUAdapters;
	DEFAULT_CREATE(DX12Device);
	FRIEND_CONSTRUCT_DESTRUCT(DX12Device);
	BEGIN_DERIVED_REFECTION_TYPE(DX12Device, GraphicsDevice)
	END_REFECTION_TYPE;

protected:
	DX12Device();
	~DX12Device();

public:
	virtual bool			CreateDevice(RenderWindow* window)override;
	virtual bool			CreateSwapChain(RenderWindow* window)override;
	virtual void			CreateRenderContent()override;
	virtual void			Resize(const WindowResizeDesc& desc)override;
	virtual void			PreRender(RenderWindow* window)override;
	virtual void			PostRender(RenderWindow* window)override;

	virtual CGProgram*		CreateShader()override;
	virtual Texture*		CreateTexture(const TextureDesc& desc)override;
	virtual RenderTexture*	CreateRenderTexture(const TextureDesc& desc)override;
	virtual FinalProcess*	CreateFinalProcess()override;
	virtual HardwareVertexBuffer* CreateVertexBuffer()override;
	virtual HardwareIndexBuffer* CreateIndexBuffer()override;

public:
	ID3D12Device*			GetDevice()const { return _d3Device; }
	IDXGIFactory4*			GetDXGIFactory()const { return _dXGIFactory; }
	D3D_FEATURE_LEVEL		GetFeatureLevel()const { return _featureLevel; }

	void					CreateCommandQueue();
	ID3D12CommandQueue*		GetCommandQueue()const { return _commandQueue; }
	ID3D12CommandAllocator* GetCommandAllocator()const { return _commandAllocator; }
	ID3D12GraphicsCommandList* GetCommandList()const { return _commandList; }
	void					FlushCommandQueue();

	void					CreateRootSignature();
	ID3D12RootSignature*	GetRootSignature()const { return _rootSignature; }

	uint					GetRtvDescriptorSize()const { return _rtvDescriptorSize; }
	uint 					GetDsvDescriptorSize()const { return _dsvDescriptorSize; }
	uint 					GetCbvSrvUavDescriptorSize()const { return _cbvSrvUavDescriptorSize; }

	int						GetStartTextureDescriptor()const { return 0; }
	int						GetStartCBufferDescriptor()const { return MaxTextureDescriptor; }

private:	
	IDXGIFactory4*			_dXGIFactory = nullptr;
	ID3D12Device*			_d3Device = nullptr;
	D3D_FEATURE_LEVEL		_featureLevel = D3D_FEATURE_LEVEL_12_0;

	ID3D12CommandAllocator* _commandAllocator = nullptr;
	ID3D12CommandQueue*		_commandQueue = nullptr;
	ID3D12GraphicsCommandList* _commandList = nullptr;

	ID3D12Fence*			_fence = nullptr;
	HANDLE					_fenceEvent = nullptr;
	UINT64                  _fenceValue = 0;

	ID3D12RootSignature*	_rootSignature = nullptr;

	uint					_rtvDescriptorSize = 0;
	uint 					_dsvDescriptorSize = 0;
	uint 					_cbvSrvUavDescriptorSize = 0;

	static const int		MaxTextureDescriptor = 10;
	static const int		MaxCBufferDescriptor = 16;
};
DC_END_NAMESPACE

