#include "BloomEffect.h"
#include "runtime/input/Input.h"
#include "runtime/graphics/Material.h"
#include "runtime/graphics/Pass.h"
#include "runtime/graphics/null/RenderTexture.h"
#include "editor/common/ECommonComponent.h"
#include "editor/EditorAppliction.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(BloomEffect, PostEffect);
BloomEffect::BloomEffect()
{
	_material = Material::Create("internal/material/PostProcess/Bloom.material");
	_material->Retain();
}
BloomEffect::~BloomEffect()
{
	SAFE_RELEASE(_material);
}
Object* BloomEffect::Clone(Object* newObj)
{
	base::Clone(newObj);
	BloomEffect* obj = dynamic_cast<BloomEffect*>(newObj);
	if (!obj)return newObj;

	obj->_colorThreshold = _colorThreshold;
	obj->_bloomColor = _bloomColor;
	obj->_bloomFactor = _bloomFactor;
	obj->_downSample = _downSample;

	return obj;
}
void BloomEffect::OnRenderImage(RenderTexture* source, RenderTexture* dest)
{
	if (_material != nullptr)
	{
		TextureDesc desc;
		desc.width = source->GetWidth() >> _downSample; desc.height = source->GetHeight() >> _downSample; desc.format = source->GetFormat();
		desc.flags = TextureFlag::COLOR;
		RenderTexture* rt1 = RenderTexture::Alloc(desc);
		RenderTexture* rt2 = RenderTexture::Alloc(desc);
		Blit(source, rt1, _material);
		//使用pass1提取高亮部分
		_material->SetColor("_ColorThreshold", _colorThreshold);
		Blit(rt1, rt2, _material, 0);
		//使用pass2高斯模糊，两次模糊，横向纵向，使用pass1进行高斯模糊
		_material->SetVector("_BlurOffsets", Vector4(0, 1, 0, 0));
		Blit(rt2, rt1, _material, 1);
		_material->SetVector("_BlurOffsets", Vector4(1, 0, 0, 0));
		Blit(rt1, rt2, _material, 1);
		//使用pass3进行效果计算，清晰场景图直接从source输入到shader的_MainTex中
		_material->SetColor("_BloomColor", _bloomColor);
		_material->SetFloat("_BloomFactor", _bloomFactor);
		_material->SetTexture("_BlurTex", rt2, 2, 1);
		Blit(source, rt1, _material, 2);

		Blit(rt1, dest);
		RenderTexture::Free(rt1);
		RenderTexture::Free(rt2);
	}
	else
	{
		Blit(source, dest);
	}
}
void BloomEffect::OnDrawEditor()
{
	base::OnDrawEditor();

	ImGuiEx::Label("Intensity");
	ImGui::DragFloat("##Intensity", &_bloomFactor, 0.01f, 0.0f, MAX_float);

	ImGuiEx::Label("Threshold");
	ECommonComponent::ShowColor("##Threshold", _colorThreshold.ptr(), false);

	ImGuiEx::Label("Color");
	ECommonComponent::ShowColor("##Color", _bloomColor.ptr(), true);

	ImGuiEx::Label("Down Sample");
	ImGui::SliderInt("##DownSample", &_downSample, 0, 10);
}
/********************************************************************/
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(BloomEffect);
template<class TransferFunction> inline
void BloomEffect::Transfer(TransferFunction& transfer, void* ptr)
{
	base::Transfer(transfer, ptr);

	TRANSFER_SIMPLE(_colorThreshold);
	TRANSFER_SIMPLE(_bloomColor);
	TRANSFER_SIMPLE(_bloomFactor);
	TRANSFER_SIMPLE(_downSample);
}
DC_END_NAMESPACE
