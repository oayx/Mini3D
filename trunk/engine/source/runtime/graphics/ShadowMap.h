 
/*****************************************************************************
* Author： hannibal
* Date：2020/3/18
* Description：阴影
*****************************************************************************/
#pragma once

#include "core/Object.h"

DC_BEGIN_NAMESPACE
class Light;
class Camera;
class Material;
class Renderer;
class RenderTexture;
/********************************************************************/
class ShadowMap final : public Object
{
	friend class Light;
	DEFAULT_CREATE(ShadowMap);
	FRIEND_CONSTRUCT_DESTRUCT(ShadowMap);
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(ShadowMap);
	BEGIN_DERIVED_REFECTION_TYPE(ShadowMap, Object)
	END_REFECTION_TYPE;

private:
	ShadowMap(int w, int h, ColorFormat format);
	virtual ~ShadowMap();

public:
	void			PreRender(Camera* camera, Light* light);
	void			PostRender();
	virtual void	RenderOneObject(Camera* camera, Light* light, Renderer *renderable);

public:
	RenderTexture*	GetTexture()const { return _renderTexture; }
	const Size&		GetShadowMapSize()const { return _shadowMapSize; }

private:
	Material*		GetMaterial();

private:
	RenderTexture*	_renderTexture = nullptr;
	Size			_shadowMapSize;

	Matrix4			_viewProj;
	Material*		_defaultMaterial = nullptr;
};
DC_END_NAMESPACE
