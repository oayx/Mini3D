#include "UINormalButton.h"
#include "UIAtlas.h"
#include "runtime/resources/AssetsManager.h"
#include "editor/common/EAssetSelect.h"
#include "editor/common/ECommonComponent.h"
#include "editor/main/EMain_Project.h"
#include "editor/EditorAppliction.h"
#include "editor/EditorCursor.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(UINormalButton, UIButton);
Object* UINormalButton::Clone(Object* newObj)
{
	base::Clone(newObj);
	UINormalButton* obj = dynamic_cast<UINormalButton*>(newObj);
	if (!obj)return newObj;

	obj->SetNormalAtlas(_normalAtlasFile, _normalAtlasName);
	obj->SetSelectAtlas(_selectAtlasFile, _selectAtlasName);
	obj->SetDeactiveAtlas(_deactiveAtlasFile, _deactiveAtlasName);
	obj->SetStatus(_status);

	return obj;
}
void UINormalButton::SetNormalAtlas(const String& file, const String& name) 
{ 
	_normalAtlasFile = file; 
	_normalAtlasName = name; 
}
void UINormalButton::SetSelectAtlas(const String& file, const String& name) 
{ 
	_selectAtlasFile = file; 
	_selectAtlasName = name;
}
void UINormalButton::SetDeactiveAtlas(const String& file, const String& name) 
{ 
	_deactiveAtlasFile = file; 
	_deactiveAtlasName = name;
}
void UINormalButton::SetStatus(UIButtonStatus status)
{
	switch (status)
	{
	case dc::UIButtonStatus::Normal:
		if (!_normalAtlasFile.IsEmpty() && !_normalAtlasName.IsEmpty())
		{
			base::SetAtlas(_normalAtlasFile, _normalAtlasName);
		}
		break;
	case dc::UIButtonStatus::Select:
		if (!_selectAtlasFile.IsEmpty() && !_selectAtlasName.IsEmpty())
		{
			base::SetAtlas(_selectAtlasFile, _selectAtlasName);
		}
		break;
	case dc::UIButtonStatus::Deactive:
		if (!_deactiveAtlasFile.IsEmpty() && !_deactiveAtlasName.IsEmpty())
		{
			base::SetAtlas(_deactiveAtlasFile, _deactiveAtlasName);
		}
		break;
	default:
		break;
	}
	_status = status;
}
void UINormalButton::SetActive(bool b)
{
	base::SetActive(b);
	if (!_active)
		this->SetStatus(UIButtonStatus::Deactive);
	else
		this->SetStatus(UIButtonStatus::Normal);
}
void UINormalButton::OnMouseDown(const Vector2& pos, MouseBtnID btn)
{
	base::OnMouseDown(pos, btn);
	if (_active) 
	{
		SetStatus(UIButtonStatus::Select);
	}
}
void UINormalButton::OnMouseUpInside(const Vector2& pos, MouseBtnID btn)
{
	base::OnMouseUpInside(pos, btn);
	if (_active)
	{
		SetStatus(UIButtonStatus::Normal);
	}
}
void UINormalButton::OnMouseUpOutside(const Vector2& pos, MouseBtnID btn)
{
	base::OnMouseUpOutside(pos, btn);
	if (_active)
	{
		SetStatus(UIButtonStatus::Normal);
	}
}
void UINormalButton::OnDrawEditor()
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
	if (ImGui::TreeNodeEx("NormalButton", baseFlags))
	{
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
							if (_status == UIButtonStatus::Normal)base::SetAtlas(_normalAtlasFile, _normalAtlasName);
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
						if (_status == UIButtonStatus::Normal)base::SetAtlas(_normalAtlasFile, _normalAtlasName);
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
					if (_status == UIButtonStatus::Normal)base::SetAtlas(_normalAtlasFile, _normalAtlasName);
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
							if (_status == UIButtonStatus::Select)base::SetAtlas(_selectAtlasFile, _selectAtlasName);
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
						if (_status == UIButtonStatus::Select)base::SetAtlas(_selectAtlasFile, _selectAtlasName);
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
					if (_status == UIButtonStatus::Select)base::SetAtlas(_selectAtlasFile, _selectAtlasName);
				}
			}
			ImGui::PopID();
		}
		{
			ImGui::PushID(3);
			{
				ImGuiEx::Label("Deactive Altas");
				const char* szName = _deactiveAtlasFile.c_str();
				ImGui::Button(szName, ImVec2(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(ICON_FA_LINK).x - 5, 0));
				if (ImGui::IsItemClicked(0) && !_deactiveAtlasFile.IsEmpty())
				{
					EMain_Project::SetSelectFile(_deactiveAtlasFile);
				}
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ProjectAsset"))
					{
						const String& filePath = EditorCursor::GetDragFile();
						if (UIAtlasManager::IsAtlasFile(filePath))
						{
							_deactiveAtlasFile = filePath;
							if (_status == UIButtonStatus::Deactive)base::SetAtlas(_deactiveAtlasFile, _deactiveAtlasName);
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
						_deactiveAtlasFile = file;
						if (_status == UIButtonStatus::Deactive)base::SetAtlas(_deactiveAtlasFile, _deactiveAtlasName);
					};

					EAssetSelect::Search(ResourceType::SpriteAtlas, OnSelectAsset);
				}
			}
			{
				int selectIndex = -1;
				VecString altasNames;
				UIAtlas* uiAtlas = UIAtlasManager::GetAtlas(_deactiveAtlasFile);
				if (uiAtlas)
				{
					const Map<String, UIAtlasInfo>& atlas_infos = uiAtlas->GetAtlasInfoes();
					for (const auto& obj : atlas_infos)
					{
						altasNames.Add(obj.first);
						if (obj.first == _deactiveAtlasName)
						{
							selectIndex = altasNames.Size() - 1;
						}
					}
				}

				ImGuiEx::Label("Deactive Image");
				if (ECommonComponent::ShowDropDown("Deactive Image", altasNames, selectIndex))
				{
					_deactiveAtlasName = altasNames[selectIndex];
					if (_status == UIButtonStatus::Deactive)base::SetAtlas(_deactiveAtlasFile, _deactiveAtlasName);
				}
			}
			ImGui::PopID();
		}
		ImGui::TreePop();
	}
}
/********************************************************************/
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(UINormalButton);
template<class TransferFunction> inline
void UINormalButton::Transfer(TransferFunction& transfer, void* ptr)
{
	base::Transfer(transfer, ptr);

	TRANSFER_ENUM(_status);
	TRANSFER_SIMPLE(_normalAtlasFile);
	TRANSFER_SIMPLE(_normalAtlasName);
	TRANSFER_SIMPLE(_selectAtlasFile);
	TRANSFER_SIMPLE(_selectAtlasName);
	TRANSFER_SIMPLE(_deactiveAtlasFile);
	TRANSFER_SIMPLE(_deactiveAtlasName);

	if (transfer.IsRead())
	{
		SetStatus(_status);
	}
}
DC_END_NAMESPACE