#include "UIButton.h"
#include "editor/common/ECommonComponent.h"
#include "editor/EditorAppliction.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(UIButton, UIImage);
Object* UIButton::Clone(Object* new_obj)
{
	base::Clone(new_obj);
	UIButton* obj = dynamic_cast<UIButton*>(new_obj);
	if (!obj)return new_obj;

	obj->SetNormalColor(_normalColor);
	obj->SetPressColor(_pressColor);
	obj->SetDeactiveColor(_deactiveColor);
	obj->SetDeactiveColor(_active);

	return obj;
}
void UIButton::Awake() 
{
	base::Awake(); 
	SetColor(_normalColor); 
	SetAtlas("internal/atlas/ui.png", "UISprite.png");
}
void UIButton::OnMouseDown(const Vector2& pos, MouseBtnID btn)
{
	if (_active)
	{
		base::OnMouseDown(pos, btn);
		SetColor(_pressColor);
	}
}
void UIButton::OnMouseUpInside(const Vector2& pos, MouseBtnID btn)
{
	if (_active)
	{
		base::OnMouseUpInside(pos, btn);
		SetColor(_normalColor);
	}
}
void UIButton::OnMouseUpOutside(const Vector2& pos, MouseBtnID btn)
{
	if (_active)
	{
		base::OnMouseUpOutside(pos, btn);
		SetColor(_normalColor);
	}
}
void UIButton::OnMouseClick(const Vector2& pos, MouseBtnID btn)
{
	if (_active) 
	{
		base::OnMouseClick(pos, btn);
	}
}
void UIButton::OnDrawEditor()
{
	UIView::OnDrawEditor();

	ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;
	if (ImGui::TreeNodeEx("Image", base_flags))
	{
		UIImage::OnDrawEditor_Image();
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
}
void UIButton::OnDrawEditor_Button()
{
	{
		ImGuiEx::Label("Normal Color");
		if (ECommonComponent::ShowColor("NormalColor", _normalColor.p, false))
		{
			SetNormalColor(_normalColor);
		}
	}
	{
		ImGuiEx::Label("Press Color");
		if (ECommonComponent::ShowColor("PressColor", _pressColor.p, false))
		{
			SetNormalColor(_pressColor);
		}
	}
	{
		ImGuiEx::Label("Deactive Color");
		if (ECommonComponent::ShowColor("DeactiveColor", _deactiveColor.p, false))
		{
			SetNormalColor(_deactiveColor);
		}
	}
}
/********************************************************************/
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(UIButton);
template<class TransferFunction> inline
void UIButton::Transfer(TransferFunction& transfer, void* ptr)
{
	base::Transfer(transfer, ptr);

	TRANSFER_SIMPLE(_active);
	TRANSFER_SIMPLE(_normalColor);
	TRANSFER_SIMPLE(_pressColor);
	TRANSFER_SIMPLE(_deactiveColor);

	if (transfer.IsRead())
	{
		SetActive(_active);
	}
}
DC_END_NAMESPACE
