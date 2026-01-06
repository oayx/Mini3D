#include "MotionBlurEffect.h"
#include "runtime/input/Input.h"
#include "runtime/graphics/Material.h"
#include "runtime/graphics/Pass.h"
#include "runtime/graphics/null/RenderTexture.h"
#include "runtime/scene/Camera.h"
#include "editor/common/ECommonComponent.h"
#include "editor/EditorAppliction.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(MotionBlurEffect, PostEffect);
MotionBlurEffect::MotionBlurEffect()
{
	_material = Material::Create("internal/material/PostProcess/MotionBlur.material");
	_material->Retain();
}
MotionBlurEffect::~MotionBlurEffect()
{
	SAFE_RELEASE(_material);
}
Object* MotionBlurEffect::Clone(Object* newObj)
{
	base::Clone(newObj);
	MotionBlurEffect* obj = dynamic_cast<MotionBlurEffect*>(newObj);
	if (!obj)return newObj;

	obj->_intensity = _intensity;

	return obj;
}
void MotionBlurEffect::OnRenderImage(RenderTexture* source, RenderTexture* dest)
{
	if (_material != nullptr)
	{
		TextureDesc desc;
		desc.width = source->GetWidth(); desc.height = source->GetHeight(); desc.format = source->GetFormat();
		desc.flags = TextureFlag::COLOR;
		RenderTexture* tmpTexture = RenderTexture::Alloc(desc);
		_material->SetMatrix("_PRE_MATRIX_VP", _matViewProj);
		_material->SetVector("_Intensity", _intensity);
		Blit(source, tmpTexture, _material);
		Blit(tmpTexture, dest);
		RenderTexture::Free(tmpTexture);

		_matViewProj = _camera->GetViewMatrix() * _camera->GetProjMatrix();
	}
	else
	{
		Blit(source, dest);
	}
}
void MotionBlurEffect::OnDrawEditor()
{
	base::OnDrawEditor();
	{
		ImGuiEx::Label("Intensity");
		const float width = ImGui::GetContentRegionAvail().x;
		const float charWidth = ImGui::GetFontSize();

		ImGui::SetNextItemWidth(charWidth);
		ImGui::Text("X");

		ImGui::SameLine();
		ImGui::SetNextItemWidth(width * 0.5f - charWidth);
		if (ImGui::DragScalar("##IntensityX", ImGuiDataType_Float, &_intensity.x, 0.01f))
		{
		}

		ImGui::SameLine();
		ImGui::SetNextItemWidth(charWidth);
		ImGui::TextUnformatted("Y");

		ImGui::SameLine();
		ImGui::SetNextItemWidth(width * 0.5f - charWidth);
		if (ImGui::DragScalar("##IntensityY", ImGuiDataType_Float, &_intensity.y, 0.01f))
		{
		}
	}
}
/********************************************************************/
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(MotionBlurEffect);
template<class TransferFunction> inline
void MotionBlurEffect::Transfer(TransferFunction& transfer, void* ptr)
{
	base::Transfer(transfer, ptr);
	TRANSFER_SIMPLE(_intensity);
}
DC_END_NAMESPACE