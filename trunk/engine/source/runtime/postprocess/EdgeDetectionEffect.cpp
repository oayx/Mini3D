#include "EdgeDetectionEffect.h"
#include "runtime/input/Input.h"
#include "runtime/graphics/Material.h"
#include "runtime/graphics/Pass.h"
#include "runtime/graphics/null/RenderTexture.h"
#include "editor/common/ECommonComponent.h"
#include "editor/EditorAppliction.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(EdgeDetectionEffect, PostEffect);
EdgeDetectionEffect::EdgeDetectionEffect()
{
	_material = Material::Create("internal/material/PostProcess/EdgeDetection.material");
	_material->Retain();
}
EdgeDetectionEffect::~EdgeDetectionEffect()
{
	SAFE_RELEASE(_material);
}
Object* EdgeDetectionEffect::Clone(Object* new_obj)
{
	base::Clone(new_obj);
	EdgeDetectionEffect* obj = dynamic_cast<EdgeDetectionEffect*>(new_obj);
	if (!obj)return new_obj;

	obj->_edgeColor = _edgeColor;

	return obj;
}
void EdgeDetectionEffect::OnRenderImage(RenderTexture* source, RenderTexture* dest)
{
	if (_material != nullptr)
	{
		TextureDesc desc;
		desc.width = source->GetWidth(); desc.height = source->GetHeight(); desc.format = source->GetFormat();
		desc.flags = TextureFlag::COLOR;
		RenderTexture* tmp_texture = RenderTexture::Alloc(desc);
		_material->SetColor("_EdgeColor", _edgeColor);
		Blit(source, tmp_texture, _material);
		Blit(tmp_texture, dest);
		RenderTexture::Free(tmp_texture);
	}
	else
	{
		Blit(source, dest);
	}
}
void EdgeDetectionEffect::OnDrawEditor()
{
	base::OnDrawEditor();
	ImGuiEx::Label("EdgeColor");
	ECommonComponent::ShowColor("##EdgeColor", _edgeColor.ptr(), true);
}
/********************************************************************/
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(EdgeDetectionEffect);
template<class TransferFunction> inline
void EdgeDetectionEffect::Transfer(TransferFunction& transfer, void* ptr)
{
	base::Transfer(transfer, ptr);

	TRANSFER_SIMPLE(_edgeColor);
}
DC_END_NAMESPACE