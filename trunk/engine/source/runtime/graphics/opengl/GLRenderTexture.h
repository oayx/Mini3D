/*****************************************************************************
* Author： hannibal
* Date：2010/7/21
* Description： 渲染到纹理
*****************************************************************************/
#pragma once
 
#include "runtime/graphics/null/RenderTexture.h"
#include "GLDefine.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class GLRenderTexture final : public RenderTexture
{
	friend class GLDevice;
	DEFAULT_CREATE(GLRenderTexture);
	FRIEND_CONSTRUCT_DESTRUCT(GLRenderTexture);
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(GLRenderTexture);
	BEGIN_DERIVED_REFECTION_TYPE(GLRenderTexture, RenderTexture)
	END_REFECTION_TYPE;

protected:
	GLRenderTexture(const TextureDesc& desc);
	~GLRenderTexture();

public:
	virtual void  PreRender()override;
	virtual void  BeginFrame(RenderFrameDesc& desc)override;
	virtual void  PostRender()override;
	virtual void  Resize(const WindowResizeDesc& desc)override {}

	virtual bool  GetData(MemoryDataStream& stream)override;
	virtual bool  Copy(Texture* dst)override;
	virtual void* GetTexture()override;
	virtual void* GetTextureView()override { return GetTexture(); }

	virtual void  SaveToFile(const String& name, ImageType type)override;

private:
	void		  CreateRenderTarget();
	void		  CreateDepthStencil();

protected:
	GLuint _frameBuffer = 0;	//FBO对象
	GLuint _colorTexture = 0;	//颜色纹理
	GLuint _depthTexture = 0;	//深度+模板纹理
};
DC_END_NAMESPACE
