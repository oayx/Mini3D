
/*****************************************************************************
* Author： hannibal
* Description：
*****************************************************************************/
#pragma once

#include "core/event/EventController.h"
#include "runtime/CommonDefine.h"
#include "runtime/graphics/RenderDefine.h"
#include "GraphicsCaps.h"

DC_BEGIN_NAMESPACE
class CGProgram;
class Texture;
class GPUAdapter;
class SwapChain;
class FinalProcess;
class RenderContent;
class RenderTexture;
class RenderWindow;
class HardwareVertexBuffer;
class HardwareIndexBuffer;
/********************************************************************/
class GraphicsDevice : public EventController
{
	friend class Application;
	typedef Vector<SwapChain*> SwapChains;
	DISALLOW_COPY_ASSIGN(GraphicsDevice);
	BEGIN_DERIVED_REFECTION_TYPE(GraphicsDevice, EventController)
	END_REFECTION_TYPE;

protected:
	GraphicsDevice();
	~GraphicsDevice();
	virtual void Release()override;

public:
	virtual bool			CreateDevice(RenderWindow* window) = 0;
	virtual bool			CreateSwapChain(RenderWindow* window) = 0;
	virtual void			CreateRenderContent() = 0;
	virtual void			DestroyDevice(RenderWindow* window);
	virtual void			DestroySwapChain(RenderWindow* window);
	virtual void			DestroyRenderContent();
	virtual void			Resize(const WindowResizeDesc& desc);
	virtual void			PreRender(RenderWindow* window);
	virtual void			PostRender(RenderWindow* window);
	virtual void			Present(RenderWindow* window);

	virtual CGProgram*		CreateShader() = 0;
	virtual Texture*		CreateTexture(const TextureDesc& desc) = 0;
	virtual RenderTexture*	CreateRenderTexture(const TextureDesc& desc) = 0;
	virtual FinalProcess*	CreateFinalProcess() = 0;
	virtual HardwareVertexBuffer* CreateVertexBuffer() = 0;
	virtual HardwareIndexBuffer* CreateIndexBuffer() = 0;

public:
	GraphicsCaps*			GetCaps()const { return _graphicsCaps; }
	RenderContent*			GetRenderContent()const { return _renderContent; }
	SwapChain*				GetSwapChain()const { return _currSwapChain == nullptr && !_swapChains.IsEmpty() ? _swapChains[0] : _currSwapChain; }
	const String&			GetShaderVersion()const { return _shaderVersion; }

protected:
	GPUAdapter*				_gpuAdapter = nullptr;
	GraphicsCaps*			_graphicsCaps = nullptr;
	RenderContent*			_renderContent = nullptr;
	SwapChain*				_currSwapChain = nullptr;
	SwapChains				_swapChains = {};
	String					_shaderVersion = "";
};
extern GraphicsCaps* GetGraphicsCaps();
DC_END_NAMESPACE