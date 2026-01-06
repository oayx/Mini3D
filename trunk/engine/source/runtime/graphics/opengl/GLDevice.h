 
/*****************************************************************************
* Author： hannibal
* Date：2020/8/18
* Description：gl设备
*****************************************************************************/
#pragma once

#include "runtime/graphics/null/GraphicsDevice.h"
#include "GLDefine.h"
#include "GLSwapChain.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class GLDevice final : public GraphicsDevice
{
	friend class Application;
	DEFAULT_CREATE(GLDevice);
	FRIEND_CONSTRUCT_DESTRUCT(GLDevice);
	DISALLOW_COPY_ASSIGN(GLDevice);
	BEGIN_DERIVED_REFECTION_TYPE(GLDevice, GraphicsDevice)
	END_REFECTION_TYPE;

protected:
	GLDevice();
	~GLDevice();

public:
	virtual bool			CreateDevice(RenderWindow* window)override;
	virtual bool			CreateSwapChain(RenderWindow* window)override;
	virtual void			CreateRenderContent()override;
	virtual void			DestroyDevice(RenderWindow* window)override { base::DestroyDevice(window); }
	virtual void			DestroySwapChain(RenderWindow* window)override { base::DestroySwapChain(window); }
	virtual void			DestroyRenderContent()override { base::DestroyRenderContent(); }
	virtual void			Resize(const WindowResizeDesc& desc)override;

	virtual CGProgram*			CreateShader()override;
	virtual Texture*			CreateTexture(const TextureDesc& desc)override;
	virtual RenderTexture*		CreateRenderTexture(const TextureDesc& desc)override;
	virtual FinalProcess*		CreateFinalProcess()override;
	virtual HardwareVertexBuffer* CreateVertexBuffer()override;
	virtual HardwareIndexBuffer* CreateIndexBuffer()override;

public:
	GLShaderModel			GetShaderModel()const { return _shaderModel; }

private:
	GLShaderModel			_shaderModel = GLShaderModel::UNKNOWN;
};
DC_END_NAMESPACE