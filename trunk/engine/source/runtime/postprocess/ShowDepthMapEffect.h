 
/*****************************************************************************
* Author： hannibal
* Date：2020/6/18
* Description：显示深度
*****************************************************************************/
#pragma once

#include "runtime/graphics/null/PostProcess.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL ShowDepthMapEffect Final : public PostEffect
{
	friend class PostProcess;
	FRIEND_CONSTRUCT_DESTRUCT(ShowDepthMapEffect);
	BEGIN_DERIVED_REFECTION_TYPE(ShowDepthMapEffect, PostEffect)
		CTORS(DEFAULT_CTOR(ShowDepthMapEffect))
	END_DERIVED_REFECTION_TYPE;

	ShowDepthMapEffect();
	~ShowDepthMapEffect();

protected:
	virtual void OnRenderImage(RenderTexture* source, RenderTexture* dest)override;

private:
	Material* _material = nullptr;
};
DC_END_NAMESPACE
