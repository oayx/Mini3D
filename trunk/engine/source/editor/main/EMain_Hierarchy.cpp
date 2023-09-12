#include "EMain_Hierarchy.h"
#include "runtime/graphics/Material.h"
#include "runtime/input/Input.h"
#include "runtime/scene/SceneManager.h"
#include "runtime/Application.h"
#include "editor/common/ECommonComponent.h"
#include "editor/main/EMain_SceneView.h"
#include "editor/EditorAppliction.h"
#include "external/imgui/ex/imgui_user.inl"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(EMain_Hierarchy, EWindowBase);
bool EMain_Hierarchy::IsShow = true;
bool EMain_Hierarchy::_isFocus = false;
void EMain_Hierarchy::Render()
{
	DC_PROFILE_FUNCTION();
	if (!IsShow)return;

	ImGui::SetNextWindowSize(ImVec2(600, 800), ImGuiCond_FirstUseEver);
	ImGui::Begin(ICON_FA_STREAM " Hierarchy", &IsShow, ImGuiWindowFlags_NoCollapse);

	if (ImGui::BeginPopupContextWindow())
	{
		ShowRightMenu("");
		ImGui::EndPopup();
	}
	ShowSceneGraph();
	HandleShortcutKey();

	_isFocus = ImGui::IsWindowFocused();
	ImGui::End();
}
void EMain_Hierarchy::ShowSceneGraph()
{
	static char filter[128] = "";
	{
		const float w = ImGui::CalcTextSize(ICON_FA_TIMES).x + ImGui::GetStyle().ItemSpacing.x * 2;
		ImGui::SetNextItemWidth(-w);
		ImGui::InputTextWithHint("##filter", "Filter", filter, sizeof(filter));
		ImGui::SameLine();
		if (ImGuiEx::IconButton(ICON_FA_TIMES, "Clear filter"))
		{
			filter[0] = '\0';
		}
	}

	for (int i = 0; i < SceneManager::GetSceneCount(); ++i)
	{
		ImGui::PushID(i);
		Scene* scene = SceneManager::GetScene(i);
		if (scene)
		{
			ShowScene(scene, filter);
		}
		ImGui::PopID();
	}
}
void EMain_Hierarchy::ShowScene(Scene* scene, const char* filter)
{
	bool open = (ImGui::CollapsingHeader(scene->GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen));
	if(open)
	{
		bool result = false;
		Transform* root_node = SceneManager::GetCurrScene()->GetRootObject()->GetTransform();
		for (int i = 0; i < root_node->GetChildCount(); ++i)
		{
			ImGui::PushID(i);
			Transform* node = root_node->GetChild(i);
			if (filter[0] == '\0')
			{
				bool ret = ShowNode(node);
				if (!result && ret)result = true;
			}
			else
			{
				bool ret = ShowSearchNode(node, filter);
				if (!result && ret)result = true;
			}
			ImGui::PopID();
		}
		//取消选中
		if (ImGui::IsWindowHovered(0) && ImGui::IsMouseClicked(0) && !result)
		{
			EditorAppliction::SetInspectorType(InspectorType::Hierarchy, 0);
		}
	}
}
bool EMain_Hierarchy::ShowNode(Transform* node)
{
	bool result = false;

	ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
	const String& node_name = node->GetInstanceName();
	ImGuiTreeNodeFlags node_flags = base_flags;

	//选中节点
	bool is_selected = EditorAppliction::GetInspectorId() == node->GetObjectInstanceId() ? true : false;
	if (is_selected)node_flags |= ImGuiTreeNodeFlags_Selected;
	//子节点是否有选中，有就展开
	if (!is_selected)
	{
		for (int i = 0; i < node->GetChildCount(); ++i)
		{//子节点是否有选中的
			ImGui::PushID(i);
			Transform* child_node = node->GetChild(i);
			if (EditorAppliction::GetInspectorId() == child_node->GetObjectInstanceId())
			{
				ImGui::SetNextItemOpen(1);
			}
			ImGui::PopID();
		}
	}

	bool is_active = node->GetGameObject()->ActiveInHierarchy();
	if (node->GetChildCount() > 0)
	{
		if (!is_active)ImGui::PushStyleColor(ImGuiCol_Text, INACTIVE_TEXT_COLOR);
		bool node_open = (ImGui::TreeNodeEx(node_name.c_str(), node_flags));
		if (!is_active)ImGui::PopStyleColor();
		if (ImGui::IsItemClicked())
		{
			EditorAppliction::SetInspectorType(InspectorType::Hierarchy, node->GetObjectInstanceId());
			result = true;
		}
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0) && EditorAppliction::GetInspectorId() == node->GetObjectInstanceId())
		{
			EMain_SceneView::OnDoubleClick(node);
			result = true;
		}
		if (ImGui::IsItemClicked(1))
		{
			EditorAppliction::SetInspectorType(InspectorType::Hierarchy, node->GetObjectInstanceId());
			result = true;
		}
		if (node_open)
		{
			for (int i = 0; i < node->GetChildCount(); ++i)
			{//子节点
				ImGui::PushID(i);
				Transform* child_node = node->GetChild(i);
				bool ret = ShowNode(child_node);
				if (!result && ret)result = true;
				ImGui::PopID();
			}
			ImGui::TreePop();
		}
	}
	else
	{//叶子
		node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

		if(!is_active)ImGui::PushStyleColor(ImGuiCol_Text, INACTIVE_TEXT_COLOR);
		ImGui::TreeNodeEx(node_name.c_str(), node_flags);
		if (!is_active)ImGui::PopStyleColor();
		if (ImGui::IsItemClicked())
		{
			EditorAppliction::SetInspectorType(InspectorType::Hierarchy, node->GetObjectInstanceId());
			result = true;
		}
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0) && EditorAppliction::GetInspectorId() == node->GetObjectInstanceId())
		{
			EMain_SceneView::OnDoubleClick(node);
			result = true;
		}
		if (ImGui::IsItemClicked(1))
		{
			EditorAppliction::SetInspectorType(InspectorType::Hierarchy, node->GetObjectInstanceId());
			result = true;
		}
	}
	return result;
}
bool EMain_Hierarchy::ShowSearchNode(Transform* node, const char* filter)
{
	bool result = false;
	const String& node_name = node->GetInstanceName();
	const char* sz_name = node_name.c_str();

	if (String::Stristr(sz_name, filter) != nullptr)
	{
		//选中节点
		bool is_selected = EditorAppliction::GetInspectorId() == node->GetObjectInstanceId() ? true : false;
		ImGui::Selectable(sz_name, is_selected);

		if (ImGui::IsItemClicked())
		{
			EditorAppliction::SetInspectorType(InspectorType::Hierarchy, node->GetObjectInstanceId());
			result = true;
		}
		if (ImGui::IsMouseDoubleClicked(0) && EditorAppliction::GetInspectorId() == node->GetObjectInstanceId())
		{
			EMain_SceneView::OnDoubleClick(node);
			result = true;
		}
		if (ImGui::IsItemClicked(1))
		{
			EditorAppliction::SetInspectorType(InspectorType::Hierarchy, node->GetObjectInstanceId());
			result = true;
		}
	}
	for (int i = 0; i < node->GetChildCount(); ++i)
	{//子节点
		Transform* child_node = node->GetChild(i);
		bool ret = ShowSearchNode(child_node, filter);
		if (!result && ret)result = true;
	}
	return result;
}
void EMain_Hierarchy::ShowRightMenu(const String& node)
{
	static uint64 copy_object_id = 0;
	if (ImGui::Selectable("Copy"))
	{
		copy_object_id = EditorAppliction::GetInspectorId();
	}
	if (ImGui::Selectable("Paste"))
	{
		if (copy_object_id > 0)
		{
			GameObject* obj = SceneManager::GetGameObject(copy_object_id);
			if (obj)
			{
				GameObject* new_obj = GameObject::Instantiation(obj->GetInstanceName(), obj->GetTransform()->GetParent());
				obj->Clone(new_obj);
			}
		}
	}
	if (ImGui::Selectable("Duplicate"))
	{
		CopySelectObject();
	}
	if (ImGui::Selectable("Delete"))
	{
		DeleteSelectObject();
	}

	ECommonComponent::ShowCreateObject();
}
void EMain_Hierarchy::HandleShortcutKey()
{
	if (!ImGui::IsWindowFocused())return;

	if (Input::GetKeyDown(KeyCode::Delete))
	{
		DeleteSelectObject();
	}
	if (Input::GetKeyDown(KeyCode::D) && Input::GetKey(KeyCode::LeftControl))
	{
		CopySelectObject();
	}
}
void EMain_Hierarchy::CopySelectObject()
{
	GameObject* obj = SceneManager::GetGameObject(EditorAppliction::GetInspectorId());
	if (obj)
	{
		GameObject* new_obj = GameObject::Instantiation(obj->GetInstanceName(), obj->GetTransform()->GetParent());
		obj->Clone(new_obj);
	}
}
void EMain_Hierarchy::DeleteSelectObject()
{
	GameObject* obj = SceneManager::GetGameObject(EditorAppliction::GetInspectorId());
	if (obj)
	{
		GameObject::Destroy(obj);
		EditorAppliction::SetInspectorType(InspectorType::Hierarchy, 0);
	}
}
DC_END_NAMESPACE