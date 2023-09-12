#include "EditorMainMenu.h"
#include "runtime/ui/GUI.h"
#include "runtime/Application.h"
#include "runtime/Screen.h"
#include "editor/common/ECommonComponent.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
void ShowAppInfo();
void ShowSceneInfo();
IMPL_DERIVED_REFECTION_TYPE(EditorMainMenu, Object);
static bool is_show_app_info = false;
static bool is_show_scene_info = false;
void EditorMainMenu::Render()
{
	DC_PROFILE_FUNCTION();
	if (is_show_app_info)ShowAppInfo();
	if (is_show_scene_info)ShowSceneInfo();

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Info"))
		{
			if (ImGui::MenuItem("AppInfo")) 
			{
				is_show_app_info = !is_show_app_info;
			}
			if (ImGui::MenuItem("SceneInfo")) 
			{
				is_show_scene_info = !is_show_scene_info;
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}
void ShowAppInfo()
{
	ImGui::Begin("EditorAppInfo", 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::SetWindowPos(ImVec2(0, 20));

	ImGui::Text("FPS:%.1f(%.2fms)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
	ImGui::SameLine(150);
	ImGui::Text("Screen:%.0f*%.0f", Screen::GetWidth(), Screen::GetHeight());

	ImGui::Text("Tris:%d", SceneManager::GetDrawTriangles());
	ImGui::SameLine(150);
	ImGui::Text("DrawCall:%d", SceneManager::GetDrawCall());

	ImGui::Text("Batch:%d", SceneManager::GetRenderBatch());
	ImGui::SameLine(150);
	ImGui::Text("SetPass:%d", SceneManager::GetSetPassCall());

	ImGui::End();
}

void ShowSceneNode(Transform* node)
{
	if (ImGui::TreeNode(node->GetInstanceName().c_str()))
	{
		for (int j = 0; j < (int)node->GetChildCount(); ++j)
		{
			Transform* child = node->GetChild(j);
			ShowSceneNode(child);
		}
		const Vector<Component*>& componentes = node->GetGameObject()->GetAllComponent();
		for (const auto& component : componentes)
		{
			ImGui::BulletText("%s", component->GetClassId().c_str());
		}
		ImGui::TreePop();
	}
}
void ShowSceneInfo()
{
	ImGui::Begin("SceneGraph", &is_show_scene_info);
	ImGui::SetWindowSize(ImVec2(500, 680));

	if (SceneManager::GetCurrScene())
	{
		Transform* root_node = SceneManager::GetCurrScene()->GetRootObject()->GetTransform();
		for (int i = 0; i < root_node->GetChildCount(); ++i)
		{
			Transform* node = root_node->GetChild(i);
			if (node)
			{
				ShowSceneNode(node);
			}
		}
	}

	ImGui::End();
}
DC_END_NAMESPACE
