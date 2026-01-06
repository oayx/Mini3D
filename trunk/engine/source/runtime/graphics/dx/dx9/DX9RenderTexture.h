
/*****************************************************************************
* Author： hannibal
* Date：2010年4月15日
* Description： 
*****************************************************************************/
#pragma once

#include "DX9Define.h"
#include "runtime/graphics/null/RenderTexture.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
// ClassName：DX9RenderTexture
// Description：渲染到纹理
class DX9RenderTexture final : public RenderTexture
{
	friend class DX9Device;
	DEFAULT_CREATE(DX9RenderTexture);
	FRIEND_CONSTRUCT_DESTRUCT(DX9RenderTexture);
	DISALLOW_COPY_ASSIGN(DX9RenderTexture);
	BEGIN_DERIVED_REFECTION_TYPE(DX9RenderTexture, RenderTexture)
	END_REFECTION_TYPE;

	DX9RenderTexture(const TextureDesc& desc);
	~DX9RenderTexture();

public:
	virtual void  PreRender()override {}
	virtual void  BeginFrame(RenderFrameDesc& desc)override;
	virtual void  PostRender()override {}
	virtual void  Resize(const WindowResizeDesc& desc)override {}

	virtual bool  GetData(MemoryDataStream& stream)override;
	virtual bool  Copy(Texture* dst)override;
	virtual void* GetTexture()override { return _colorTexture; }
	virtual void* GetTextureView()override { return _colorTexture; }

	virtual void  SaveToFile(const String& name, ImageType format)override;

private:
	void		CreateRenderTarget();
	void		CreateDepthStencil();

protected:
	IDirect3DBaseTexture9*	_colorTexture = nullptr;
	IDirect3DSurface9*		_colorSurface[6] = { nullptr };
	IDirect3DSurface9*		_depthStencilSurface = nullptr;
};
DC_END_NAMESPACE