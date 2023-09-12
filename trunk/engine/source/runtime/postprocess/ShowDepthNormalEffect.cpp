#include "ShowDepthNormalEffect.h"
#include "runtime/input/Input.h"
#include "runtime/graphics/Material.h"
#include "runtime/graphics/Pass.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(ShowDepthNormalEffect, PostEffect);
ShowDepthNormalEffect::ShowDepthNormalEffect()
{
	_material = Material::Create("internal/material/PostProcess/ShowDepthNormal.material");
	_material->Retain();
}
ShowDepthNormalEffect::~ShowDepthNormalEffect()
{
	SAFE_RELEASE(_material);
}
void ShowDepthNormalEffect::OnRenderImage(RenderTexture* source, RenderTexture* dest)
{
	if (_material != nullptr)
	{
		Blit(source, dest, _material);
	}
	else
	{
		Blit(source, dest);
	}
}
DC_END_NAMESPACE
