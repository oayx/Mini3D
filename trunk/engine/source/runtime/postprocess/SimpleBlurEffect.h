 
/*****************************************************************************
* Author： hannibal
* Date：2020/6/18
* Description：均值模糊
*****************************************************************************/
#pragma once

#include "runtime/graphics/null/PostProcess.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL SimpleBlurEffect final : public PostEffect
{
	friend class PostProcess;
	DECLARE_OBJECT_CLONE;
	DECLARE_OBJECT_SERIALIZE(ColorAdjustEffect);
	FRIEND_CONSTRUCT_DESTRUCT(SimpleBlurEffect);
	BEGIN_DERIVED_REFECTION_TYPE(SimpleBlurEffect, PostEffect)
		CTORS(DEFAULT_CTOR(SimpleBlurEffect))
		ADD_EDITOR_COMPONENT("Simple Blur")
	END_REFECTION_TYPE;

	SimpleBlurEffect();
	~SimpleBlurEffect();

protected:
	virtual void OnRenderImage(RenderTexture* source, RenderTexture* dest)override;
	virtual void OnDrawEditor()override;

private:
	Material* _material = nullptr;
	float _blurRadius = 1.0f;
};
DC_END_NAMESPACE
