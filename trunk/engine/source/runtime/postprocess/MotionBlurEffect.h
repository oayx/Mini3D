 
/*****************************************************************************
* Author： hannibal
* Date：2021/6/18
* Description：运动模糊
*****************************************************************************/
#pragma once

#include "runtime/graphics/null/PostProcess.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL MotionBlurEffect final : public PostEffect
{
	friend class PostProcess;
	DECLARE_OBJECT_CLONE;
	DECLARE_OBJECT_SERIALIZE(ColorAdjustEffect);
	FRIEND_CONSTRUCT_DESTRUCT(MotionBlurEffect);
	BEGIN_DERIVED_REFECTION_TYPE(MotionBlurEffect, PostEffect)
		CTORS(DEFAULT_CTOR(MotionBlurEffect))
		ADD_EDITOR_COMPONENT("Motion Blur")
	END_REFECTION_TYPE;

	MotionBlurEffect();
	~MotionBlurEffect();

protected:
	virtual void OnRenderImage(RenderTexture* source, RenderTexture* dest)override;
	virtual void OnDrawEditor()override;

private:
	Material*	_material = nullptr;
	Matrix4		_matViewProj;
	Vector4		_intensity = Vector4(1.0f, 0.5f, 0.0f, 0.0f);
};
DC_END_NAMESPACE
