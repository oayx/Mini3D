#include "EOpenProject.h"
#include "runtime/project/ProjectManager.h"
#include "runtime/resources/Resources.h"
#include "editor/common/ECommonComponent.h"
#include "editor/EditorAppliction.h"
#include "editor/EditorDefine.h"
#include "editor/EditorMain.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
bool EOpenProject::IsShow = true;
void EOpenProject::Render()
{
	if (!IsShow)return;

	ImGui::OpenPopup("Open or Create Project");

	{//模态对话框
		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		ImGui::SetNextWindowSize(ImVec2(800.0f, 600.0f));
		if (ImGui::BeginPopupModal("Open or Create Project", NULL, ImGuiWindowFlags_NoResize))
		{
			ImGui::NewLine();

			ImGui::Dummy(ImVec2(10.0f, 0.0f));
			ImGui::SameLine();
			ImGui::PushFont(EditorMain::GetBigFont());
			ImGui::TextUnformatted("Projects");
			ImGui::PopFont();
			ImGui::SameLine();

			ImGui::SameLine();
			ImGui::Dummy(ImVec2(380.0f, 0.0f));
			ImGui::SameLine();
			if (ImGui::Button("New", ImVec2(120, 0)))
			{
				std::string select_path = Platform::OpenFileDialog("New Project", ".", "New Project", "", false);
				if (!select_path.empty())
				{
					bool result = EditorAppliction::CreateProject(select_path);
					if (result)
					{
						ImGui::CloseCurrentPopup();
						EOpenProject::IsShow = false;
					}
				}
			}

			ImGui::SameLine();
			if (ImGui::Button("Open", ImVec2(120, 0)))
			{
				std::string select_path = Platform::OpenFolderDialog("Open Project", Path::GetCurrentProcessDirectory().c_str());
				if (!select_path.empty())
				{
					bool result = EditorAppliction::OpenProject(select_path);
					if (result)
					{
						ImGui::CloseCurrentPopup();
						EOpenProject::IsShow = false;
					}
				}
			}

			ImGui::SameLine();
			if (ImGui::Button("Close"))
			{
				if (ProjectManager::GetCurrProject())
				{
					ImGui::CloseCurrentPopup();
				}
				EOpenProject::IsShow = false;
			}

			ImGui::Separator();

			ImGui::NewLine();
			ImGui::BeginChild("project list", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));
			int count = ProjectManager::GetProjectCount();
			for (int i = count - 1; i >= 0; --i)
			{
				ImGui::PushID(i);
				Project* project = ProjectManager::GetProject(i);
				if (!project)continue;
				String full_path = project->GetFullPath();
				String name = Path::GetFileName(full_path);
				String path = Path::GetDirectoryName(full_path);
				{//名称
					ImGui::Dummy(ImVec2(5.0f, 0.0f));
					ImGui::SameLine();
					ImGui::PushFont(EditorMain::GetBigFont());
					if (ImGui::Selectable(name.c_str()))
					{
						bool result = EditorAppliction::OpenProject(full_path);
						if (result)
						{
							ImGui::CloseCurrentPopup();
							EOpenProject::IsShow = false;
						}
					}
					ImGui::PopFont();
				}
				{//路径
					ImGui::Dummy(ImVec2(5.0f, 0.0f));
					ImGui::SameLine();
					ImGui::Text("path:%s", path.c_str());
				}

				if (i != 0)
				{
					ImGui::Separator();
					ImGui::NewLine();
				}
				ImGui::PopID();
			}
			ImGui::EndChild();

			ImGui::EndPopup();
		}
	}
}
DC_END_NAMESPACE