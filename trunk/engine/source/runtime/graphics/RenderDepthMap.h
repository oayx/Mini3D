 
/*****************************************************************************
* Author： hannibal
* Date：2020/10/18
* Description：深度图(深度图在view空间计算，取值[0-1])
*****************************************************************************/
#pragma once

#include "core/Object.h"
#include "runtime/scene/SceneUtils.h"

DC_BEGIN_NAMESPACE
class Camera;
class Material;
class Renderer;
class RenderTexture;
/********************************************************************/
class RenderDepthMap final : public Object
{
	friend class Camera;
	DEFAULT_CREATE(RenderDepthMap);
	FRIEND_CONSTRUCT_DESTRUCT(RenderDepthMap);
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(RenderDepthMap);
	BEGIN_DERIVED_REFECTION_TYPE(RenderDepthMap, Object)
	END_REFECTION_TYPE;

private:
	RenderDepthMap(DepthTextureMode mode, int w, int h, ColorFormat format);
	~RenderDepthMap();

public:
	void PreRender();
	void PostRender();
	virtual void RenderOneObject(Camera* camera, Renderer *renderable);

public:
	RenderTexture* GetTexture()const { return _renderTexture; }
	DepthTextureMode GetMode()const { return _depthTextureMode; }

private:
	void CreateMaterial();

private:
	DepthTextureMode _depthTextureMode = DepthTextureMode::None;
	RenderTexture*	_renderTexture = nullptr;
	Material*		_defaultMaterial = nullptr;
};
DC_END_NAMESPACE
