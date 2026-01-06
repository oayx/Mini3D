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
Object* UICheckbox::Clone(Object* newObj)
{
	base::Clone(newObj);
	UICheckbox* obj = dynamic_cast<UICheckbox*>(newObj);
	if (!obj)return newObj;

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

	ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;
	if (ImGui::TreeNodeEx("Image", baseFlags))
	{
		UIView::OnDrawEditor_Color();
		UIImage::OnDrawEditor_ImageType();
		UIView::OnDrawEditor_RaycastTarget();

		ImGui::TreePop();
	}
	ImGui::NewLine();
	if (ImGui::TreeNodeEx("Button", baseFlags))
	{
		UIButton::OnDrawEditor_Button();
		ImGui::TreePop();
	}
	ImGui::NewLine();
	if (ImGui::TreeNodeEx("Checkbox", baseFlags))
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
				const char* szName = _normalAtlasFile.c_str();
				ImGui::Button(szName, ImVec2(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(ICON_FA_LINK).x - 5, 0));
				if (ImGui::IsItemClicked(0) && !_normalAtlasFile.IsEmpty())
				{
					EMain_Project::SetSelectFile(_normalAtlasFile);
				}
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ProjectAsset"))
					{
						const String& filePath = EditorCursor::GetDragFile();
						if (UIAtlasManager::IsAtlasFile(filePath))
						{
							_normalAtlasFile = filePath;
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
				int selectIndex = -1;
				VecString altasNames;
				UIAtlas* uiAtlas = UIAtlasManager::GetAtlas(_normalAtlasFile);
				if (uiAtlas)
				{
					const Map<String, UIAtlasInfo>& atlas_infos = uiAtlas->GetAtlasInfoes();
					for (const auto& obj : atlas_infos)
					{
						altasNames.Add(obj.first);
						if (obj.first == _normalAtlasName)
						{
							selectIndex = altasNames.Size() - 1;
						}
					}
				}

				ImGuiEx::Label("Normal Image");
				if (ECommonComponent::ShowDropDown("Normal Image", altasNames, selectIndex))
				{
					_normalAtlasName = altasNames[selectIndex];
					if (!_isChecked)base::SetAtlas(_normalAtlasFile, _normalAtlasName);
				}
			}
			ImGui::PopID();
		}
		{
			ImGui::PushID(2);
			{
				ImGuiEx::Label("Select Altas");
				const char* szName = _selectAtlasFile.c_str();
				ImGui::Button(szName, ImVec2(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(ICON_FA_LINK).x - 5, 0));
				if (ImGui::IsItemClicked(0) && !_selectAtlasFile.IsEmpty())
				{
					EMain_Project::SetSelectFile(_selectAtlasFile);
				}
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ProjectAsset"))
					{
						const String& filePath = EditorCursor::GetDragFile();
						if (UIAtlasManager::IsAtlasFile(filePath))
						{
							_selectAtlasFile = filePath;
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
				int selectIndex = -1;
				VecString altasNames;
				UIAtlas* uiAtlas = UIAtlasManager::GetAtlas(_selectAtlasFile);
				if (uiAtlas)
				{
					const Map<String, UIAtlasInfo>& atlas_infos = uiAtlas->GetAtlasInfoes();
					for (const auto& obj : atlas_infos)
					{
						altasNames.Add(obj.first);
						if (obj.first == _selectAtlasName)
						{
							selectIndex = altasNames.Size() - 1;
						}
					}
				}

				ImGuiEx::Label("Select Image");
				if (ECommonComponent::ShowDropDown("Select Image", altasNames, selectIndex))
				{
					_selectAtlasName = altasNames[selectIndex];
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
