#include "EInspector_Project_Mesh.h"
#include "runtime/graphics/null/Texture.h"
#include "runtime/resources/AssetsManager.h"
#include "runtime/ui/GUI.h"
#include "runtime/Application.h"
#include "editor/main/EMain_Hierarchy.h"
#include "editor/EditorAppliction.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
void EInspector_Project_Mesh::Render(AssetMeta* meta)
{
	DC_PROFILE_FUNCTION;
	base::Render(meta);

	if (!meta)return;
	MeshMeta* mesh_meta = meta->As<MeshMeta>();
	if (!mesh_meta)return;

	RenderGUI(mesh_meta);
	RenderModal(mesh_meta);
}
void EInspector_Project_Mesh::RenderGUI(MeshMeta* meta)
{
	if (ImGui::BeginTabBar("##EInspector_Project_Mesh", ImGuiTabBarFlags_None))
	{
		if (ImGui::BeginTabItem("  Model  "))
		{
			ImGuiEx::Label("Gen Normal");
			ImGui::Checkbox("##GenNormal", &_meshMeta._genNormal);

			ImGuiEx::Label("Gen Tangent");
			ImGui::Checkbox("##GenTangent", &_meshMeta._genTangent);

			ImGuiEx::Label("Optimize Mesh");
			ImGui::Checkbox("##OptimizeMesh", &_meshMeta._optimizeMesh);

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("  Animation  "))
		{
			ImGuiEx::Label("Import Animation");
			ImGui::Checkbox("##Import Animation", &_meshMeta._importAnimation);

			if (_meshMeta._importAnimation)
			{
				ImGui::NewLine();
				static int selected = 0;
				if (selected >= _meshMeta._clips.Size())selected = -1;
				{//clips
					if (ImGui::BeginTable("ImportAnimationTable1", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_NoBordersInBodyUntilResize))
					{
						ImGui::TableSetupColumn("Clips", ImGuiTableColumnFlags_WidthStretch);
						ImGui::TableSetupColumn("Start", ImGuiTableColumnFlags_WidthFixed, 50.0f);
						ImGui::TableSetupColumn("End", ImGuiTableColumnFlags_WidthFixed, 50.0f);
						ImGui::TableHeadersRow();
						for (int row = 0; row < _meshMeta._clips.Size(); row++)
						{
							ImGui::TableNextRow();
							const ClipAnimationDesc& clip = _meshMeta._clips[row];

							ImGui::TableSetColumnIndex(0);
							if (ImGui::Selectable(clip.Name.c_str(), selected == row, ImGuiSelectableFlags_SpanAllColumns))
								selected = row;
							ImGui::TableSetColumnIndex(1);
							ImGui::Text("%d", clip.StartFrame);
							ImGui::TableSetColumnIndex(2);
							ImGui::Text("%d", clip.EndFrame);
						}
						ImGui::EndTable();
					}
				}

				{//+ - 
					const float width = ImGui::GetContentRegionAvail().x;
					ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + width - 80, ImGui::GetCursorScreenPos().y));
					if (ImGui::SmallButton(ICON_FA_PLUS))
					{
						ClipAnimationDesc clip; clip.Name = "New Clip";
						_meshMeta._clips.Add(clip);
						selected = _meshMeta._clips.Size() - 1;
					}
					ImGui::SameLine();
					if (ImGui::SmallButton(ICON_FA_MINUS))
					{
						if (selected >= 0)
						{
							_meshMeta._clips.RemoveAt(selected);
							selected = -1;
						}
					}
				}

				ImGui::NewLine();
				{//clip info
					ClipAnimationDesc clip;
					if (selected >= 0)clip = _meshMeta._clips[selected];

					ImGuiEx::Label("Name");
					char sz_layer_name[50] = { 0 };
					Strncpy(sz_layer_name, clip.Name.c_str(), clip.Name.Size() >= ARRAY_SIZE(sz_layer_name) ? ARRAY_SIZE(sz_layer_name) - 1 : clip.Name.Size());
					if (ImGui::InputText("##Clip Name", sz_layer_name, ARRAY_SIZE(sz_layer_name)) && selected >= 0)
					{
						_meshMeta._clips[selected].Name = sz_layer_name;
					}

					int start_frame = clip.StartFrame;
					ImGuiEx::Label("Start");
					if (ImGui::InputInt("##Clip Start", &start_frame) && selected >= 0)
					{
						if (start_frame < 0)start_frame = 0;
						_meshMeta._clips[selected].StartFrame = start_frame;
					}

					int end_frame = clip.EndFrame;
					ImGuiEx::Label("End");
					if (ImGui::InputInt("##Clip End", &end_frame) && selected >= 0)
					{
						if (end_frame < start_frame)end_frame = start_frame;
						_meshMeta._clips[selected].EndFrame = end_frame;
					}

					ImGuiEx::Label("Loop");
					if (ImGui::Checkbox("##Clip Loop", &clip.Loop) && selected >= 0)
					{
						_meshMeta._clips[selected].Loop = clip.Loop;
					}
				}
			}

			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	ImGui::Separator();
	ImGui::NewLine();
	{
		const float width = ImGui::GetContentRegionAvail().x;
		float x = ImGui::GetCursorScreenPos().x + width - 2.0f * (ImGui::CalcTextSize("Revert").x + 15.0f);
		ImGui::SetCursorScreenPos(ImVec2(x, ImGui::GetCursorScreenPos().y - 10.0f));
		if (ImGui::Button("Revert"))
		{
			_meshMeta = *meta;
		}
		ImGui::SameLine();
		if (ImGui::Button("Apply"))
		{
			*meta = _meshMeta;
			meta->Deserialize();
		}
	}
}
void EInspector_Project_Mesh::RenderModal(MeshMeta* meta)
{
	static MeshMeta old_tmp_meta;
	static String old_meta_guid = "";
	if (meta->GetGUID() != _meshMeta.GetGUID())
	{//是否选择了其他文件
		MeshMeta* old_meta = dynamic_cast<MeshMeta*>(AssetsManager::GetMeta(_assetGUID));
		if (old_meta && (*old_meta != _meshMeta))
		{
			old_tmp_meta = _meshMeta;
			old_meta_guid = _assetGUID;
			ImGui::OpenPopup("Unapplied import settings");
		}
		else
		{
			_assetGUID = meta->GetGUID();
			_meshMeta = *meta;
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
				MeshMeta* old_meta = dynamic_cast<MeshMeta*>(AssetsManager::GetMeta(old_meta_guid));
				if (old_meta)
				{
					*old_meta = old_tmp_meta;
					old_meta->Deserialize();
				}
				_assetGUID = meta->GetGUID();
				_meshMeta = *meta;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			if (ImGui::Button("Revert", ImVec2(120, 0)))
			{
				_assetGUID = meta->GetGUID();
				_meshMeta = *meta;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}
}
DC_END_NAMESPACE