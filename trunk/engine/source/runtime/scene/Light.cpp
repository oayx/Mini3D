#include "Light.h"
#include "runtime/graphics/ShadowMap.h"
#include "runtime/graphics/Material.h"
#include "runtime/project/QualitySettings.h"
#include "runtime/Application.h"
#include "editor/EditorDefine.h"
#include "editor/common/ECommonComponent.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(Light, Component);
Light::Light() 
{ 
	SceneManager::AddLight(this);
}
Light::~Light() 
{
	SceneManager::RemoveLight(this);
	SAFE_DELETE(_shadowMap);
}
Object* Light::Clone(Object* new_obj)
{
	base::Clone(new_obj);
	Light* obj = dynamic_cast<Light*>(new_obj);
	if (!obj)return new_obj;

	obj->mType = mType;
	obj->Diffuse = Diffuse;
	obj->Range = Range;
	obj->Attenuation = Attenuation;
	obj->Intensity = Intensity;
	obj->SpotAngle = SpotAngle;
	obj->CullMask = CullMask;

	obj->_shadowType = _shadowType;
	obj->_shadowStrength = _shadowStrength;
	obj->_shadowBias = _shadowBias;
	if (_shadowType != ShadowType::None)
		obj->EnableShadowMap();

	return obj;
}
void Light::EnableShadowMap(ShadowType type, int w, int h, ColorFormat format)
{
	SAFE_DELETE(_shadowMap);

	_shadowType = type;
	_shadowMap = ShadowMap::Create(w, h, format);
}
void Light::EnableShadowMap()
{
	if (_shadowType == ShadowType::None)
	{
		SAFE_DELETE(_shadowMap);
	}
	else
	{
		int w = 0, h = 0;
		ColorFormat format = ColorFormat::DEPTH24_STENCIL8;
		switch (QualitySettings::GetShadowResolution())
		{
		case ShadowResolution::Low: w = 512; h = 512; format = ColorFormat::DEPTH16; break;
		case ShadowResolution::Medium: w = 1024; h = 1024; break;
		case ShadowResolution::High: w = 2048; h = 2048; break;
		case ShadowResolution::VeryHigh:w = 4096; h = 4096; break;
		default: w = 1024; h = 1024; break;
		}
		EnableShadowMap(_shadowType, w, h, format);
	}
}
void Light::DisableShadowMap()
{
	SAFE_DELETE(_shadowMap);
}
ShadowType Light::GetValidShadowType()const
{
	ShadowType type = _shadowType;
	if (type > QualitySettings::GetShadowType())type = QualitySettings::GetShadowType();
	return type;
}
void Light::OnDrawEditor()
{
	base::OnDrawEditor();
	{
		const char* sz_flags[] = { "Directional", "Point", "Spot" };
		ImGuiEx::Label("Type");
		static int current_index = 0;
		switch (mType)
		{
		case LightType::Direction: current_index = 0; break;
		case LightType::Point:current_index = 1; break;
		case LightType::Spot:current_index = 2; break;
		}
		if (ImGui::Combo("##Type", &current_index, sz_flags, ARRAY_SIZE(sz_flags)))
		{
			switch (current_index)
			{
			case 0: mType = LightType::Direction; break;
			case 1: mType = LightType::Point; break;
			case 2: mType = LightType::Spot; break;
			}
		}

		ImGuiEx::Label("Color");
		ECommonComponent::ShowColor("Color", Diffuse.p, false);

		if (mType == LightType::Point || mType == LightType::Spot)
		{
			float min_value = 0.0f;
			ImGuiEx::Label("Range");
			ImGui::DragScalar("##Range", ImGuiDataType_Float, &Range, 0.05f, &min_value, nullptr, "%.2f");
		}

		if (mType == LightType::Spot)
		{
			ImGuiEx::Label("Spot Angle");
			ImGui::SliderFloat("SpotAngle", &SpotAngle, 1, 179);
		}

		float min_value = 0.0f;
		ImGuiEx::Label("Intensity");
		ImGui::DragScalar("##Intensity", ImGuiDataType_Float, &Intensity, 0.05f, &min_value, nullptr, "%.2f");
	}
	{
		ImGui::NewLine();
		const char* sz_flags[] = { "No Shadows", "Hard Shadows", "Soft Shadows" };
		ImGuiEx::Label("Shadow Type");
		static int current_index = (int)_shadowType;
		if (ImGui::Combo("##Shadow Type", &current_index, sz_flags, ARRAY_SIZE(sz_flags)))
		{
			_shadowType = (ShadowType)current_index;
			EnableShadowMap();
		}

		if (current_index != 0)
		{
			{
				ImGuiEx::Label("        Strength");
				float min_value = 0.0f;
				float max_value = 1.0f;
				ImGui::DragScalar("##Strength", ImGuiDataType_Float, &_shadowStrength, 0.01f, &min_value, &max_value, "%.2f");
			}
			{
				ImGuiEx::Label("        Bias");
				float min_value = -2.0f;
				float max_value = 2.0f;
				ImGui::DragScalar("##Bias", ImGuiDataType_Float, &_shadowBias, 0.001f, &min_value, &max_value, "%.3f");
			}
		}
	}
	{//Color Mask
		ImGui::NewLine();

		ImGuiEx::Label("Color Mask");
		ECommonComponent::ShowCullmask("Color Mask", CullMask);
	}
}
/********************************************************************/
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(Light);
template<class TransferFunction> inline
void Light::Transfer(TransferFunction& transfer, void* ptr)
{
	base::Transfer(transfer, ptr);

	TRANSFER_ENUM(mType);
	TRANSFER_SIMPLE(Diffuse);
	TRANSFER_SIMPLE(Range);
	TRANSFER_SIMPLE(Attenuation);
	TRANSFER_SIMPLE(Intensity);
	TRANSFER_SIMPLE(SpotAngle);
	TRANSFER_SIMPLE(CullMask);

	TRANSFER_ENUM(_shadowType);
	TRANSFER_SIMPLE(_shadowStrength);
	TRANSFER_SIMPLE(_shadowBias);

	if (transfer.IsRead() && GetValidShadowType() != ShadowType::None)
	{
		EnableShadowMap();
	}
}
DC_END_NAMESPACE