#include "EMain_Menu.h"
#include "runtime/mesh/MeshRender.h"
#include "runtime/project/ProjectManager.h"
#include "runtime/ui/GUI.h"
#include "runtime/Application.h"
#include "editor/common/ECommonComponent.h"
#include "editor/file/EOpenProject.h"
#include "editor/help/EIMGUISetter.h"
#include "editor/main/EMain_GameView.h"
#include "editor/main/EMain_Hierarchy.h"
#include "editor/main/EMain_Inspector.h"
#include "editor/main/EMain_Project.h"
#include "editor/main/EMain_SceneView.h"
#include "editor/tools/shader/CompileShader.h"
#include "editor/window/EConsole.h"
#include "editor/window/ELightSetting.h"
#include "editor/EditorAppliction.h"
#include "editor/EditorMain.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(EMain_Menu, EWindowBase);
void EMain_Menu::Render()
{
	DC_PROFILE_FUNCTION();
	static bool show_app_style_editor = true;
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem(ICON_FA_PLUS "New Scane"))
			{
				SceneManager::CreateScene("New Scene");
			}
			if (ImGui::MenuItem(NO_ICON "Open Scene"))
			{
				std::string select_path = Platform::OpenFileDialog("Select Scene", ".", "", "scene", true);
				Debuger::Log("Open Scene:%s", select_path.c_str());
				if (!select_path.empty())
				{
					String path = ProjectManager::ToProjectAssetsPath(select_path);
					SceneManager::LoadScene(path, LoadSceneMode::Single);
				}
			}
			if (ImGui::MenuItem(ICON_FA_SAVE "Save Scene"))
			{
				SceneManager::SaveScene();
			}
			if (ImGui::MenuItem(NO_ICON "Save Scene As..")) 
			{
				std::string select_path = Platform::OpenFileDialog("Select Scene", ".", "", "scene", false);
				if (!select_path.empty())
				{
					String path = ProjectManager::ToProjectAssetsPath(select_path);
					SceneManager::SaveAsScene(path);
				}
			}
			ImGui::Separator();
			if (ImGui::MenuItem(ICON_FA_PLUS_SQUARE "New Project")) 
			{
				std::string select_path = Platform::OpenFileDialog("New Project", ".", "New Project", "", false);
				if (!select_path.empty())
				{
					EditorAppliction::CreateProject(select_path);
				}
			}
			ImGui::MenuItem(NO_ICON "Open Project", nullptr, &EOpenProject::IsShow);

			if (ImGui::MenuItem(ICON_FA_SAVE "Save Project")) {}
			ImGui::Separator();
			if (ImGui::MenuItem(ICON_FA_COG "Build Setting...", nullptr, false, false)) {}
			ImGui::Separator();
			if (ImGui::MenuItem(ICON_FA_POWER_OFF "Exit"))
			{ 
				Application::Quit();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem(ICON_FA_UNDO "Undo", "CTRL+Z", false, false)) {}
			if (ImGui::MenuItem(ICON_FA_REDO "Redo", "CTRL+Y", false, false)) {}  // Disabled item
			ImGui::Separator();
			if (ImGui::MenuItem(ICON_FA_CLONE "Cut", "CTRL+X", false, false)) {}
			if (ImGui::MenuItem(ICON_FA_COPY "Copy", "CTRL+C", false, false)) {}
			if (ImGui::MenuItem(ICON_FA_PASTE "Paste", "CTRL+V", false, false)) {}
			ImGui::Separator();
			if (ImGui::BeginMenu(NO_ICON "Project Settings"))
			{
				if (ImGui::MenuItem("Tag and Layers"))
				{
					EditorAppliction::SetInspectorType(InspectorType::ProjectSettings_Layers);
				}
				if (ImGui::MenuItem("Physics"))
				{
					EditorAppliction::SetInspectorType(InspectorType::ProjectSettings_Physics);
				}
				if (ImGui::MenuItem("Quality"))
				{
					EditorAppliction::SetInspectorType(InspectorType::ProjectSettings_Quality);
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Assets"))
		{
			if (ImGui::MenuItem("Show in Explorer"))
			{
				if (ProjectManager::GetCurrProject())
				{
					Platform::OpeninExplorer(ProjectManager::GetCurrProject()->GetFullPath().c_str());
				}
			}
			ECommonComponent::ShowCreateAsset();
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("GameObject"))
		{
			ECommonComponent::ShowCreateObject();
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Component"))
		{
			auto OnSelectAsset = [](const String& name)
			{
				if (EditorAppliction::GetInspectorId())
				{
					GameObject* obj = SceneManager::GetGameObject(EditorAppliction::GetInspectorId());
					if (obj)
					{
						obj->AddComponent(name);
					}
				}
			};
			ECommonComponent::ShowAddComponent(OnSelectAsset, EditorAppliction::GetComponents());
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Tools"))
		{
			if (ImGui::MenuItem("Compile Shader"))
			{
				CompileShader compileShader;
				compileShader.StartCompile();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Window"))
		{
			ImGui::MenuItem("Hierarchy", nullptr, &EMain_Hierarchy::IsShow);
			ImGui::MenuItem("Inspector", nullptr, &EMain_Inspector::IsShow);
			ImGui::MenuItem("Project", nullptr, &EMain_Project::IsShow);
			ImGui::MenuItem("Scene", nullptr, &EMain_SceneView::IsShow);
			ImGui::MenuItem("Game", nullptr, &EMain_GameView::IsShow);
			ImGui::MenuItem("Console", nullptr, &EConsole::IsShow);
			ImGui::MenuItem("Lighting", nullptr, &ELightSetting::IsShow);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help"))
		{
			ImGui::MenuItem("About", nullptr, false, false);
			ImGui::MenuItem("Style Editor", nullptr, &EIMGUISetter::IsShow);
			ImGui::MenuItem("Update", nullptr, false, false);
			ImGui::MenuItem("Report a Bug...", nullptr, false, false);
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}
DC_END_NAMESPACE