#include "FighterResultPanel.h"
#include "gui/UIManager.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
void FighterResultPanel::Awake()
{
	GameObject* bg_obj = GameObject::InstantiationUI("BG");
	bg_obj->GetTransform()->SetParent(GetTransform(), false);
	UIImage* img_bg = bg_obj->AddComponent<UIImage>();
	img_bg->SetAtlas("atlas/compnent.png", "ui_bg_3.png");
	img_bg->SetNativeSize();

	{
		GameObject* label_obj = GameObject::InstantiationUI("Label");
		label_obj->GetTransform()->SetParent(bg_obj->GetTransform(), false);
		UILabel* label = label_obj->AddComponent<UILabel>();
		label->SetText("Game over, restart or not?");
		label->SetFontSize(30);
		label->SetColor(Color::Black);
		label->SetSize(300, 200);
	}

	{
		GameObject* btn_left_obj = GameObject::InstantiationUI("BtnLeft");
		btn_left_obj->GetTransform()->SetParent(bg_obj->GetTransform(), false);
		btn_left_obj->GetComponent<Transform>()->SetLocalPosition(Vector3(-70, -85, 0));

		UIImage* btn_left = btn_left_obj->AddComponent<UIImage>();
		btn_left->SetAtlas("atlas/compnent.png", "ui_14.png");
		btn_left->SetNativeSize();
		btn_left->SetEventListener(UIEventType::LMouseDown, [this](const UIEventInfo& info)
			{
				Application::Quit();
			});

		GameObject* btn_right_obj = GameObject::InstantiationUI("BtnRight");
		btn_right_obj->GetTransform()->SetParent(bg_obj->GetTransform(), false);
		btn_right_obj->GetComponent<Transform>()->SetLocalPosition(Vector3(70, -85, 0));

		UIImage* btn_right = btn_right_obj->AddComponent<UIImage>();
		btn_right->SetAtlas("atlas/compnent.png", "ui_13.png");
		btn_right->SetNativeSize();
		btn_right->SetEventListener(UIEventType::LMouseDown, [this](const UIEventInfo& info)
			{
				EventDispatcher::TriggerEvent(Evt_Restart);
				UIManager::Instance()->Close<FighterResultPanel>();
			});
	}
}
void FighterResultPanel::RegisterEvent()
{
}
void FighterResultPanel::UnRegisterEvent()
{
}
DC_END_NAMESPACE
