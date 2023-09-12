#include "EInspector_ProjectSettings_Layers.h"
#include "runtime/scene/LayerMask.h"
#include "runtime/scene/TagManager.h"
#include "editor/EditorAppliction.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
void EInspector_ProjectSettings_Layers::Render()
{
	RenderLayer();
	RenderTag();
}
void EInspector_ProjectSettings_Layers::RenderLayer()
{
	if (ImGui::CollapsingHeader("Layers", ImGuiTreeNodeFlags_DefaultOpen))
	{
		for (int i = 0; i < 5; ++i)
		{
			char sz_name[30] = { 0 };
			int ret = Snprintf(sz_name, ARRAY_SIZE(sz_name), "Builtin Layer %d", i);

			ImGui::PushID(i);
			ImGuiEx::Label(sz_name);

			const String& layer_name = LayerMask::LayerToName(i);
			ImGui::TextUnformatted(layer_name.c_str());

			ImGui::PopID();
		}

		for (int i = 5; i < 32; ++i)
		{
			char sz_name[30] = { 0 };
			int ret = Snprintf(sz_name, ARRAY_SIZE(sz_name), "User Layer %d", i);

			ImGui::PushID(i);
			ImGuiEx::Label(sz_name);

			const String& layer_name = LayerMask::LayerToName(i);
			char sz_layer_name[LayerMask::MAX_NAME_SIZE] = { 0 };
			Strncpy(sz_layer_name, layer_name.c_str(), layer_name.Size() >= ARRAY_SIZE(sz_layer_name) ? ARRAY_SIZE(sz_layer_name) - 1 : layer_name.Size());
			if (ImGui::InputText("LayerName", sz_layer_name, ARRAY_SIZE(sz_layer_name)))
			{
				LayerMask::SetLayerName(i, sz_layer_name);
				LayerMask::Save();
			}

			ImGui::PopID();
		}
	}
}
void EInspector_ProjectSettings_Layers::RenderTag()
{
	if (ImGui::CollapsingHeader("Tags"))
	{
		static int selected = -1;
		for (int i = 0; i < TagManager::GetTagCount(); ++i)
		{
			ImGui::PushID(i);

			char sz_name[10] = { 0 };
			int ret = Snprintf(sz_name, ARRAY_SIZE(sz_name), "Tag %d", i);
			ImGuiEx::Label(sz_name);

			const String& tag_name = TagManager::GetTag(i);
			char sz_tag_name[TagManager::MAX_NAME_SIZE] = { 0 };
			Strncpy(sz_tag_name, tag_name.c_str(), tag_name.Size() >= ARRAY_SIZE(sz_tag_name) ? ARRAY_SIZE(sz_tag_name) - 1 : tag_name.Size());
			ImGui::SameLine();
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 40);
			if (ImGui::InputText("##tag name", sz_tag_name, ARRAY_SIZE(sz_tag_name)))
			{
				TagManager::ModifyTag(i, sz_tag_name);
				TagManager::Save();
			}

			ImGui::SameLine();
			ImGui::SetNextItemWidth(-1);
			if (ImGui::Button(ICON_FA_MINUS))
			{
				TagManager::RemoveTag(i);
				TagManager::Save();
			}
			ImGui::PopID();
		}

		ImGui::Separator();

		const float width = ImGui::GetContentRegionAvail().x;
		ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + width - 35, ImGui::GetCursorScreenPos().y));
		if (ImGui::Button(ICON_FA_PLUS))
		{
			TagManager::AddTag("New Tag");
			TagManager::Save();
		}
	}
}
DC_END_NAMESPACE