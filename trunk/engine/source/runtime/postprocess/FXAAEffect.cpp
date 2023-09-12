#include "FXAAEffect.h"
#include "runtime/input/Input.h"
#include "runtime/graphics/Material.h"
#include "runtime/graphics/Pass.h"
#include "runtime/graphics/null/RenderTexture.h"
#include "runtime/scene/Camera.h"
#include "editor/common/ECommonComponent.h"
#include "editor/EditorAppliction.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(FXAAEffect, PostEffect);
FXAAEffect::FXAAEffect()
{
	_material = Material::Create("internal/material/PostProcess/FXAA.material");
	_material->Retain();
}
FXAAEffect::~FXAAEffect()
{
	SAFE_RELEASE(_material);
}
Object* FXAAEffect::Clone(Object* new_obj)
{
	base::Clone(new_obj);
	FXAAEffect* obj = dynamic_cast<FXAAEffect*>(new_obj);
	if (!obj)return new_obj;

	return obj;
}
void FXAAEffect::OnRenderImage(RenderTexture* source, RenderTexture* dest)
{
	if (_material != nullptr)
	{
		float rcpWidth = 1.0f / _camera->GetRenderTargetWidth();
		float rcpHeight = 1.0f / _camera->GetRenderTargetHeight();

		TextureDesc desc;
		desc.width = source->GetWidth(); desc.height = source->GetHeight(); desc.format = source->GetFormat();
		desc.flags = TextureFlag::COLOR;
		RenderTexture* tmp_texture = RenderTexture::Alloc(desc);
		_material->SetVector("_RcpFrame", Vector4(rcpWidth, rcpHeight, 0, 0));
		_material->SetVector("_RcpFrameOpt", Vector4(rcpWidth * 2, rcpHeight * 2, rcpWidth * 0.5f, rcpHeight * 0.5f));
		Blit(source, tmp_texture, _material);
		Blit(tmp_texture, dest);
		RenderTexture::Free(tmp_texture);
	}
	else
	{
		Blit(source, dest);
	}
}
void FXAAEffect::OnDrawEditor()
{
	base::OnDrawEditor();
}
/********************************************************************/
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(FXAAEffect);
template<class TransferFunction> inline
void FXAAEffect::Transfer(TransferFunction& transfer, void* ptr)
{
	base::Transfer(transfer, ptr);
}
DC_END_NAMESPACE