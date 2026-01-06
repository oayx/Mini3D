#include "EMain_Tools.h"
#include "runtime/resources/Resources.h"
#include "runtime/ui/GUI.h"
#include "runtime/Application.h"
#include "editor/common/ECommonComponent.h"
#include "editor/help/EIMGUISetter.h"
#include "editor/main/EMain_Hierarchy.h"
#include "editor/main/EMain_Inspector.h"
#include "editor/main/EMain_Project.h"
#include "editor/window/EConsole.h"
#include "editor/EditorAppliction.h"
#include "editor/EditorMain.h"
#include "external/imgui/imgui_internal.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(EMain_Tools, EWindowBase);	
GizmoOperation EMain_Tools::GetOperation()
{
	for (int i = 0; i < (int)GizmoOperation::Max; ++i)
	{
		if (_gizmoOperation[i])return GizmoOperation(i);
	}
	return GizmoOperation::None;
}
void EMain_Tools::Render()
{
	DC_PROFILE_FUNCTION;
	ShowLeftToolbar();
	ShowPlayButton();
	ShowRightToolbar();
}
void EMain_Tools::ShowLeftToolbar()
{
	const ImVec4 colActive = ImGui::GetStyle().Colors[ImGuiCol_ButtonActive];
	const ImVec4 bgColor = ImGui::GetStyle().Colors[ImGuiCol_Text];

	ImGui::Spacing();
	if (ImGuiEx::ToolbarButton(EditorMain::GetBigIconFont(), ICON_FA_HAND_ROCK, _gizmoOperation[(int)GizmoOperation::Drag] ? colActive : bgColor, "Drag"))
	{
		ResetOperation();
		_gizmoOperation[(int)GizmoOperation::Drag] = !_gizmoOperation[(int)GizmoOperation::Drag];
	}
	ImGui::Spacing();
	if (ImGuiEx::ToolbarButton(EditorMain::GetBigIconFont(), ICON_FA_ARROWS_ALT, _gizmoOperation[(int)GizmoOperation::Translate] ? colActive : bgColor, "Translate"))
	{
		ResetOperation();
		_gizmoOperation[(int)GizmoOperation::Translate] = !_gizmoOperation[(int)GizmoOperation::Translate];
	}
	ImGui::Spacing();
	if (ImGuiEx::ToolbarButton(EditorMain::GetBigIconFont(), ICON_FA_UNDO, _gizmoOperation[(int)GizmoOperation::Rotate] ? colActive : bgColor, "Rotate"))
	{
		ResetOperation();
		_gizmoOperation[(int)GizmoOperation::Rotate] = !_gizmoOperation[(int)GizmoOperation::Rotate];
	}
	ImGui::Spacing();
	if (ImGuiEx::ToolbarButton(EditorMain::GetBigIconFont(), ICON_FA_EXPAND_ARROWS_ALT, _gizmoOperation[(int)GizmoOperation::Scale] ? colActive : bgColor, "Scale"))
	{
		ResetOperation();
		_gizmoOperation[(int)GizmoOperation::Scale] = !_gizmoOperation[(int)GizmoOperation::Scale];
	}
	if (EditorConfig::Is3D)
	{
		ImGui::Spacing();
		if (ImGuiEx::ToolbarButton(EditorMain::GetBigIconFont(), ICON_FA_SNOWFLAKE, _gizmoOperation[(int)GizmoOperation::Universal] ? colActive : bgColor, "Universal"))
		{
			ResetOperation();
			_gizmoOperation[(int)GizmoOperation::Universal] = !_gizmoOperation[(int)GizmoOperation::Universal];
		}
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	//local+world
	if (ImGui::Selectable(EditorConfig::IsLocal ? ICON_FA_CUBE " Local" : ICON_FA_GLOBE " World", false, 0, ImVec2(ImGui::CalcTextSize(ICON_FA_GLOBE " World").x, 0.0f)))
	{
		EditorConfig::IsLocal = !EditorConfig::IsLocal;
	}
}
void EMain_Tools::ShowRightToolbar()
{
	{//切换显示模式
		const float width = ImGui::GetContentRegionAvail().x;
		float x = ImGui::GetCursorScreenPos().x + width - 100.0f;
		ImGui::SetCursorScreenPos(ImVec2(x, ImGui::GetCursorScreenPos().y));
		ImGui::SetNextItemWidth(90.0f);

		const char* values[] = { "Default", "2 by 3" };
		ImGui::Combo("##Layout", &EditorConfig::LayoutIndex, values, ARRAY_SIZE(values));
	}
}
void EMain_Tools::ShowPlayButton()
{
	float w = ImGui::GetWindowContentRegionWidth() * 0.5f - 30.0f - ImGui::GetCursorPosX();
	ImGui::Dummy(ImVec2(w, ImGui::GetTextLineHeight()));
	{
		const ImVec4 colActive = ImGui::GetStyle().Colors[ImGuiCol_ButtonActive];
		const ImVec4 bgColor = ImGui::GetStyle().Colors[ImGuiCol_Text];

		ImGui::SameLine();
		if (ImGuiEx::ToolbarButton(EditorMain::GetBigIconFont(), ICON_FA_PLAY, Application::IsPlay() ? colActive : bgColor, "Play"))
		{
			bool is_playing = Application::IsPlay();
			if (!is_playing)
			{//unplay->play
				SceneManager::SaveScene();
				Scene* currScene = SceneManager::GetCurrScene();
				if (currScene)
				{//重新加载场景
					SceneManager::LoadScene(currScene->GetFile(), LoadSceneMode::Single);
				}
			}
			Application::SetPlay(!is_playing);
			if (!Application::IsPlay())
			{//play->unplay
				Scene* currScene = SceneManager::GetCurrScene();
				if (currScene)
				{//恢复场景
					SceneManager::LoadScene(currScene->GetFile(), LoadSceneMode::Single);
				}
			}
		}

		ImGui::SameLine(0, 10);
		if (ImGuiEx::ToolbarButton(EditorMain::GetBigIconFont(), ICON_FA_PAUSE, Application::IsPause() ? colActive : bgColor, "Pause"))
		{
			Application::SetPause(!Application::IsPause());
		}

		ImGui::SameLine(0, 10);
		if (ImGuiEx::ToolbarButton(EditorMain::GetBigIconFont(), ICON_FA_STEP_FORWARD, bgColor, "NextFrame"))
		{

		}
	}
}
void EMain_Tools::ResetOperation()
{
	for (int i = 0; i < (int)GizmoOperation::Max; ++i)
	{
		_gizmoOperation[i] = false;
	}
}
DC_END_NAMESPACE