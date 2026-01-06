#include "ColorAdjustEffect.h"
#include "runtime/input/Input.h"
#include "runtime/graphics/Material.h"
#include "runtime/graphics/Pass.h"
#include "editor/EditorAppliction.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(ColorAdjustEffect, PostEffect);
ColorAdjustEffect::ColorAdjustEffect()
{
	_material = Material::Create("internal/material/PostProcess/ColorAdjust.material");
	_material->Retain();
}
ColorAdjustEffect::~ColorAdjustEffect()
{
	SAFE_RELEASE(_material);
}
Object* ColorAdjustEffect::Clone(Object* newObj)
{
	base::Clone(newObj);
	ColorAdjustEffect* obj = dynamic_cast<ColorAdjustEffect*>(newObj);
	if (!obj)return newObj;

	obj->_brightness = _brightness;
	obj->_saturation = _saturation;
	obj->_contrast = _contrast;

	return obj;
}
void ColorAdjustEffect::OnRenderImage(RenderTexture* source, RenderTexture* dest)
{
	if (_material != nullptr)
	{
		TextureDesc desc;
		desc.width = source->GetWidth(); desc.height = source->GetHeight(); desc.format = source->GetFormat();
		desc.flags = TextureFlag::COLOR;
		RenderTexture* tmpTexture = RenderTexture::Alloc(desc);
		_material->SetFloat("_Brightness", _brightness);
		_material->SetFloat("_Saturation", _saturation);
		_material->SetFloat("_Contrast", _contrast);
		Blit(source, tmpTexture, _material);
		Blit(tmpTexture, dest);
		RenderTexture::Free(tmpTexture);
	}
	else
	{
		Blit(source, dest);
	}
} 
void ColorAdjustEffect::OnDrawEditor()
{
	base::OnDrawEditor();
	ImGuiEx::Label("Brightness");
	ImGui::DragFloat("##Brightness", &_brightness, 0.01f, 0.0f, 10.0f);

	ImGuiEx::Label("Saturation");
	ImGui::DragFloat("##Saturation", &_saturation, 0.01f, 0.0f, 5.0f);

	ImGuiEx::Label("Contrast");
	ImGui::DragFloat("##Contrast", &_contrast, 0.01f, 0.0f, 5.0f);
}
/********************************************************************/
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(ColorAdjustEffect);
template<class TransferFunction> inline
void ColorAdjustEffect::Transfer(TransferFunction& transfer, void* ptr)
{
	base::Transfer(transfer, ptr);

	TRANSFER_SIMPLE(_brightness);
	TRANSFER_SIMPLE(_saturation);
	TRANSFER_SIMPLE(_contrast);
}
DC_END_NAMESPACE