 
/*****************************************************************************
* Author： hannibal
* Date：2010/4/15
* Description： 
*****************************************************************************/
#pragma once

#include "runtime/graphics/null/RenderTexture.h"
#include "DX12Define.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class DX12RenderTexture final : public RenderTexture
{
	friend class DX12Device;
	DEFAULT_CREATE(DX12RenderTexture);
	FRIEND_CONSTRUCT_DESTRUCT(DX12RenderTexture);
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(DX12RenderTexture);
	BEGIN_DERIVED_REFECTION_TYPE(DX12RenderTexture, RenderTexture)
	END_REFECTION_TYPE;

protected:
	DX12RenderTexture(const TextureDesc& desc);
public:
	~DX12RenderTexture();
	virtual void  PreRender()override;
	virtual void  BeginFrame(RenderFrameDesc& desc)override;
	virtual void  PostRender()override;
	virtual void  Resize(const WindowResizeDesc& desc)override {}

	virtual bool  Copy(Texture* dst) { return false; }
	virtual void* GetTexture()override { return _colorTexture; }
	virtual void* GetTextureView()override { return _colorTextureView; }

	virtual void SaveToFile(const String& name, ImageType type)override {}

private:
	void		CreateRenderTexture();
	void		CreateDepthStencilTexture();

protected:
	ID3D12Resource*			_colorTexture = nullptr;		//渲染目标视图
	ID3D12DescriptorHeap*	_colorTextureView = nullptr;	//shader视图

	ID3D12DescriptorHeap*	_renderTargetView = nullptr;
	ID3D12DescriptorHeap*	_depthStencilView = nullptr;
};
DC_END_NAMESPACE

