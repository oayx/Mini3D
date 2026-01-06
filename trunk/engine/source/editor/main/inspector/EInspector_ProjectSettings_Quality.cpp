#include "EInspector_ProjectSettings_Quality.h"
#include "runtime/project/QualitySettings.h"
#include "editor/EditorAppliction.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
void EInspector_ProjectSettings_Quality::Render()
{
	ImGui::TextUnformatted("Rendering");
	{
		ImGuiEx::Label("Anti Aliasing");
		const char* szNames[] = { "Disabled", "2x Multi Sampling", "4x Multi Sampling", "8x Multi Sampling" };
		int currentSelect = 0;
		switch (QualitySettings::GetMSAAType())
		{
		case MSAAType::Disabled:currentSelect = 0; break;
		case MSAAType::Level_2:currentSelect = 1; break;
		case MSAAType::Level_4:currentSelect = 2; break;
		case MSAAType::Level_8:currentSelect = 3; break;
		}
		if (ImGui::Combo("##Anti Aliasing", &currentSelect, szNames, ARRAY_SIZE(szNames)))
		{
			switch (currentSelect)
			{
			case 0: QualitySettings::SetMSAAType(MSAAType::Disabled); break;
			case 1: QualitySettings::SetMSAAType(MSAAType::Level_2); break;
			case 2: QualitySettings::SetMSAAType(MSAAType::Level_4); break;
			case 3: QualitySettings::SetMSAAType(MSAAType::Level_8); break;
			}
		}
	}

	ImGui::NewLine();
	ImGui::TextUnformatted("Shadows");
	{
		{
			const char* szFlags[] = { "Disable Shadows", "Hard Shadows Only", "Hard and Soft Shadows" };
			ImGuiEx::Label("Shadows");
			static int currentIndex = (int)QualitySettings::GetShadowType();
			if (ImGui::Combo("##Shadow Type", &currentIndex, szFlags, ARRAY_SIZE(szFlags)))
			{
				QualitySettings::SetShadowType((ShadowType)currentIndex);
			}
		}
		{
			const char* szFlags[] = { "Low Resolution", "Medium Resolution", "High Resolution", "Very High Resolution" };
			ImGuiEx::Label("Shadow Resolution");
			static int currentIndex = (int)QualitySettings::GetShadowResolution();
			if (ImGui::Combo("##Shadow Resolution", &currentIndex, szFlags, ARRAY_SIZE(szFlags)))
			{
				QualitySettings::SetShadowResolution((ShadowResolution)currentIndex);
			}
		}
		{
			ImGuiEx::Label("Shadow Distance");
			float distance = QualitySettings::GetShadowDistance();
			if (ImGui::InputScalar("##Shadow Distance", ImGuiDataType_Float, &distance, nullptr))
			{
				distance = Math::Clamp(distance, 0.0f, 1000.0f);
				QualitySettings::SetShadowDistance(distance);
			}
		}
	}

	ImGui::NewLine();
	ImGui::TextUnformatted("HDR");
	{
		{
			ImGuiEx::Label("Enable");
			bool enable = QualitySettings::IsEnableHDR();
			if (ImGui::Checkbox("##EnableHDR", &enable))
			{
				QualitySettings::SetEnableHDR(enable);
			}
		}
		{
			ImGuiEx::Label("Exposure");
			float exposure = QualitySettings::GetHDRExposure();
			if (ImGui::DragFloat("##Exposure", &exposure, 0.01f, 0.0f, 10.0f))
			{
				QualitySettings::SetHDRExposure(exposure);
			}
		}
		{
			ImGuiEx::Label("Format");
			const char* szFlags[] = { "FP16" };
			static int currentIndex = 0;
			switch (QualitySettings::GetHDRFormat())
			{
			case ColorFormat::RGBA16F:currentIndex = 0; break;
			default: break;
			}
			if (ImGui::Combo("##Format", &currentIndex, szFlags, ARRAY_SIZE(szFlags)))
			{
				switch (currentIndex)
				{
				case 0:QualitySettings::SetHDRFormat(ColorFormat::RGBA16F); break;
				default: break;
				}
			}
		}
	}
}
DC_END_NAMESPACE