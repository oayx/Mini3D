 
/*****************************************************************************
* Author： hannibal
* Date：2021/6/16
* Description：图形工具
*****************************************************************************/
#pragma once

#include "core/Object.h"

DC_BEGIN_NAMESPACE
class Camera;
class Texture;
class Material;
class RenderTexture;
class VariablePrimitive;
/********************************************************************/
class ENGINE_DLL Graphics : public Object
{
	friend class Application;
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(Graphics);
	BEGIN_DERIVED_REFECTION_TYPE(Graphics, Object)
	END_REFECTION_TYPE;

private:
	static void Destroy();

public:
	static void Blit(Texture* source, RenderTexture* dest, Material* mat = nullptr, int pass = 0);
	static void Blit(Camera* camera, Texture* source, RenderTexture* dest, Material* mat = nullptr, int pass = 0);

private:
	inline static Material*			 _blitMaterial = nullptr;
	inline static VariablePrimitive* _primitive = nullptr;
};
DC_END_NAMESPACE