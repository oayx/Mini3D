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
void EMain_Project::SetSelectFile(const String& filePath)
{
	if (filePath.IsEmpty())return;
	if (Resource::IsInternal(filePath))return;
	_selectPath = Path::GetDirectoryName(filePath);
	_selectFile = Path::GetFileName(filePath);
	_isInputMode = false;
}
void EMain_Project::Render()
{
	DC_PROFILE_FUNCTION;
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
void EMain_Project::ShowLeft(const Vector<AssetMeta*>& dirs, String selectPath)
{
	DC_PROFILE_FUNCTION;
	String folder = "";
	if (!selectPath.IsEmpty())
	{
		int index = selectPath.LastIndexOf('/');
		folder = selectPath.Substring(0, index);
		if (index >= 0)
			selectPath = selectPath.Substring(index + 1);
		else
			selectPath = "";
	}
	ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
	for (int i = 0; i < dirs.Size(); i++)
	{
		const AssetMeta* meta = dirs[i];
		if (!meta->IsFolder())continue;

		ImGuiTreeNodeFlags nodeFlags = baseFlags;
		const bool isSelected = _selectPath == meta->GetFullPath() && !_selectPath.IsEmpty() ? true : false;
		if (!folder.IsEmpty())
		{
			if (meta->GetFileName() == folder)nodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;
		}
		if (isSelected)nodeFlags |= ImGuiTreeNodeFlags_Selected;

		if (meta->HasChildrenFolder())
		{
			char szName[MAX_FILE_NAME] = { 0 };
			Snprintf(szName, ARRAY_SIZE(szName), "%s %s", ICON_FA_FOLDER, meta->GetFileName().c_str());
			bool node_open = (ImGui::TreeNodeEx(szName, nodeFlags));
			if (ImGui::IsItemClicked())
				_selectPath = meta->GetFullPath();
			if(node_open)
			{
				ShowLeft(meta->Children(), selectPath);
				ImGui::TreePop();
			}
		}
		else
		{//叶子
			char szName[MAX_FILE_NAME] = { 0 };
			Snprintf(szName, ARRAY_SIZE(szName), "%s %s", ICON_FA_FOLDER, meta->GetFileName().c_str());
			nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
			ImGui::TreeNodeEx(szName, nodeFlags);
			if (ImGui::IsItemClicked())
				_selectPath = meta->GetFullPath();
		}
	}
}
void EMain_Project::ShowRight()
{
	DC_PROFILE_FUNCTION;
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

			String childPath = _selectPath;
			while (childPath.Size() > 0)
			{
				int index = childPath.IndexOf('/');
				String name = childPath.Substring(0, index >= 0 ? index : -1);
				ImGui::SameLine();
				if (ImGui::SmallButton(name.c_str()))
				{
					int count = childPath.IndexOf('/');
					if (count < 0)count = childPath.Size();
					_selectPath = _selectPath.Substring(0, _selectPath.Size() - childPath.Size() + count).c_str();
				}

				if (index < 0)break;

				ImGui::SameLine();
				ImGui::Text(">");
				childPath = childPath.Substring(index + 1);
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
	const AssetMeta* currMeta = AssetsManager::GetMetaByPath(_selectPath);
	const float ICON_SIZE = ImGui::GetFont()->FontSize + 3;
	if(currMeta)
	{
		bool isEndEdit = false;
		static char szNameBuff[MAX_FILE_NAME] = { 0 };
		static String oldSelectFile = "";
		if (_isInputMode && (ImGui::IsMouseReleased(0) || ImGui::IsItemClicked(1)))
		{//左键松开或右键按下
			isEndEdit = true;
		}

		for (int showType = 0; showType < 2; ++showType)
		{
			for (int i = 0; i < currMeta->GetChildrenCount(); ++i)
			{
				const AssetMeta* meta = currMeta->GetChildren(i);
				bool isFolder = meta->IsFolder();
				if ((showType == 0 && meta->IsFolder()) ||
					(showType == 1 && meta->GetFileType() == FileInfoType::File && !IsIgnoreExt(meta->GetExtension())))
				{//先显示目录,再显示文件
					bool select = _selectFile == meta->GetFileName();
					if (EditorConfig::ProjectIconZoom > 1.2f)
					{//ICON模式
						_isInputMode = false;
						bool isSelect = ECommonComponent::ShowFileIcon(meta->GetFileName().c_str(), select, EditorIconCache::GetTexture(meta), Vector2(32 + 32 * EditorConfig::ProjectIconZoom, 32 + 32 * EditorConfig::ProjectIconZoom));
						if (isSelect)
						{
							_selectFile = meta->GetFileName();
							EditorAppliction::SetInspectorType(InspectorType::Project, meta->GetFullPath());

							if (ImGui::IsMouseDoubleClicked(0) && isFolder)
								_selectPath = meta->GetFullPath();
						}
						if (!isFolder && ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
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
								if (isFolder && ImGui::IsMouseDoubleClicked(0))
									_selectPath = meta->GetFullPath();
							}
						}
						else
						{
							ImGui::InputText("##GetFileName()", szNameBuff, ARRAY_SIZE(szNameBuff));
						}

						if (!isFolder && !_isInputMode && ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
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
								if ((ImGui::IsItemHovered(0) && ImGui::IsMouseReleased(0)) && _selectFile == meta->GetFileName() && !isFolder)
								{//进入编辑模式
									if (!_isInputMode)
									{
										_isInputMode = true;
										const String& name = meta->GetFileName();
										oldSelectFile = name;
										AssertEx(name.Size() < ARRAY_SIZE(szNameBuff), "");
										Memory::Clear(szNameBuff, ARRAY_SIZE(szNameBuff));
										Memory::Copy(szNameBuff, name.c_str(), name.Size());
									}
									isEndEdit = false;//如果按下的是当前输入框
								}
								if (_isInputMode && _selectFile != meta->GetFileName())
								{
									isEndEdit = true;
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
			isEndEdit = true;
		}
		if (_isInputMode && isEndEdit)
		{//退出编辑模式
			_isInputMode = false;
			//重命名
			String new_name = szNameBuff;
			if (new_name.Size() > 0 && new_name != oldSelectFile)
			{
				EditorAppliction::SetInspectorType(InspectorType::Project, "");
				File::Rename(Resource::GetFullDataPath(_selectPath + "/" + oldSelectFile), new_name);
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
	if (ext.Equals(".meta", true) || ext.Equals(".vs", true) || ext.Equals(".gs", true) || ext.Equals(".hs", true) 
		|| ext.Equals(".ds", true) || ext.Equals(".ps", true) || ext.Equals(".cs", true))
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

			String fullPath = Path::Combine(_selectPath, _selectFile);
			File::Delete(Resource::GetFullDataPath(fullPath));
		}
	}
	if (ImGui::Selectable("Open in Explorer"))
	{
		if (!_selectFile.IsEmpty())
		{
			String fullPath = Path::Combine(_selectPath, _selectFile);
			Platform::OpeninExplorer(Resource::GetFullDataPath(fullPath).c_str());
		}
	}
	ECommonComponent::ShowCreateAsset();
}
void EMain_Project::DragFileToEditor()
{
	const Vector<std::wstring>& files = EditorCursor::GetAddFiles();
	if (files.Size() > 0)
	{
		ImVec2 mousePos = ImGui::GetMousePos();
		ImVec2 pos = ImGui::GetWindowPos();
		ImVec2 size = ImGui::GetWindowSize();
		if (mousePos.x >= pos.x && mousePos.y >= pos.y && mousePos.x <= pos.x + size.x && mousePos.y <= pos.y + size.y)
		{
			String dstPath = Resource::GetFullDataPath(_selectPath);
			for (const auto& file : files)
			{
#if defined(DC_PLATFORM_WIN32)
				String srcPath = String::FromWString(file);
				File::Copy(srcPath, dstPath + "/" + Path::GetFileName(srcPath));
#endif
			}
		}
	}
}
DC_END_NAMESPACE