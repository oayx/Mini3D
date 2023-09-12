#include "ShowDepthMapEffect.h"
#include "runtime/input/Input.h"
#include "runtime/graphics/Material.h"
#include "runtime/graphics/Pass.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(ShowDepthMapEffect, PostEffect);
ShowDepthMapEffect::ShowDepthMapEffect()
{
	_material = Material::Create("internal/material/PostProcess/ShowDepthMap.material");
	_material->Retain();
}
ShowDepthMapEffect::~ShowDepthMapEffect()
{
	SAFE_RELEASE(_material);
}
void ShowDepthMapEffect::OnRenderImage(RenderTexture* source, RenderTexture* dest)
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
