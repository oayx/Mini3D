
/*****************************************************************************
* Author： hannibal
* Date：2020/8/27 
* Description： 天空盒
*****************************************************************************/
#pragma once

#include "Renderer.h"

DC_BEGIN_NAMESPACE
class Camera;
class Texture;
/********************************************************************/
// 天空盒
class SkyBox final : public Object
{
	friend class Camera;
	DEFAULT_CREATE(SkyBox);
	FRIEND_CONSTRUCT_DESTRUCT(SkyBox);
	DISALLOW_COPY_ASSIGN(SkyBox);
	BEGIN_DERIVED_REFECTION_TYPE(SkyBox, Object)
		CTORS(DEFAULT_CTOR(SkyBox))
	END_REFECTION_TYPE;

	SkyBox();
	~SkyBox();

private:
	void Render(Camera* camera);
	void SetCubeFile(const String& file);

public:
	Texture* GetCubeTexture();

private:
	VariablePrimitive* _primitive = nullptr;
	Texture* _cubeTexture = nullptr;
	Matrix4 _matProj;
};//SkyBox
DC_END_NAMESPACE
