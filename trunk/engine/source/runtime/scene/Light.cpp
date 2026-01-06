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
Object* Light::Clone(Object* newObj)
{
	base::Clone(newObj);
	Light* obj = dynamic_cast<Light*>(newObj);
	if (!obj)return newObj;

	obj->mType = mType;
	obj->Diffuse = Diffuse;
	obj->Range = Range;
	obj->Attenuation = Attenuation;
	obj->Intensity = Intensity;
	obj->SpotAngle = SpotAngle;

	obj->_shadowType = _shadowType;
	obj->_shadowStrength = _shadowStrength;
	obj->_shadowBias = _shadowBias;
	obj->_cullMask = _cullMask;

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
		const char* szFlags[] = { "Directional", "Point", "Spot" };
		ImGuiEx::Label("Type");
		static int currentIndex = 0;
		switch (mType)
		{
		case LightType::Direction: currentIndex = 0; break;
		case LightType::Point:currentIndex = 1; break;
		case LightType::Spot:currentIndex = 2; break;
		}
		if (ImGui::Combo("##Type", &currentIndex, szFlags, ARRAY_SIZE(szFlags)))
		{
			switch (currentIndex)
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
			float minValue = 0.0f;
			ImGuiEx::Label("Range");
			ImGui::DragScalar("##Range", ImGuiDataType_Float, &Range, 0.05f, &minValue, nullptr, "%.2f");
		}

		if (mType == LightType::Spot)
		{
			ImGuiEx::Label("Spot Angle");
			ImGui::SliderFloat("SpotAngle", &SpotAngle, 1, 179);
		}

		float minValue = 0.0f;
		ImGuiEx::Label("Intensity");
		ImGui::DragScalar("##Intensity", ImGuiDataType_Float, &Intensity, 0.05f, &minValue, nullptr, "%.2f");
	}
	{
		ImGui::NewLine();
		const char* szFlags[] = { "No Shadows", "Hard Shadows", "Soft Shadows" };
		ImGuiEx::Label("Shadow Type");
		static int currentIndex = (int)_shadowType;
		if (ImGui::Combo("##Shadow Type", &currentIndex, szFlags, ARRAY_SIZE(szFlags)))
		{
			_shadowType = (ShadowType)currentIndex;
			EnableShadowMap();
		}

		if (currentIndex != 0)
		{
			{
				ImGuiEx::Label("        Strength");
				float minValue = 0.0f;
				float maxValue = 1.0f;
				ImGui::DragScalar("##Strength", ImGuiDataType_Float, &_shadowStrength, 0.01f, &minValue, &maxValue, "%.2f");
			}
			{
				ImGuiEx::Label("        Bias");
				float minValue = -2.0f;
				float maxValue = 2.0f;
				ImGui::DragScalar("##Bias", ImGuiDataType_Float, &_shadowBias, 0.001f, &minValue, &maxValue, "%.3f");
			}
		}
	}
	{//Color Mask
		ImGui::NewLine();

		ImGuiEx::Label("Color Mask");
		ECommonComponent::ShowCullmask("Color Mask", _cullMask);
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

	TRANSFER_ENUM(_shadowType);
	TRANSFER_SIMPLE(_shadowStrength);
	TRANSFER_SIMPLE(_shadowBias);
	TRANSFER_SIMPLE(_cullMask);

	if (transfer.IsRead() && GetValidShadowType() != ShadowType::None)
	{
		EnableShadowMap();
	}
}
DC_END_NAMESPACE