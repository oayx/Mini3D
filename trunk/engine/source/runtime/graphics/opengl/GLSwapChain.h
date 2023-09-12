
/*****************************************************************************
* Author： hannibal
* Description：
*****************************************************************************/
#pragma once

#include "runtime/graphics/null/SwapChain.h"
#include "GLDefine.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class GLSwapChain Final : public SwapChain
{
	friend class GLDevice;
	DEFAULT_CREATE(GLSwapChain);
	FRIEND_CONSTRUCT_DESTRUCT(GLSwapChain);
	DISALLOW_COPY_ASSIGN(GLSwapChain);
	BEGIN_DERIVED_REFECTION_TYPE(GLSwapChain, SwapChain)
	END_DERIVED_REFECTION_TYPE;

	GLSwapChain();
	~GLSwapChain();

public:
	virtual bool CreateSwapChain(RenderWindow* window)override;
	virtual void Resize(const WindowResizeDesc& desc)override;
	virtual void BeginFrame(RenderFrameDesc& desc)override;
	virtual void Present(uint sync)override;

	virtual bool Copy(Texture* texture)const override;

private:
};
DC_END_NAMESPACE