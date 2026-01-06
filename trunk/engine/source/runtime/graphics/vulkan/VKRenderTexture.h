/*****************************************************************************
* Author： hannibal
* Date：2010/7/21
* Description： 
*****************************************************************************/
#pragma once
 
#include "runtime/graphics/null/RenderTexture.h"
#include "VKDefine.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class VKRenderTexture final : public RenderTexture
{
	friend class VKDevice;
	DEFAULT_CREATE(VKRenderTexture);
	FRIEND_CONSTRUCT_DESTRUCT(VKRenderTexture);
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(VKRenderTexture);
	BEGIN_DERIVED_REFECTION_TYPE(VKRenderTexture, RenderTexture)
	END_REFECTION_TYPE;

protected:
	VKRenderTexture(const TextureDesc& desc);
public:
	~VKRenderTexture();
	virtual void  PreRender()override;
	virtual void  BeginFrame(RenderFrameDesc& desc)override;
	virtual void  PostRender()override;
	virtual void  Resize(const WindowResizeDesc& desc)override {}

	virtual bool  Copy(Texture* dst);
	virtual void* GetTexture()override { return nullptr; }
	virtual void* GetTextureView()override { return nullptr; }

	virtual void  SaveToFile(const String& name, ImageType type)override;

protected:
};
DC_END_NAMESPACE
