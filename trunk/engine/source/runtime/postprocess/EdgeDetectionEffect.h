 
/*****************************************************************************
* Author： hannibal
* Date：2021/6/18
* Description：边缘检测
*****************************************************************************/
#pragma once

#include "runtime/graphics/null/PostProcess.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL EdgeDetectionEffect Final : public PostEffect
{
	friend class PostProcess;
	DECLARE_OBJECT_CLONE;
	DECLARE_OBJECT_SERIALIZE(ColorAdjustEffect);
	FRIEND_CONSTRUCT_DESTRUCT(EdgeDetectionEffect);
	BEGIN_DERIVED_REFECTION_TYPE(EdgeDetectionEffect, PostEffect)
		CTORS(DEFAULT_CTOR(EdgeDetectionEffect))
		ADD_EDITOR_COMPONENT("Edge Detection")
	END_DERIVED_REFECTION_TYPE;

	EdgeDetectionEffect();
	~EdgeDetectionEffect();

protected:
	virtual void OnRenderImage(RenderTexture* source, RenderTexture* dest)override;
	virtual void OnDrawEditor()override;

private:
	Material*	_material = nullptr;
	Color		_edgeColor = Color::White;
};
DC_END_NAMESPACE
