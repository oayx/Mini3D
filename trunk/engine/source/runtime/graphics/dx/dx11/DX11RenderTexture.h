 
/*****************************************************************************
* Author： hannibal
* Date：2010/4/15
* Description： 
*****************************************************************************/
#pragma once

#include "runtime/graphics/null/RenderTexture.h"
#include "DX11Define.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
// ClassName：DX11RenderTexture
// Description：渲染到纹理
class DX11RenderTexture Final : public RenderTexture
{
	friend class DX11Device;
	DEFAULT_CREATE(DX11RenderTexture);
	FRIEND_CONSTRUCT_DESTRUCT(DX11RenderTexture);
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(DX11RenderTexture);
	BEGIN_DERIVED_REFECTION_TYPE(DX11RenderTexture, RenderTexture)
	END_DERIVED_REFECTION_TYPE;

protected:
	DX11RenderTexture(const TextureDesc& desc);

public:
	~DX11RenderTexture();
	virtual void  PreRender()override;
	virtual void  BeginFrame(RenderFrameDesc& desc)override;
	virtual void  PostRender()override;
	virtual void  Resize(const WindowResizeDesc& desc)override {}

	virtual bool  GetData(MemoryDataStream& stream)override;
	virtual bool  Copy(Texture* dst);
	virtual void* GetTexture()override;
	virtual void* GetTextureView()override;

	virtual void  SaveToFile(const String& name, ImageType type)override;

private:
	void		  CreateRenderTarget();
	void		  CreateDepthStencil();

protected:
	ID3D11Texture2D*			_colorTexture = nullptr;		//渲染目标纹理
	ID3D11ShaderResourceView*	_colorSRV = nullptr;
	ID3D11RenderTargetView*		_colorRTV[6] = { nullptr };	//可能是立方体
	ID3D11Texture2D*			_stagingTexture = nullptr;

	ID3D11Texture2D*			_depthTexture = nullptr;		//深度纹理
	ID3D11ShaderResourceView*	_depthSRV = nullptr;
	ID3D11DepthStencilView*		_depthDSV = nullptr;
};
DC_END_NAMESPACE
