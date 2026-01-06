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
void ELightSetting::Render()
{
	DC_PROFILE_FUNCTION;
	if (!IsShow)return;
	ImGui::SetNextWindowSizeConstraints(ImVec2(400.0f, 600.0f), ImVec2(FLT_MAX, FLT_MAX));
	if (ImGui::Begin(ICON_FA_EDIT " Lighting", &IsShow, ImGuiWindowFlags_NoCollapse))
	{
		Scene* currScene = SceneManager::GetCurrScene();
		if (!currScene)
		{
			ImGui::End();
			return;
		}

		ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;
		if (ImGui::TreeNodeEx("Environment", baseFlags))
		{
			{
				ImGuiEx::Label("Skybox Material");
				const char* szName = currScene->GetSkyboxMaterial().c_str();
				ImGui::Button(szName, ImVec2(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(ICON_FA_LINK).x - 5, 0));
				if (ImGui::IsItemClicked(0))
				{
					EMain_Project::SetSelectFile(currScene->GetSkyboxMaterial());
				}
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ProjectAsset"))
					{
						const String& filePath = EditorCursor::GetDragFile();
						if (Resource::GetResourceType(filePath) == ResourceType::Material)
						{
							currScene->SetSkyboxMaterial(filePath);
						}
						EditorCursor::EndDrag();
					}
					ImGui::EndDragDropTarget();
				}
				ImGui::SameLine();
				if (ImGui::Selectable(ICON_FA_LINK, false, 0, ImGui::CalcTextSize(ICON_FA_LINK)))
				{
					auto OnSelectAsset = [currScene](const String& file)
					{
						currScene->SetSkyboxMaterial(file);
					};

					EAssetSelect::Search(ResourceType::Material, OnSelectAsset);
				}
			}

			{
				ImGui::NewLine();
				ImGui::TextUnformatted("Environment Lighting");
				{
					ImGuiEx::Label("    Source");
					const char* szNames[] = { "Color", "Skybox" };
					int currentIndex = 0;
					if (ImGui::Combo("##Source", &currentIndex, szNames, ARRAY_SIZE(szNames)))
					{
					}
				}
				{
					ImGuiEx::Label("    Ambient Color");
					Color color = currScene->GetAmbientColor();
					if (ECommonComponent::ShowColor("##Ambient Color", color.ptr(), false))
					{
						currScene->SetAmbientColor(color);
					}
				}
			}

			{
				ImGui::NewLine();
				ImGui::TextUnformatted("Environment Reflections");
				{
					ImGuiEx::Label("    Source");
					const char* szNames[] = { "Skybox" };
					int currentIndex = 0;
					if (ImGui::Combo("##Source", &currentIndex, szNames, ARRAY_SIZE(szNames)))
					{
					}
				}
				{
					ImGuiEx::Label("Intensity");
					float intensity = currScene->GetReflectionsIntensity();
					if (ImGui::DragFloat("##Exposure", &intensity, 0.001f, 0.0f, 1.0f))
					{
						currScene->SetReflectionsIntensity(intensity);
					}
				}
			}
			ImGui::TreePop();
		}
		if (ImGui::TreeNodeEx("Other Settings", baseFlags))
		{
			const FogDesc& fogDesc = currScene->GetFog();
			ImGuiEx::Label("Fog");
			bool enable = fogDesc.enable;
			if (ImGui::Checkbox("##Fog Enable", &enable))
			{
				currScene->SetFogEnable(enable);
			}
			
			if (fogDesc.enable)
			{
				ImGuiEx::Label("    Color");
				Color color = fogDesc.color;
				if (ECommonComponent::ShowColor("##Fog Color", color.ptr(), false))
				{
					currScene->SetFogColor(color);
				}
				
				ImGuiEx::Label("    Mode");
				const char* szNames[] = { "Linear", "Exp", "Exp2" };
				int currentIndex = (int)fogDesc.mode;
				if (ImGui::Combo("##Fog Mode", &currentIndex, szNames, ARRAY_SIZE(szNames)))
				{
					currScene->SetFogMode((FogMode)currentIndex);
				}

				if (currentIndex == 0)
				{//线性雾
					ImGuiEx::Label("    Start");
					float start = fogDesc.start;
					if (ImGui::DragFloat("##Start", &start, 0.1f, 0.0f))
					{
						currScene->SetFogRange(start, fogDesc.end);
					}

					ImGuiEx::Label("    End");
					float end = fogDesc.end;
					if (ImGui::DragFloat("##End", &end, 0.1f, start))
					{
						currScene->SetFogRange(fogDesc.start, end);
					}
				}
				else
				{//指数雾
					ImGuiEx::Label("    Density");
					float density = fogDesc.density;
					if (ImGui::DragFloat("##Fog Density", &density, 0.01f, 0.0f, 1.0f))
					{
						currScene->SetFogDensity(density);
					}
				}
			}

			ImGui::TreePop();
		}
	}
	ImGui::End();
}
DC_END_NAMESPACE