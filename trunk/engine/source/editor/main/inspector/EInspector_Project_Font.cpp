#include "EInspector_Project_Font.h"
#include "runtime/font/FontManager.h"
#include "runtime/graphics/null/Texture.h"
#include "runtime/resources/AssetsManager.h"
#include "runtime/ui/GUI.h"
#include "runtime/Application.h"
#include "editor/main/EMain_Hierarchy.h"
#include "editor/EditorAppliction.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
void EInspector_Project_Font::Render(AssetMeta* meta)
{
	DC_PROFILE_FUNCTION();
	base::Render(meta);

	if (!meta)return;
	FontMeta* font_meta = meta->As<FontMeta>();
	if (!font_meta)return;

	RenderGUI(font_meta);
	RenderModal(font_meta);
}
void EInspector_Project_Font::RenderGUI(FontMeta* meta)
{
	char name_buff[MAX_FILE_NAME] = { 0 };
	String font_name = meta->GetFontName();
	Memory::Copy(name_buff, font_name.c_str(), font_name.Size());
	ImGuiEx::Label("Font Name");
	if (ImGui::InputText("##Font Name", name_buff, ARRAY_SIZE(name_buff)))
	{
		_fontMeta.SetFontName(name_buff);
	}

	ImGuiEx::Label("Font Size");
	if (ImGui::DragInt("##Font Size", &_fontMeta._fontSize, 0.1f, 1, 512))
	{
	}
	
	ImGui::Separator();
	ImGui::NewLine();
	{
		const float width = ImGui::GetContentRegionAvail().x;
		float x = ImGui::GetCursorScreenPos().x + width - 2.0f * (ImGui::CalcTextSize("Revert").x + 15.0f);
		ImGui::SetCursorScreenPos(ImVec2(x, ImGui::GetCursorScreenPos().y - 10.0f));
		if (ImGui::Button("Revert"))
		{
			_fontMeta = *meta;
		}
		ImGui::SameLine();
		if (ImGui::Button("Apply") && *meta != _fontMeta)
		{
			*meta = _fontMeta;
			meta->Deserialize();
		}
	}
}
void EInspector_Project_Font::RenderModal(FontMeta* meta)
{
	static FontMeta old_tmp_meta;
	static String old_meta_guid = "";
	if (meta->GetGUID() != _fontMeta.GetGUID())
	{//是否选择了其他文件
		FontMeta* old_meta = dynamic_cast<FontMeta*>(AssetsManager::GetMeta(_assetGUID));
		if (old_meta && (*old_meta != _fontMeta))
		{
			old_tmp_meta = _fontMeta;
			old_meta_guid = _assetGUID;
			ImGui::OpenPopup("Unapplied import settings");
		}
		else
		{
			_assetGUID = meta->GetGUID();
			_fontMeta = *meta;
		}
	}
	{//模态对话框
		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		if (ImGui::BeginPopupModal("Unapplied import settings", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::NewLine();
			ImGui::TextUnformatted(old_tmp_meta.GetFullPath().c_str());
			ImGui::NewLine();
			if (ImGui::Button("Apply", ImVec2(120, 0)))
			{
				FontMeta* old_meta = dynamic_cast<FontMeta*>(AssetsManager::GetMeta(old_meta_guid));
				if (old_meta)
				{
					*old_meta = old_tmp_meta;
					old_meta->Deserialize();
				}
				_assetGUID = meta->GetGUID();
				_fontMeta = *meta;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			if (ImGui::Button("Revert", ImVec2(120, 0)))
			{
				_assetGUID = meta->GetGUID();
				_fontMeta = *meta;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}
}
DC_END_NAMESPACE