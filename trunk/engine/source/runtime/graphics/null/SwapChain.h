
/*****************************************************************************
* Author： hannibal
* Description：
*****************************************************************************/
#pragma once

#include "core/Object.h"
#include "core/color/ColorConverter.h"
#include "runtime/CommonDefine.h"

DC_BEGIN_NAMESPACE
class Texture;
class RenderWindow;
/********************************************************************/
class SwapChain : public Object
{
	friend class GraphicsDevice;
	FRIEND_CONSTRUCT_DESTRUCT(SwapChain);
	DISALLOW_COPY_ASSIGN(SwapChain);
	BEGIN_DERIVED_REFECTION_TYPE(SwapChain, Object)
	END_REFECTION_TYPE;

protected:
	SwapChain() = default;
	~SwapChain() = default;

public:
	virtual bool CreateSwapChain(RenderWindow* window);
	virtual void Resize(const WindowResizeDesc& desc);
	virtual void BeginFrame(RenderFrameDesc& desc);
	virtual void EndFrame();
	virtual void Present(uint sync) = 0;

	virtual bool Copy(Texture* texture)const = 0;
	virtual int  GetWidth()const { return _backbufferWidth; }
	virtual int  GetHeight()const { return _backbufferHeight; }
	virtual const MSAALevel GetMsaa()const { return _msaa; }

protected:
	int _backbufferWidth = 0;
	int _backbufferHeight = 0;
	MSAALevel _msaa;
};
DC_END_NAMESPACE