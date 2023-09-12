#include "EInspector_Hierarchy.h"
#include "runtime/scene/LayerMask.h"
#include "runtime/scene/TagManager.h"
#include "runtime/Application.h"
#include "editor/common/ECommonComponent.h"
#include "editor/main/EMain_Hierarchy.h"
#include "editor/EditorAppliction.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
void EInspector_Hierarchy::Render()
{
	DC_PROFILE_FUNCTION();
	uint64 select_obj_id = EditorAppliction::GetInspectorId();
	if (select_obj_id == 0)return;

	GameObject* select_obj = SceneManager::GetGameObject(select_obj_id);
	if (!select_obj)return;

	ShowHeader(select_obj);
	ShowTransform(select_obj);
	ShowComponent(select_obj);
	ShowAddComponent(select_obj);
}
void EInspector_Hierarchy::ShowHeader(GameObject* select_obj)
{
	//是否显示
	bool is_show = select_obj->ActiveSelf();
	ImGuiEx::Label("Visible");
	if (ImGui::Checkbox("##Visible", &is_show))
	{
		select_obj->SetActive(is_show);
	}
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Set GameObject Visible");

	//静态
	static bool is_static = false;
	ImGuiEx::Label("Static");
	if (ImGui::Checkbox("##Static", &is_static))
	{
	}

	//名称
	char name_buff[MAX_FILE_NAME] = { 0 };
	const String& name = select_obj->GetInstanceName();
	AssertEx(name.Size() < ARRAY_SIZE(name_buff), "");
	Memory::Copy(name_buff, name.c_str(), name.Size());
	ImGuiEx::Label("Name");
	if (ImGui::InputText("##Name", name_buff, ARRAY_SIZE(name_buff)))
	{
		select_obj->SetInstanceName(name_buff);
	}

	//Tag
	ImGuiEx::Label("Tag");
	if (ImGui::BeginCombo("##Tag", select_obj->GetTag().c_str()))
	{
		for (int i = 0; i < TagManager::GetTagCount(); ++i)
		{
			if (ImGui::Selectable(TagManager::GetTag(i).c_str()))
			{
				select_obj->SetTag(TagManager::GetTag(i));
			}
		}

		ImGui::EndCombo();
	}

	//layer
	{
		ImGuiEx::Label("Layer");
		if (ImGui::BeginCombo("##Layer", LayerMask::LayerToName(select_obj->GetLayer()).c_str()))
		{
			for (int i = 0; i < 33; ++i)
			{
				if (i == 32)
				{
					ImGui::Separator();
					if (ImGui::Selectable("Add Layer..."))
					{
						EditorAppliction::SetInspectorType(InspectorType::ProjectSettings_Layers);
					}
				}
				else
				{
					const char* sz_name = LayerMask::LayerToName(i).c_str();
					if (sz_name[0] == '\0')continue;

					bool is_select = select_obj->GetLayer() == i;

					char sz_full_name[LayerMask::MAX_NAME_SIZE + 10] = { 0 };
					Snprintf(sz_full_name, ARRAY_SIZE(sz_full_name), " %d : %s", i, sz_name);

					if (ImGui::Checkbox(sz_full_name, &is_select))
					{
						select_obj->SetLayer(i);
					}
				}
			}
			ImGui::EndCombo();
		}
	}

	ImGui::NewLine();
}
void EInspector_Hierarchy::ShowTransform(GameObject* select_obj)
{
	ImGui::Separator();
	Transform* transform = select_obj->GetTransform();

}
void EInspector_Hierarchy::ShowComponent(GameObject* select_obj)
{
	const Vector<Component*>& componentes = select_obj->GetAllComponent();
	//先显示Transfrom
	for (int i = 0; i < componentes.Size(); ++i)
	{
		Component* component = componentes[i];
		if (!component->GetType()->HasFlag(TypeFlag::ShowInEditor))continue;
		if (!component->Is<Transform>())continue;

		Transform* transform = component->As<Transform>();
		const char* sz_name = transform->GetType()->GetName().c_str();
		bool is_open = ImGui::CollapsingHeader(sz_name, ImGuiTreeNodeFlags_DefaultOpen);
		if (ImGui::BeginPopupContextItem(sz_name))
		{
			if (ImGui::Selectable("Reset"))
			{
				transform->SetLocalPosition(Vector3::zero);
				transform->SetLocalRotation(Quaternion(Vector3::zero));
				transform->SetLocalScale(Vector3::one);
			}
			if (ImGui::Selectable("Reset Positin"))
			{
				transform->SetLocalPosition(Vector3::zero);
			}
			if (ImGui::Selectable("Reset Rotation"))
			{
				transform->SetLocalRotation(Quaternion(Vector3::zero));
			}
			if (ImGui::Selectable("Reset Scale"))
			{
				transform->SetLocalScale(Vector3::one);
			}
			ImGui::EndPopup();
		}
		if (is_open)
		{
			component->OnDrawEditor();
		}
		break;
	}
	for(int i = 0; i < componentes.Size(); ++i)
	{
		Component* component = componentes[i];
		if (!component->GetType()->HasFlag(TypeFlag::ShowInEditor))continue;
		if (component->Is<Transform>())continue;

		ImGui::PushID(i);
		ImGui::Separator();
		const char* sz_name = component->GetType()->GetName().c_str();
		bool is_show = component->IsEnable();
		bool is_open = ImGuiEx::CollapsingHeader(sz_name, &is_show, ImGuiTreeNodeFlags_DefaultOpen);
		if (ImGui::BeginPopupContextItem(sz_name))
		{
			if (ImGui::Selectable("Remove component"))
			{
				select_obj->Destroy(component);
			}
			ImGui::EndPopup();
		}
		if(is_open)
		{
			component->OnDrawEditor();
		}
		if (is_show != component->IsEnable())
		{
			component->SetEnable(is_show);
		}
		ImGui::PopID();
	}
}
void EInspector_Hierarchy::ShowAddComponent(GameObject* select_obj)
{
	ImGui::Separator();
	ImGui::NewLine(); 

	ImVec2 btn_size(220.0f, 25.0f);
	const float x = (ImGui::GetContentRegionAvail().x - btn_size.x) * 0.5f + 8.0f;
	ImGui::SetCursorPosX(x);
	ImVec2 popup_start = ImGui::GetCursorScreenPos(); popup_start.y += ImGui::GetFrameHeightWithSpacing();
	if (ImGui::Button("Add Component", btn_size))
	{
		ImGui::OpenPopup("AddComponentPopup");
	}

	ImGui::SetNextWindowPos(popup_start);
	ImGui::SetNextWindowSize(ImVec2(btn_size.x, 300.0f));
	if (ImGui::BeginPopup("AddComponentPopup", 0)) 
	{
		static char filter[128] = "";
		const float w = ImGui::CalcTextSize(ICON_FA_TIMES).x + ImGui::GetStyle().ItemSpacing.x * 2.0f;
		ImGui::SetNextItemWidth(-w);
		ImGui::InputTextWithHint("##filter", "Filter", filter, sizeof(filter));
		ImGui::SameLine();
		if (ImGuiEx::IconButton(ICON_FA_TIMES, "Clear filter")) 
		{
			filter[0] = '\0';
		}

		const EComponents& components = EditorAppliction::GetComponents();
		auto OnSelectAsset = [select_obj](const String& name)
		{
			if(select_obj)select_obj->AddComponent(name);
		};
		if (filter[0] == '\0')
		{
			ECommonComponent::ShowAddComponent(OnSelectAsset, components);
		}
		else
		{
			ShowSearchAddComponent(OnSelectAsset, components, filter);
		}

		ImGui::EndPopup();
	}
}
void EInspector_Hierarchy::ShowSearchAddComponent(std::function<void(String)> action, const EComponents& parent_component, const char* filter)
{
	for (const auto& obj : parent_component.Components)
	{
		const char* sz_name = obj.first.c_str();
		if (String::Stristr(sz_name, filter) != nullptr)
		{
			const Type* type = Type::GetType(obj.second.c_str());
			if (type && type->HasFlag(TypeFlag::ShowInEditor))
			{
				if (ImGui::Selectable(sz_name))
				{
					if (action)action(obj.first);
				}
			}
		}
	}
	for (const auto& child : parent_component.Childrends)
	{
		ShowSearchAddComponent(action, child.second, filter);
	}
}
DC_END_NAMESPACE