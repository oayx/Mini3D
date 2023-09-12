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
		const char* sz_names[] = { "Disabled", "2x Multi Sampling", "4x Multi Sampling", "8x Multi Sampling" };
		int current_select = 0;
		switch (QualitySettings::GetMSAAType())
		{
		case MSAAType::Disabled:current_select = 0; break;
		case MSAAType::Level_2:current_select = 1; break;
		case MSAAType::Level_4:current_select = 2; break;
		case MSAAType::Level_8:current_select = 3; break;
		}
		if (ImGui::Combo("##Anti Aliasing", &current_select, sz_names, ARRAY_SIZE(sz_names)))
		{
			switch (current_select)
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
			const char* sz_flags[] = { "Disable Shadows", "Hard Shadows Only", "Hard and Soft Shadows" };
			ImGuiEx::Label("Shadows");
			static int current_index = (int)QualitySettings::GetShadowType();
			if (ImGui::Combo("##Shadow Type", &current_index, sz_flags, ARRAY_SIZE(sz_flags)))
			{
				QualitySettings::SetShadowType((ShadowType)current_index);
			}
		}
		{
			const char* sz_flags[] = { "Low Resolution", "Medium Resolution", "High Resolution", "Very High Resolution" };
			ImGuiEx::Label("Shadow Resolution");
			static int current_index = (int)QualitySettings::GetShadowResolution();
			if (ImGui::Combo("##Shadow Resolution", &current_index, sz_flags, ARRAY_SIZE(sz_flags)))
			{
				QualitySettings::SetShadowResolution((ShadowResolution)current_index);
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
			const char* sz_flags[] = { "FP16" };
			static int current_index = 0;
			switch (QualitySettings::GetHDRFormat())
			{
			case ColorFormat::RGBA16F:current_index = 0; break;
			default: break;
			}
			if (ImGui::Combo("##Format", &current_index, sz_flags, ARRAY_SIZE(sz_flags)))
			{
				switch (current_index)
				{
				case 0:QualitySettings::SetHDRFormat(ColorFormat::RGBA16F); break;
				default: break;
				}
			}
		}
	}
}
DC_END_NAMESPACE