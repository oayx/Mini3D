 
/*****************************************************************************
* Author： hannibal
* Date：2020/6/18
* Description：显示深度和法线
*****************************************************************************/
#pragma once

#include "runtime/graphics/null/PostProcess.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL ShowDepthNormalEffect Final : public PostEffect
{
	friend class PostProcess;
	FRIEND_CONSTRUCT_DESTRUCT(ShowDepthNormalEffect);
	BEGIN_DERIVED_REFECTION_TYPE(ShowDepthNormalEffect, PostEffect)
		CTORS(DEFAULT_CTOR(ShowDepthNormalEffect))
	END_DERIVED_REFECTION_TYPE;

	ShowDepthNormalEffect();
	~ShowDepthNormalEffect();

protected:
	virtual void OnRenderImage(RenderTexture* source, RenderTexture* dest)override;

private:
	Material* _material = nullptr;
};
DC_END_NAMESPACE
