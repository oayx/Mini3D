#include "ELightSetting.h"
#include "runtime/scene/SceneManager.h"
#include "runtime/resources/AssetsManager.h"
#include "runtime/resources/Resources.h"
#include "editor/EditorDefine.h"
#include "editor/common/EAssetSelect.h"
#include "editor/main/EMain_Project.h"
#include "editor/common/ECommonComponent.h"
#include "editor/EditorCursor.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
bool ELightSetting::IsShow = false;
void ELightSetting::Render()
{
	DC_PROFILE_FUNCTION();
	if (!IsShow)return;
	ImGui::SetNextWindowSizeConstraints(ImVec2(400.0f, 600.0f), ImVec2(FLT_MAX, FLT_MAX));
	if (ImGui::Begin(ICON_FA_EDIT " Lighting", &IsShow, ImGuiWindowFlags_NoCollapse))
	{
		Scene* curr_scene = SceneManager::GetCurrScene();
		if (!curr_scene)
		{
			ImGui::End();
			return;
		}

		ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;
		if (ImGui::TreeNodeEx("Environment", base_flags))
		{
			{
				ImGuiEx::Label("Skybox Material");
				const char* sz_name = curr_scene->GetSkyboxMaterial().c_str();
				ImGui::Button(sz_name, ImVec2(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(ICON_FA_LINK).x - 5, 0));
				if (ImGui::IsItemClicked(0))
				{
					EMain_Project::SetSelectFile(curr_scene->GetSkyboxMaterial());
				}
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ProjectAsset"))
					{
						const String& file_path = EditorCursor::GetDragFile();
						if (Resource::GetResourceType(file_path) == ResourceType::Material)
						{
							curr_scene->SetSkyboxMaterial(file_path);
						}
						EditorCursor::EndDrag();
					}
					ImGui::EndDragDropTarget();
				}
				ImGui::SameLine();
				if (ImGui::Selectable(ICON_FA_LINK, false, 0, ImGui::CalcTextSize(ICON_FA_LINK)))
				{
					auto OnSelectAsset = [curr_scene](const String& file)
					{
						curr_scene->SetSkyboxMaterial(file);
					};

					EAssetSelect::Search(ResourceType::Material, OnSelectAsset);
				}
			}

			{
				ImGui::NewLine();
				ImGui::TextUnformatted("Environment Lighting");
				{
					ImGuiEx::Label("    Source");
					const char* sz_names[] = { "Color", "Skybox" };
					int current_index = 0;
					if (ImGui::Combo("##Source", &current_index, sz_names, ARRAY_SIZE(sz_names)))
					{
					}
				}
				{
					ImGuiEx::Label("    Ambient Color");
					Color color = curr_scene->GetAmbientColor();
					if (ECommonComponent::ShowColor("##Ambient Color", color.ptr(), false))
					{
						curr_scene->SetAmbientColor(color);
					}
				}
			}

			{
				ImGui::NewLine();
				ImGui::TextUnformatted("Environment Reflections");
				{
					ImGuiEx::Label("    Source");
					const char* sz_names[] = { "Skybox" };
					int current_index = 0;
					if (ImGui::Combo("##Source", &current_index, sz_names, ARRAY_SIZE(sz_names)))
					{
					}
				}
				{
					ImGuiEx::Label("Intensity");
					float intensity = curr_scene->GetReflectionsIntensity();
					if (ImGui::DragFloat("##Exposure", &intensity, 0.001f, 0.0f, 1.0f))
					{
						curr_scene->SetReflectionsIntensity(intensity);
					}
				}
			}
			ImGui::TreePop();
		}
		if (ImGui::TreeNodeEx("Other Settings", base_flags))
		{
			const FogDesc& fog_desc = curr_scene->GetFog();
			ImGuiEx::Label("Fog");
			bool enable = fog_desc.enable;
			if (ImGui::Checkbox("##Fog Enable", &enable))
			{
				curr_scene->SetFogEnable(enable);
			}
			
			if (fog_desc.enable)
			{
				ImGuiEx::Label("    Color");
				Color color = fog_desc.color;
				if (ECommonComponent::ShowColor("##Fog Color", color.ptr(), false))
				{
					curr_scene->SetFogColor(color);
				}
				
				ImGuiEx::Label("    Mode");
				const char* sz_names[] = { "Linear", "Exp", "Exp2" };
				int current_index = (int)fog_desc.mode;
				if (ImGui::Combo("##Fog Mode", &current_index, sz_names, ARRAY_SIZE(sz_names)))
				{
					curr_scene->SetFogMode((FogMode)current_index);
				}

				if (current_index == 0)
				{//线性雾
					ImGuiEx::Label("    Start");
					float start = fog_desc.start;
					if (ImGui::DragFloat("##Start", &start, 0.1f, 0.0f))
					{
						curr_scene->SetFogRange(start, fog_desc.end);
					}

					ImGuiEx::Label("    End");
					float end = fog_desc.end;
					if (ImGui::DragFloat("##End", &end, 0.1f, start))
					{
						curr_scene->SetFogRange(fog_desc.start, end);
					}
				}
				else
				{//指数雾
					ImGuiEx::Label("    Density");
					float density = fog_desc.density;
					if (ImGui::DragFloat("##Fog Density", &density, 0.01f, 0.0f, 1.0f))
					{
						curr_scene->SetFogDensity(density);
					}
				}
			}

			ImGui::TreePop();
		}
	}
	ImGui::End();
}
DC_END_NAMESPACE