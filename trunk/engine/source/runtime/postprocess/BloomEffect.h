 
/*****************************************************************************
* Author： hannibal
* Date：2020/6/18
* Description：bloom
*****************************************************************************/
#pragma once

#include "runtime/graphics/null/PostProcess.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL BloomEffect final : public PostEffect
{
	friend class PostProcess;
	DECLARE_OBJECT_CLONE;
	DECLARE_OBJECT_SERIALIZE(ColorAdjustEffect);
	FRIEND_CONSTRUCT_DESTRUCT(BloomEffect);
	BEGIN_DERIVED_REFECTION_TYPE(BloomEffect, PostEffect)
		CTORS(DEFAULT_CTOR(BloomEffect))
		ADD_EDITOR_COMPONENT("Bloom")
	END_REFECTION_TYPE;

	BloomEffect();
	~BloomEffect();

protected:
	virtual void OnRenderImage(RenderTexture* source, RenderTexture* dest)override;
	virtual void OnDrawEditor()override;

private:
	Material* _material = nullptr;
	Color	_colorThreshold = Color::Gray;
	Color	_bloomColor = Color::White;
	float	_bloomFactor = 2.0f;
	int		_downSample = 1;
};
DC_END_NAMESPACE
