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
void EMain_Hierarchy::Render()
{
	DC_PROFILE_FUNCTION;
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
		Transform* rootNode = SceneManager::GetCurrScene()->GetRootObject()->GetTransform();
		for (int i = 0; i < rootNode->GetChildCount(); ++i)
		{
			ImGui::PushID(i);
			Transform* node = rootNode->GetChild(i);
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

	ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
	const String& nodeName = node->GetInstanceName();
	ImGuiTreeNodeFlags nodeFlags = baseFlags;

	//选中节点
	bool isSelected = EditorAppliction::GetInspectorId() == node->GetObjectInstanceId() ? true : false;
	if (isSelected)nodeFlags |= ImGuiTreeNodeFlags_Selected;
	//子节点是否有选中，有就展开
	if (!isSelected)
	{
		for (int i = 0; i < node->GetChildCount(); ++i)
		{//子节点是否有选中的
			ImGui::PushID(i);
			Transform* childNode = node->GetChild(i);
			if (EditorAppliction::GetInspectorId() == childNode->GetObjectInstanceId())
			{
				ImGui::SetNextItemOpen(1);
			}
			ImGui::PopID();
		}
	}

	bool isActive = node->GetGameObject()->ActiveInHierarchy();
	if (node->GetChildCount() > 0)
	{
		if (!isActive)ImGui::PushStyleColor(ImGuiCol_Text, INACTIVE_TEXT_COLOR);
		bool node_open = (ImGui::TreeNodeEx(nodeName.c_str(), nodeFlags));
		if (!isActive)ImGui::PopStyleColor();
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
				Transform* childNode = node->GetChild(i);
				bool ret = ShowNode(childNode);
				if (!result && ret)result = true;
				ImGui::PopID();
			}
			ImGui::TreePop();
		}
	}
	else
	{//叶子
		nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

		if(!isActive)ImGui::PushStyleColor(ImGuiCol_Text, INACTIVE_TEXT_COLOR);
		ImGui::TreeNodeEx(nodeName.c_str(), nodeFlags);
		if (!isActive)ImGui::PopStyleColor();
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
	const String& nodeName = node->GetInstanceName();
	const char* szName = nodeName.c_str();

	if (String::Stristr(szName, filter) != nullptr)
	{
		//选中节点
		bool isSelected = EditorAppliction::GetInspectorId() == node->GetObjectInstanceId() ? true : false;
		ImGui::Selectable(szName, isSelected);

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
		Transform* childNode = node->GetChild(i);
		bool ret = ShowSearchNode(childNode, filter);
		if (!result && ret)result = true;
	}
	return result;
}
void EMain_Hierarchy::ShowRightMenu(const String& node)
{
	static uint64 copyObjectId = 0;
	if (ImGui::Selectable("Copy"))
	{
		copyObjectId = EditorAppliction::GetInspectorId();
	}
	if (ImGui::Selectable("Paste"))
	{
		if (copyObjectId > 0)
		{
			GameObject* obj = SceneManager::GetGameObject(copyObjectId);
			if (obj)
			{
				GameObject* newObj = GameObject::Instantiation(obj->GetInstanceName(), obj->GetTransform()->GetParent());
				obj->Clone(newObj);
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
		GameObject* newObj = GameObject::Instantiation(obj->GetInstanceName(), obj->GetTransform()->GetParent());
		obj->Clone(newObj);
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