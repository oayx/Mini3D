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
Object* MotionBlurEffect::Clone(Object* new_obj)
{
	base::Clone(new_obj);
	MotionBlurEffect* obj = dynamic_cast<MotionBlurEffect*>(new_obj);
	if (!obj)return new_obj;

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
		RenderTexture* tmp_texture = RenderTexture::Alloc(desc);
		_material->SetMatrix("_PRE_MATRIX_VP", m_matViewProj);
		_material->SetVector("_Intensity", _intensity);
		Blit(source, tmp_texture, _material);
		Blit(tmp_texture, dest);
		RenderTexture::Free(tmp_texture);

		m_matViewProj = _camera->GetViewMatrix() * _camera->GetProjMatrix();
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
		const float char_width = ImGui::GetFontSize();

		ImGui::SetNextItemWidth(char_width);
		ImGui::Text("X");

		ImGui::SameLine();
		ImGui::SetNextItemWidth(width * 0.5f - char_width);
		if (ImGui::DragScalar("##IntensityX", ImGuiDataType_Float, &_intensity.x, 0.01f))
		{
		}

		ImGui::SameLine();
		ImGui::SetNextItemWidth(char_width);
		ImGui::TextUnformatted("Y");

		ImGui::SameLine();
		ImGui::SetNextItemWidth(width * 0.5f - char_width);
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