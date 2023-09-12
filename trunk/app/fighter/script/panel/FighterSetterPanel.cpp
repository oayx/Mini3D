#include "FighterSetterPanel.h"
#include "gui/UIManager.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
void FighterSetterPanel::Awake()
{
	GameObject* bg_obj = GameObject::InstantiationUI("BG");
	bg_obj->GetTransform()->SetParent(GetTransform(), false);
	UIImage* img_bg = bg_obj->AddComponent<UIImage>();
	img_bg->SetAtlas("atlas/compnent.png", "ui_bg_3.png");			
	img_bg->SetSize(300, 400);
	img_bg->SetImageType(UIImageType::Sliced);
	img_bg->SetImageBorder(Vector4(10, 10, 10, 10));

	{
		GameObject* btn_obj = GameObject::InstantiationUI("BtnClose");
		btn_obj->GetTransform()->SetParent(bg_obj->GetTransform(), false);
		btn_obj->GetComponent<Transform>()->SetLocalPosition(Vector3(0, 100, 0));
		UIImage* btn = btn_obj->AddComponent<UIImage>();
		btn->SetAtlas("atlas/compnent.png", "ui_button_2.png");
		btn->SetNativeSize();
		btn->SetEventListener(UIEventType::LMouseDown, [this](const UIEventInfo& info)
			{
			});

		GameObject* label_obj = GameObject::InstantiationUI("Label");
		label_obj->GetTransform()->SetParent(btn_obj->GetTransform(), false);
		UILabel* label = label_obj->AddComponent<UILabel>();
		label->SetText("Sound off");
		label->SetFontSize(28);
		label->SetColor(Color::Black);
		label->SetHorizontalOverflow(UIHorizontalWrapMode::Overflow);
		label->SetAlignment(UITextAnchor::MiddleCenter);
	}

	{
		GameObject* btn_obj = GameObject::InstantiationUI("BtnClose");
		btn_obj->GetTransform()->SetParent(bg_obj->GetTransform(), false);
		btn_obj->GetComponent<Transform>()->SetLocalPosition(Vector3(0, 0, 0));
		UIImage* btn = btn_obj->AddComponent<UIImage>();
		btn->SetAtlas("atlas/compnent.png", "ui_button_2.png");
		btn->SetNativeSize();
		btn->SetEventListener(UIEventType::LMouseDown, [this](const UIEventInfo& info)
			{
			});

		GameObject* label_obj = GameObject::InstantiationUI("Label");
		label_obj->GetTransform()->SetParent(btn_obj->GetTransform(), false);
		UILabel* label = label_obj->AddComponent<UILabel>();
		label->SetText("Set quality");
		label->SetFontSize(28);
		label->SetColor(Color::Black);
		label->SetHorizontalOverflow(UIHorizontalWrapMode::Overflow);
		label->SetAlignment(UITextAnchor::MiddleCenter);
	}

	{
		GameObject* btn_obj = GameObject::InstantiationUI("BtnClose");
		btn_obj->GetTransform()->SetParent(bg_obj->GetTransform(), false);
		btn_obj->GetComponent<Transform>()->SetLocalPosition(Vector3(0, -100, 0));
		UIImage* btn = btn_obj->AddComponent<UIImage>();
		btn->SetAtlas("atlas/compnent.png", "ui_button_2.png");
		btn->SetNativeSize();
		btn->SetEventListener(UIEventType::LMouseDown, [this](const UIEventInfo& info)
			{
				UIManager::Instance()->Close<FighterSetterPanel>();
			});

		GameObject* label_obj = GameObject::InstantiationUI("Label");
		label_obj->GetTransform()->SetParent(btn_obj->GetTransform(), false);
		UILabel* label = label_obj->AddComponent<UILabel>();
		label->SetText("Return");
		label->SetFontSize(28);
		label->SetColor(Color::Red);
		label->SetHorizontalOverflow(UIHorizontalWrapMode::Overflow);
		label->SetAlignment(UITextAnchor::MiddleCenter);
	}
}
void FighterSetterPanel::RegisterEvent()
{
}
void FighterSetterPanel::UnRegisterEvent()
{
}
DC_END_NAMESPACE
