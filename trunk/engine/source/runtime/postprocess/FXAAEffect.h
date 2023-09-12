 
/*****************************************************************************
* Author： hannibal
* Date：2021/6/18
* Description：FXAA反走样
*****************************************************************************/
#pragma once

#include "runtime/graphics/null/PostProcess.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL FXAAEffect Final : public PostEffect
{
	friend class PostProcess;
	DECLARE_OBJECT_CLONE;
	DECLARE_OBJECT_SERIALIZE(ColorAdjustEffect);
	FRIEND_CONSTRUCT_DESTRUCT(FXAAEffect);
	BEGIN_DERIVED_REFECTION_TYPE(FXAAEffect, PostEffect)
		CTORS(DEFAULT_CTOR(FXAAEffect))
		ADD_EDITOR_COMPONENT("FXAA")
	END_DERIVED_REFECTION_TYPE;

	FXAAEffect();
	~FXAAEffect();

protected:
	virtual void OnRenderImage(RenderTexture* source, RenderTexture* dest)override;
	virtual void OnDrawEditor()override;

private:
	Material*	_material = nullptr;
};
DC_END_NAMESPACE
