 
/*****************************************************************************
* Author： hannibal
* Date：2020/6/18
* Description：调整颜色
*****************************************************************************/
#pragma once

#include "runtime/graphics/null/PostProcess.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL ColorAdjustEffect Final : public PostEffect
{
	friend class PostProcess;
	DECLARE_OBJECT_CLONE;
	DECLARE_OBJECT_SERIALIZE(ColorAdjustEffect);
	FRIEND_CONSTRUCT_DESTRUCT(ColorAdjustEffect);
	BEGIN_DERIVED_REFECTION_TYPE(ColorAdjustEffect, PostEffect)
		CTORS(DEFAULT_CTOR(ColorAdjustEffect))
		ADD_EDITOR_COMPONENT("Color Adjust")
	END_DERIVED_REFECTION_TYPE;

	ColorAdjustEffect();
	~ColorAdjustEffect();

protected:
	virtual void OnRenderImage(RenderTexture* source, RenderTexture* dest)override;
	virtual void OnDrawEditor()override;

private:
	Material* _material = nullptr;
	float _brightness = 1.5f;
	float _saturation = 1.0f;
	float _contrast = 1.0f;
};
DC_END_NAMESPACE
