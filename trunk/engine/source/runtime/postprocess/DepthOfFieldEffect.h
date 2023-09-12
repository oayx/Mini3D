 
/*****************************************************************************
* Author： hannibal
* Date：2021/10/8
* Description：景深
*****************************************************************************/
#pragma once

#include "runtime/graphics/null/PostProcess.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL DepthOfFieldEffect Final : public PostEffect
{
	friend class PostProcess;
	DECLARE_OBJECT_CLONE;
	DECLARE_OBJECT_SERIALIZE(ColorAdjustEffect);
	FRIEND_CONSTRUCT_DESTRUCT(DepthOfFieldEffect);
	BEGIN_DERIVED_REFECTION_TYPE(DepthOfFieldEffect, PostEffect)
		CTORS(DEFAULT_CTOR(DepthOfFieldEffect))
		ADD_EDITOR_COMPONENT("Depth Of Field")
	END_DERIVED_REFECTION_TYPE;

	DepthOfFieldEffect();
	~DepthOfFieldEffect();

protected:
	virtual void OnRenderImage(RenderTexture* source, RenderTexture* dest)override;
	virtual void OnDrawEditor()override;

	float FocalDistance01(float distance);

private:
	Material* _material = nullptr;

	int		_downSample = 1;

	float	_focalDistance = 10.0f;
	float	_focalLength = 1.0f;
	float	_nearBlurScale = 10.0f;
	float	_farBlurScale = 10.0f;
};
DC_END_NAMESPACE
