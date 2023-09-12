#include "UICheckbox.h"
#include "UIAtlas.h"
#include "runtime/resources/AssetsManager.h"
#include "editor/common/EAssetSelect.h"
#include "editor/common/ECommonComponent.h"
#include "editor/main/EMain_Project.h"
#include "editor/EditorAppliction.h"
#include "editor/EditorCursor.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(UICheckbox, UIButton);
Object* UICheckbox::Clone(Object* new_obj)
{
	base::Clone(new_obj);
	UICheckbox* obj = dynamic_cast<UICheckbox*>(new_obj);
	if (!obj)return new_obj;

	obj->SetNormalAtlas(_normalAtlasFile, _normalAtlasName);
	obj->SetSelectAtlas(_selectAtlasFile, _selectAtlasName);
	obj->SetChecked(_isChecked);

	return obj;
}
void UICheckbox::SetNormalAtlas(const String& file, const String& name)
{
	_normalAtlasFile = file;
	_normalAtlasName = name;
}
void UICheckbox::SetSelectAtlas(const String& file, const String& name)
{
	_selectAtlasFile = file;
	_selectAtlasName = name;
}
void UICheckbox::OnMouseClick(const Vector2& pos, MouseBtnID btn)
{
	base::OnMouseClick(pos, btn);
	SetChecked(!_isChecked);
}
void UICheckbox::SetChecked(bool b)
{
	_isChecked = b;
	if (_isChecked)
	{
		base::SetAtlas(_selectAtlasFile, _selectAtlasName);
	}
	else
	{
		base::SetAtlas(_normalAtlasFile, _normalAtlasName);
	}
}
void UICheckbox::OnDrawEditor()
{
	UIView::OnDrawEditor();

	ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;
	if (ImGui::TreeNodeEx("Image", base_flags))
	{
		UIView::OnDrawEditor_Color();
		UIImage::OnDrawEditor_ImageType();
		UIView::OnDrawEditor_RaycastTarget();

		ImGui::TreePop();
	}
	ImGui::NewLine();
	if (ImGui::TreeNodeEx("Button", base_flags))
	{
		UIButton::OnDrawEditor_Button();
		ImGui::TreePop();
	}
	ImGui::NewLine();
	if (ImGui::TreeNodeEx("Checkbox", base_flags))
	{
		{
			ImGuiEx::Label("Checked");
			if (ImGui::Checkbox("##IsChecked", &_isChecked))
			{
				SetChecked(_isChecked);
			}
		}
		{
			ImGui::PushID(1);
			{
				ImGuiEx::Label("Normal Altas");
				const char* sz_name = _normalAtlasFile.c_str();
				ImGui::Button(sz_name, ImVec2(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(ICON_FA_LINK).x - 5, 0));
				if (ImGui::IsItemClicked(0) && !_normalAtlasFile.IsEmpty())
				{
					EMain_Project::SetSelectFile(_normalAtlasFile);
				}
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ProjectAsset"))
					{
						const String& file_path = EditorCursor::GetDragFile();
						if (UIAtlasManager::IsAtlasFile(file_path))
						{
							_normalAtlasFile = file_path;
							if(!_isChecked)base::SetAtlas(_normalAtlasFile, _normalAtlasName);
						}
						EditorCursor::EndDrag();
					}
					ImGui::EndDragDropTarget();
				}
				ImGui::SameLine();
				if (ImGui::Selectable(ICON_FA_LINK, false, 0, ImGui::CalcTextSize(ICON_FA_LINK)))
				{
					auto OnSelectAsset = [this](const String& file)
					{
						_normalAtlasFile = file;
						if (!_isChecked)base::SetAtlas(_normalAtlasFile, _normalAtlasName);
					};

					EAssetSelect::Search(ResourceType::SpriteAtlas, OnSelectAsset);
				}
			}
			{
				int select_index = -1;
				VecString altas_names;
				UIAtlas* ui_atlas = UIAtlasManager::GetAtlas(_normalAtlasFile);
				if (ui_atlas)
				{
					const Map<String, UIAtlasInfo>& atlas_infos = ui_atlas->GetAtlasInfoes();
					for (const auto& obj : atlas_infos)
					{
						altas_names.Add(obj.first);
						if (obj.first == _normalAtlasName)
						{
							select_index = altas_names.Size() - 1;
						}
					}
				}

				ImGuiEx::Label("Normal Image");
				if (ECommonComponent::ShowDropDown("Normal Image", altas_names, select_index))
				{
					_normalAtlasName = altas_names[select_index];
					if (!_isChecked)base::SetAtlas(_normalAtlasFile, _normalAtlasName);
				}
			}
			ImGui::PopID();
		}
		{
			ImGui::PushID(2);
			{
				ImGuiEx::Label("Select Altas");
				const char* sz_name = _selectAtlasFile.c_str();
				ImGui::Button(sz_name, ImVec2(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(ICON_FA_LINK).x - 5, 0));
				if (ImGui::IsItemClicked(0) && !_selectAtlasFile.IsEmpty())
				{
					EMain_Project::SetSelectFile(_selectAtlasFile);
				}
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ProjectAsset"))
					{
						const String& file_path = EditorCursor::GetDragFile();
						if (UIAtlasManager::IsAtlasFile(file_path))
						{
							_selectAtlasFile = file_path;
							if (_isChecked)base::SetAtlas(_selectAtlasFile, _selectAtlasName);
						}
						EditorCursor::EndDrag();
					}
					ImGui::EndDragDropTarget();
				}
				ImGui::SameLine();
				if (ImGui::Selectable(ICON_FA_LINK, false, 0, ImGui::CalcTextSize(ICON_FA_LINK)))
				{
					auto OnSelectAsset = [this](const String& file)
					{
						_selectAtlasFile = file;
						if (_isChecked)base::SetAtlas(_selectAtlasFile, _selectAtlasName);
					};

					EAssetSelect::Search(ResourceType::SpriteAtlas, OnSelectAsset);
				}
			}
			{
				int select_index = -1;
				VecString altas_names;
				UIAtlas* ui_atlas = UIAtlasManager::GetAtlas(_selectAtlasFile);
				if (ui_atlas)
				{
					const Map<String, UIAtlasInfo>& atlas_infos = ui_atlas->GetAtlasInfoes();
					for (const auto& obj : atlas_infos)
					{
						altas_names.Add(obj.first);
						if (obj.first == _selectAtlasName)
						{
							select_index = altas_names.Size() - 1;
						}
					}
				}

				ImGuiEx::Label("Select Image");
				if (ECommonComponent::ShowDropDown("Select Image", altas_names, select_index))
				{
					_selectAtlasName = altas_names[select_index];
					if (_isChecked)base::SetAtlas(_selectAtlasFile, _selectAtlasName);
				}
			}
			ImGui::PopID();
		}
		ImGui::TreePop();
	}
}
/********************************************************************/
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(UICheckbox);
template<class TransferFunction> inline
void UICheckbox::Transfer(TransferFunction& transfer, void* ptr)
{
	base::Transfer(transfer, ptr);

	TRANSFER_SIMPLE(_isChecked);
	TRANSFER_SIMPLE(_normalAtlasFile);
	TRANSFER_SIMPLE(_normalAtlasName);
	TRANSFER_SIMPLE(_selectAtlasFile);
	TRANSFER_SIMPLE(_selectAtlasName);

	if (transfer.IsRead())
	{
		SetChecked(_isChecked);
	}
}
DC_END_NAMESPACE
