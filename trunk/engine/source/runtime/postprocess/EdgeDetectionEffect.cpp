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
Object* EdgeDetectionEffect::Clone(Object* newObj)
{
	base::Clone(newObj);
	EdgeDetectionEffect* obj = dynamic_cast<EdgeDetectionEffect*>(newObj);
	if (!obj)return newObj;

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
		RenderTexture* tmpTexture = RenderTexture::Alloc(desc);
		_material->SetColor("_EdgeColor", _edgeColor);
		Blit(source, tmpTexture, _material);
		Blit(tmpTexture, dest);
		RenderTexture::Free(tmpTexture);
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