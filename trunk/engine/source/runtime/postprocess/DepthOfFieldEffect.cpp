#include "DepthOfFieldEffect.h"
#include "runtime/input/Input.h"
#include "runtime/graphics/Material.h"
#include "runtime/graphics/Pass.h"
#include "runtime/graphics/null/RenderTexture.h"
#include "runtime/scene/Camera.h"
#include "editor/common/ECommonComponent.h"
#include "editor/EditorAppliction.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(DepthOfFieldEffect, PostEffect);
DepthOfFieldEffect::DepthOfFieldEffect()
{
	_material = Material::Create("internal/material/PostProcess/DepthOfField.material");
	_material->Retain();
}
DepthOfFieldEffect::~DepthOfFieldEffect()
{
	SAFE_RELEASE(_material);
}
Object* DepthOfFieldEffect::Clone(Object* new_obj)
{
	base::Clone(new_obj);
	DepthOfFieldEffect* obj = dynamic_cast<DepthOfFieldEffect*>(new_obj);
	if (!obj)return new_obj;

	obj->_downSample = _downSample;

	obj->_focalDistance = _focalDistance;
	obj->_focalLength = _focalLength;
	obj->_nearBlurScale = _nearBlurScale;
	obj->_farBlurScale = _farBlurScale;

	return obj;
}
void DepthOfFieldEffect::OnRenderImage(RenderTexture* source, RenderTexture* dest)
{
	if (_material != nullptr)
	{
		TextureDesc desc;
		desc.width = source->GetWidth() >> _downSample; desc.height = source->GetHeight() >> _downSample; desc.format = source->GetFormat();
		desc.flags = TextureFlag::COLOR;
		RenderTexture* rt1 = RenderTexture::Alloc(desc);
		RenderTexture* rt2 = RenderTexture::Alloc(desc);
		Blit(source, rt1, _material);

		//使用pass1高斯模糊，两次模糊，横向纵向
		_material->SetVector("_BlurOffsets", Vector4(0, 1, 0, 0));
		Blit(rt1, rt2, _material, 0);
		_material->SetVector("_BlurOffsets", Vector4(1, 0, 0, 0));
		Blit(rt2, rt1, _material, 0);
		//使用pass2进行DOF
		_material->SetFloat("_FocalDistance", FocalDistance01(_focalDistance));
		_material->SetFloat("_FocalLength", _focalLength / (_camera->GetZFar() - _camera->GetZNear()));
		_material->SetFloat("_NearBlurScale", _nearBlurScale);
		_material->SetFloat("_FarBlurScale", _farBlurScale);
		_material->SetTexture("_BlurTex", rt1, 1, 1);
		Blit(source, rt2, _material, 1);

		Blit(rt2, dest);
		RenderTexture::Free(rt1);
		RenderTexture::Free(rt2);
	}
	else
	{
		Blit(source, dest);
	}
}
//计算设置的焦点被转换到01空间中的距离，以便shader中通过这个01空间的焦点距离与depth比较
float DepthOfFieldEffect::FocalDistance01(float distance)
{
	Transform* transform = _camera->GetTransform();
	Vector3 view_point = _camera->WorldToViewportPoint((distance - _camera->GetZNear()) * transform->GetForward() + transform->GetPosition());
	return view_point.z / (_camera->GetZFar() - _camera->GetZNear());
}
void DepthOfFieldEffect::OnDrawEditor()
{
	base::OnDrawEditor();

	ImGuiEx::Label("Down Sample");
	ImGui::SliderInt("##DownSample", &_downSample, 0, 10);
	
	ImGuiEx::Label("Focal Distance");
	if (_camera)
		ImGui::SliderFloat("##FocalDistance", &_focalDistance, _camera->GetZNear(), _camera->GetZFar());
	else
		ImGui::SliderFloat("##FocalDistance", &_focalDistance, 0.0f, 1000.0f);

	ImGuiEx::Label("Focal Length");
	ImGui::SliderFloat("##FocalLength", &_focalLength, 0.0f, 100.0f);

	ImGuiEx::Label("Near Blur Scale");
	ImGui::SliderFloat("##NearBlurScale", &_nearBlurScale, 0.0f, 100.0f);

	ImGuiEx::Label("Far Blur Scale");
	ImGui::SliderFloat("##FarBlurScale", &_farBlurScale, 0.0f, 100.0f);
}
/********************************************************************/
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(DepthOfFieldEffect);
template<class TransferFunction> inline
void DepthOfFieldEffect::Transfer(TransferFunction& transfer, void* ptr)
{
	base::Transfer(transfer, ptr);

	TRANSFER_SIMPLE(_downSample);
	TRANSFER_SIMPLE(_focalDistance);
	TRANSFER_SIMPLE(_focalLength);
	TRANSFER_SIMPLE(_nearBlurScale);
	TRANSFER_SIMPLE(_farBlurScale);
}
DC_END_NAMESPACE
