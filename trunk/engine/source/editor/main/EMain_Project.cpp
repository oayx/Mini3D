#include "EMain_Project.h"
#include "runtime/input/Input.h"
#include "runtime/resources/AssetsManager.h"
#include "runtime/resources/Resources.h"
#include "runtime/ui/GUI.h"
#include "runtime/Application.h"
#include "editor/EditorAppliction.h"
#include "editor/EditorCursor.h"
#include "editor/common/ECommonComponent.h"
#include "editor/common/EditorIcon.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(EMain_Project, EWindowBase);
bool EMain_Project::IsShow = true;
String EMain_Project::_selectPath = "";
String EMain_Project::_selectFile = "";
bool EMain_Project::_isInputMode = false;
void EMain_Project::SetSelectFile(const String& file_path)
{
	if (file_path.IsEmpty())return;
	if (Resource::IsInternal(file_path))return;
	_selectPath = Path::GetDirectoryName(file_path);
	_selectFile = Path::GetFileName(file_path);
	_isInputMode = false;
}
void EMain_Project::Render()
{
	DC_PROFILE_FUNCTION();
	if (!IsShow)return;

	ImGui::SetNextWindowSize(ImVec2(600.0f, 400.0f));
	ImGui::Begin(ICON_FA_FOLDER" Project", &IsShow, ImGuiWindowFlags_NoCollapse);

	if (ImGui::BeginTable("table_projects", 2, ImGuiTableFlags_Resizable))
	{
		ImGui::TableNextColumn();
		{// Left
			if (ImGui::TreeNodeEx("left_assets", ImGuiTreeNodeFlags_DefaultOpen, "%s", "assets"))
			{
				if (AssetsManager::GetRootMeta())
				{
					ImGui::BeginChild("left project view", ImVec2(0.0f, -5.0f));
					ShowLeft(AssetsManager::GetRootMeta()->Children(), _selectPath);
					ImGui::EndChild();
				}

				ImGui::TreePop();
			}
		}
		ImGui::TableNextColumn(); 
		{// Right
			ImGui::BeginGroup();
			ShowRight();
			ImGui::EndGroup();
		}
		ImGui::EndTable();
	}

	//拖动文件到编辑器
	DragFileToEditor();

	ImGui::End();
}
void EMain_Project::ShowLeft(const Vector<AssetMeta*>& dirs, String select_path)
{
	DC_PROFILE_FUNCTION();
	String folder = "";
	if (!select_path.IsEmpty())
	{
		int index = select_path.LastIndexOf('/');
		folder = select_path.Substring(0, index);
		if (index >= 0)
			select_path = select_path.Substring(index + 1);
		else
			select_path = "";
	}
	ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
	for (int i = 0; i < dirs.Size(); i++)
	{
		const AssetMeta* meta = dirs[i];
		if (!meta->IsFolder())continue;

		ImGuiTreeNodeFlags node_flags = base_flags;
		const bool is_selected = _selectPath == meta->GetFullPath() && !_selectPath.IsEmpty() ? true : false;
		if (!folder.IsEmpty())
		{
			if (meta->GetFileName() == folder)node_flags |= ImGuiTreeNodeFlags_DefaultOpen;
		}
		if (is_selected)node_flags |= ImGuiTreeNodeFlags_Selected;

		if (meta->HasChildrenFolder())
		{
			char sz_name[MAX_FILE_NAME] = { 0 };
			Snprintf(sz_name, ARRAY_SIZE(sz_name), "%s %s", ICON_FA_FOLDER, meta->GetFileName().c_str());
			bool node_open = (ImGui::TreeNodeEx(sz_name, node_flags));
			if (ImGui::IsItemClicked())
				_selectPath = meta->GetFullPath();
			if(node_open)
			{
				ShowLeft(meta->Children(), select_path);
				ImGui::TreePop();
			}
		}
		else
		{//叶子
			char sz_name[MAX_FILE_NAME] = { 0 };
			Snprintf(sz_name, ARRAY_SIZE(sz_name), "%s %s", ICON_FA_FOLDER, meta->GetFileName().c_str());
			node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
			ImGui::TreeNodeEx(sz_name, node_flags);
			if (ImGui::IsItemClicked())
				_selectPath = meta->GetFullPath();
		}
	}
}
void EMain_Project::ShowRight()
{
	DC_PROFILE_FUNCTION();
	//显示选中目录
	{
		if (ImGui::SmallButton("assets"))
		{
			_selectPath = "";
		}
		if (!_selectPath.IsEmpty())
		{
			ImGui::SameLine();
			ImGui::Text(">");

			String child_path = _selectPath;
			while (child_path.Size() > 0)
			{
				int index = child_path.IndexOf('/');
				String name = child_path.Substring(0, index >= 0 ? index : -1);
				ImGui::SameLine();
				if (ImGui::SmallButton(name.c_str()))
				{
					int count = child_path.IndexOf('/');
					if (count < 0)count = child_path.Size();
					_selectPath = _selectPath.Substring(0, _selectPath.Size() - child_path.Size() + count).c_str();
				}

				if (index < 0)break;

				ImGui::SameLine();
				ImGui::Text(">");
				child_path = child_path.Substring(index + 1);
			}
		}
	}
	ImGui::Separator();

	ImGui::BeginChild("rigth project view", ImVec2(0.0f, -ImGui::GetFrameHeightWithSpacing() - 5.0f));
	ShowFiles();
	ImGui::EndChild();

	{//切换显示模式
		const float width = ImGui::GetContentRegionAvail().x;
		float x = ImGui::GetCursorScreenPos().x + width - 100.0f;
		ImGui::SetCursorScreenPos(ImVec2(x, ImGui::GetCursorScreenPos().y));
		ImGui::SetNextItemWidth(96.0f);
		ImGui::SliderFloat("##zoom", &EditorConfig::ProjectIconZoom, 1.0f, 3.0f);
	}
}
void EMain_Project::ShowFiles()
{
	//获得目录下所有文件
	const AssetMeta* curr_meta = AssetsManager::GetMetaByPath(_selectPath);
	const float ICON_SIZE = ImGui::GetFont()->FontSize + 3;
	if(curr_meta)
	{
		bool is_end_edit = false;
		static char sz_name_buff[MAX_FILE_NAME] = { 0 };
		static String old_select_file = "";
		if (_isInputMode && (ImGui::IsMouseReleased(0) || ImGui::IsItemClicked(1)))
		{//左键松开或右键按下
			is_end_edit = true;
		}

		for (int show_type = 0; show_type < 2; ++show_type)
		{
			for (int i = 0; i < curr_meta->GetChildrenCount(); ++i)
			{
				const AssetMeta* meta = curr_meta->GetChildren(i);
				bool is_folder = meta->IsFolder();
				if ((show_type == 0 && meta->IsFolder()) ||
					(show_type == 1 && meta->GetFileType() == FileInfoType::File && !IsIgnoreExt(meta->GetExtension())))
				{//先显示目录,再显示文件
					bool select = _selectFile == meta->GetFileName();
					if (EditorConfig::ProjectIconZoom > 1.2f)
					{//ICON模式
						_isInputMode = false;
						bool is_select = ECommonComponent::ShowFileIcon(meta->GetFileName().c_str(), select, EditorIconCache::GetTexture(meta), Vector2(32 + 32 * EditorConfig::ProjectIconZoom, 32 + 32 * EditorConfig::ProjectIconZoom));
						if (is_select)
						{
							_selectFile = meta->GetFileName();
							EditorAppliction::SetInspectorType(InspectorType::Project, meta->GetFullPath());

							if (ImGui::IsMouseDoubleClicked(0) && is_folder)
								_selectPath = meta->GetFullPath();
						}
						if (!is_folder && ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
						{
							ImGui::SetDragDropPayload("ProjectAsset", meta->GetFullPath().c_str(), meta->GetFullPath().Size());
							ImGui::TextUnformatted(meta->GetFileName().c_str());
							EditorCursor::BeginDrag(meta->GetFullPath());
							ImGui::EndDragDropSource();
						}
					}
					else
					{//列表模式
						ImGui::Image((ImTextureID)EditorIconCache::GetTexture(meta), ImVec2(ICON_SIZE, ICON_SIZE));
						ImGui::SameLine();
						if (!_isInputMode || _selectFile != meta->GetFileName())
						{
							if (ImGui::Selectable(meta->GetFileName().c_str(), &select, ImGuiSelectableFlags_AllowDoubleClick))
							{
								if (is_folder && ImGui::IsMouseDoubleClicked(0))
									_selectPath = meta->GetFullPath();
							}
						}
						else
						{
							ImGui::InputText("##GetFileName()", sz_name_buff, ARRAY_SIZE(sz_name_buff));
						}

						if (!is_folder && !_isInputMode && ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
						{
							ImGui::SetDragDropPayload("ProjectAsset", meta->GetFullPath().c_str(), meta->GetFullPath().Size());
							ImGui::TextUnformatted(meta->GetFileName().c_str());
							EditorCursor::BeginDrag(meta->GetFullPath());
							ImGui::EndDragDropSource();
						}
						else
						{
							if ((ImGui::IsItemHovered(0) && ImGui::IsMouseReleased(0)) || ImGui::IsItemClicked(1))
							{
								if ((ImGui::IsItemHovered(0) && ImGui::IsMouseReleased(0)) && _selectFile == meta->GetFileName() && !is_folder)
								{//进入编辑模式
									if (!_isInputMode)
									{
										_isInputMode = true;
										const String& name = meta->GetFileName();
										old_select_file = name;
										AssertEx(name.Size() < ARRAY_SIZE(sz_name_buff), "");
										Memory::Clear(sz_name_buff, ARRAY_SIZE(sz_name_buff));
										Memory::Copy(sz_name_buff, name.c_str(), name.Size());
									}
									is_end_edit = false;//如果按下的是当前输入框
								}
								if (_isInputMode && _selectFile != meta->GetFileName())
								{
									is_end_edit = true;
								}
								_selectFile = meta->GetFileName();
								EditorAppliction::SetInspectorType(InspectorType::Project, meta->GetFullPath());
							}
						}
					}
				}
			}
		}
		if (_isInputMode && Input::GetKeyDown(KeyCode::Return))
		{//按下enter键
			is_end_edit = true;
		}
		if (_isInputMode && is_end_edit)
		{//退出编辑模式
			_isInputMode = false;
			//重命名
			String new_name = sz_name_buff;
			if (new_name.Size() > 0 && new_name != old_select_file)
			{
				EditorAppliction::SetInspectorType(InspectorType::Project, "");
				File::Rename(Resource::GetFullDataPath(_selectPath + "/" + old_select_file), new_name);
			}
		}
	}

	//右键菜单
	if (ImGui::BeginPopupContextWindow())
	{
		ShowRightMenu(_selectFile);
		ImGui::EndPopup();
	}
}
bool EMain_Project::IsIgnoreExt(const String& ext)
{
	if (ext.Equals("meta", true) || ext.Equals("vs", true) || ext.Equals("gs", true) || ext.Equals("hs", true) 
		|| ext.Equals("ds", true) || ext.Equals("ps", true) || ext.Equals("cs", true))
		return true;
	return false;
}
void EMain_Project::ShowRightMenu(const String& node)
{
	if (ImGui::Selectable("Delete"))
	{
		if (!_selectFile.IsEmpty())
		{
			EditorAppliction::SetInspectorType(InspectorType::Project, "");

			String full_path = Path::Combine(_selectPath, _selectFile);
			File::Delete(Resource::GetFullDataPath(full_path));
		}
	}
	if (ImGui::Selectable("Open in Explorer"))
	{
		if (!_selectFile.IsEmpty())
		{
			String full_path = Path::Combine(_selectPath, _selectFile);
			Platform::OpeninExplorer(Resource::GetFullDataPath(full_path).c_str());
		}
	}
	ECommonComponent::ShowCreateAsset();
}
void EMain_Project::DragFileToEditor()
{
	const Vector<std::wstring>& files = EditorCursor::GetAddFiles();
	if (files.Size() > 0)
	{
		ImVec2 mouse_pos = ImGui::GetMousePos();
		ImVec2 pos = ImGui::GetWindowPos();
		ImVec2 size = ImGui::GetWindowSize();
		if (mouse_pos.x >= pos.x && mouse_pos.y >= pos.y && mouse_pos.x <= pos.x + size.x && mouse_pos.y <= pos.y + size.y)
		{
			String dst_path = Resource::GetFullDataPath(_selectPath);
			for (const auto& file : files)
			{
#if defined(DC_PLATFORM_WIN32)
				String src_path = String::FromWString(file);
				File::Copy(src_path, dst_path + "/" + Path::GetFileName(src_path));
#endif
			}
		}
	}
}
DC_END_NAMESPACE