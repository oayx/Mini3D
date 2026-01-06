 #pragma once

#include "TestBase.h"
#include <array>

DC_BEGIN_NAMESPACE
class TestUI : public TestBase
{
	typedef TestBase base;

public:
	TestUI()
	{
		std::function<void()> fun = CALLBACK_0(TestUI::TestImage, this);
		_menus.Add(std::make_pair("Image", fun));

		fun = CALLBACK_0(TestUI::TestProgress, this);
		_menus.Add(std::make_pair("Progress", fun));

		fun = CALLBACK_0(TestUI::TestHeadImage, this);
		_menus.Add(std::make_pair("HeadImage", fun));

		fun = CALLBACK_0(TestUI::TestLable, this);
		_menus.Add(std::make_pair("Lable", fun));

		fun = CALLBACK_0(TestUI::TestInputField, this);
		_menus.Add(std::make_pair("InputField", fun));

		fun = CALLBACK_0(TestUI::TestButton, this);
		_menus.Add(std::make_pair("Button", fun));

		fun = CALLBACK_0(TestUI::TestAnimation, this);
		_menus.Add(std::make_pair("Animation", fun));
	}

	virtual void OnEnter(int child_unit_index)override
	{
		base::OnEnter(child_unit_index);
		GameObject* ui_root = GameObject::Find("UIRoot");
		_rootObject = GameObject::Instantiation("UI");
		_rootObject->GetTransform()->SetParent(ui_root->GetTransform(), false);
	}
	virtual void OnLeave()override
	{
		base::OnLeave();
	}
	virtual String GetTitle()override { return "UI"; }
	void TestImage()
	{
		GameObject* obj1 = GameObject::Instantiation("PanelRoot");
		this->AddGameObject(obj1);

		UIImage* image1 = obj1->AddComponent<UIImage>();
		image1->SetAtlas("atlas/common.png", "btn_card_filter02.png");
		image1->SetSize(400, 300);
		image1->SetImageType(UIImageType::Sliced);
		image1->SetImageBorder(Vector4(10, 10, 10, 10));
		image1->SetEventListener(UIEventType::LMouseDown, [this](const UIEventInfo& info)
			{

			});

		{
			GameObject* obj2 = GameObject::Instantiation("Title");
			obj2->GetTransform()->SetParent(obj1->GetTransform(), false);

			UIImage* image2 = obj2->AddComponent<UIImage>();
			image2->SetAtlas("atlas/common.png", "btn_card_filter01.png");
			image2->SetNativeSize();
			image2->SetEventListener(UIEventType::LMouseDown, [this](const UIEventInfo& info)
				{
					Debuger::Log("LMouseDown");
				});
			obj2->GetComponent<Transform>()->SetLocalPosition(Vector3(0, -40, 0));
			obj2->GetRectTransform()->SetAlige(AligeType::Top);
		}
	}
	void TestProgress()
	{
		{
			GameObject* obj1 = GameObject::Instantiation("HProgress");
			this->AddGameObject(obj1);
			obj1->GetTransform()->SetLocalPosition(Vector3(0, 80, 0));

			UIImage* image1 = obj1->AddComponent<UIImage>();
			image1->SetAtlas("atlas/common.png", "btn_card_filter03.png");
			image1->SetNativeSize();
			image1->SetImageType(UIImageType::Filled);
			image1->SetFillMethod(UIImageFillMethod::Horizontal, (int)UIImageOriginHorizontal::Right);
			uint timer_id = Timer::AddLoop(0.1f, [image1]()->void
				{
					float amount = image1->GetFillAmount() + 0.1f;
					if (amount > 1)
						amount -= 1;
					image1->SetFillAmount(amount);
				});
			_timers.Add(timer_id);
		}
		{
			GameObject* obj1 = GameObject::Instantiation("VProgress");
			this->AddGameObject(obj1);
			obj1->GetTransform()->SetLocalPosition(Vector3(0, 150, 0));

			UIImage* image1 = obj1->AddComponent<UIImage>();
			image1->SetAtlas("atlas/common.png", "btn_card_filter03.png");
			image1->SetNativeSize();
			image1->SetImageType(UIImageType::Filled);
			image1->SetFillMethod(UIImageFillMethod::Vertical, (int)UIImageOriginVertical::Bottom);
			uint timer_id = Timer::AddLoop(0.1f, [image1]()->void
				{
					float amount = image1->GetFillAmount() + 0.1f;
					if (amount > 1)
						amount -= 1;
					image1->SetFillAmount(amount);
				});
			_timers.Add(timer_id);
		}
	}
	void TestHeadImage()
	{
		{
			GameObject* obj1 = GameObject::Instantiation("HeadRoot");
			this->AddGameObject(obj1);
			obj1->GetTransform()->SetLocalPosition(Vector3(300, 300, 0));

			UIImage* image1 = obj1->AddComponent<UIImage>();
			image1->SetAtlas("atlas/head.png", "frame_head.png");
			image1->SetNativeSize();

			//{
			//	GameObject* obj2 = GameObject::Instantiation("Head");
			//	UIImage* image2 = obj2->AddComponent<UIImage>();
			//	image2->SetAtlas("atlas/head.png", "icon_head.png");
			//	image2->SetNativeSize();
			//	Transform* node2 = obj2->GetTransform();
			//	node2->SetParent(obj1->GetTransform(), false);
			//	node2->SetLocalPosition(Vector3(6, 5, 0));
			//}
		}
	}
	void TestLable()
	{
		{
			GameObject* obj = GameObject::Instantiation("FontRoot1");
			this->AddGameObject(obj);
			//obj1->GetTransform()->SetLocalPosition(Vector3(0, -250, 0));

			UIImage* image1 = obj->AddComponent<UIImage>();
			image1->SetAtlas("atlas/common.png", "btn_card_filter02.png");
			image1->SetSize(200, 200);
			image1->SetImageType(UIImageType::Sliced);
			image1->SetImageBorder(Vector4(10, 10, 10, 10));
		}
		{
			GameObject* obj = GameObject::Instantiation("FontRoot2");
			this->AddGameObject(obj);

			UILabel* label1 = obj->AddComponent<UILabel>();
			label1->SetText("1234567890qwe");//1234567890qwertyuiopasdfghjklzxcvbnm
			label1->SetFontSize(40);
			label1->SetColor(Color::Black);
			label1->SetSize(200, 200);
			label1->SetWordSpace(0);
			label1->SetLineSpace(5);
			//label1->SetFontStyle(UIFontStyle::BoldAndItalic);
			label1->SetAlignment(UITextAnchor::MiddleCenter);
			label1->SetHorizontalOverflow(UIHorizontalWrapMode::Wrap);
			label1->SetVerticalOverflow(UIVerticalWrapMode::Overflow);
			//UITextOutline* outline = obj2->AddComponent<UITextOutline>();
			//outline->color = Color(0, 0, 1, 0.5f);
			//outline->distanceX = 1;
			//outline->distanceY = 1;
			//UITextShadow* outline = obj2->AddComponent<UITextShadow>();
			//outline->color = Color(0, 0, 1, 0.5f);
			//outline->distanceX = 1;
			//outline->distanceY = -1;
		}
	}
	void TestInputField()
	{
		{
			GameObject* obj = GameObject::Instantiation("InputField");
			this->AddGameObject(obj);
			UIInputField* input = obj->AddComponent<UIInputField>();
			input->SetAtlas("atlas/common.png", "btn_card_filter02.png");
			input->SetSize(200, 100);
			input->SetImageType(UIImageType::Sliced);
			input->SetImageBorder(Vector4(10, 10, 10, 10));
			//input->SetText("1234567890qwer");//1234567890qwertyuiopasdfghjklzxcvbnm
			input->SetFontSize(30);
			input->SetFontColor(Color::Black);
			input->SetContentSize(Size(190, 80));
		}
	}
	void TestButton()
	{
		{
			GameObject* obj = GameObject::Instantiation("NormalButton");
			this->AddGameObject(obj);
			UINormalButton* btn = obj->AddComponent<UINormalButton>();
			btn->SetNormalAtlas("atlas/common.png", "btn_common_confirm.png");
			btn->SetSelectAtlas("atlas/common.png", "btn_common_cancel.png");
			btn->SetDeactiveAtlas("atlas/common.png", "btn_common_ash.png");
			//btn->SetActive(false);
			btn->SetEventListener(UIEventType::LMouseClick, [](const UIEventInfo& info) {Debuger::Log("LMouseClick"); });
			btn->SetEventListener(UIEventType::RMouseClick, [](const UIEventInfo& info) {Debuger::Log("RMouseClick"); });
			btn->SetEventListener(UIEventType::MMouseClick, [](const UIEventInfo& info) {Debuger::Log("MMouseClick"); });
			btn->SetEventListener(UIEventType::LMouseDown, [](const UIEventInfo& info) {Debuger::Log("LMouseDown"); });
			btn->SetEventListener(UIEventType::RMouseDown, [](const UIEventInfo& info) {Debuger::Log("RMouseDown"); });
			btn->SetEventListener(UIEventType::MMouseDown, [](const UIEventInfo& info) {Debuger::Log("MMouseDown"); });
			btn->SetEventListener(UIEventType::LMouseUpInside, [](const UIEventInfo& info) {Debuger::Log("LMouseUpInside"); });
			btn->SetEventListener(UIEventType::RMouseUpInside, [](const UIEventInfo& info) {Debuger::Log("RMouseUpInside"); });
			btn->SetEventListener(UIEventType::MMouseUpInside, [](const UIEventInfo& info) {Debuger::Log("MMouseUpInside"); });
			btn->SetEventListener(UIEventType::LMouseUpOutside, [](const UIEventInfo& info) {Debuger::Log("LMouseUpOutside"); });
			btn->SetEventListener(UIEventType::RMouseUpOutside, [](const UIEventInfo& info) {Debuger::Log("RMouseUpOutside"); });
			btn->SetEventListener(UIEventType::MMouseUpOutside, [](const UIEventInfo& info) {Debuger::Log("MMouseUpOutside"); });
		}
		{
			GameObject* obj = GameObject::Instantiation("UICheckbox");
			this->AddGameObject(obj);
			UICheckbox* btn = obj->AddComponent<UICheckbox>();
			btn->SetNormalAtlas("atlas/common.png", "btn_common_confirm.png");
			btn->SetSelectAtlas("atlas/common.png", "btn_common_cancel.png");
			btn->SetChecked(true);
		}
	}
	void TestAnimation()
	{
		{
			GameObject* obj = GameObject::Instantiation("UIImageAnimation");
			this->AddGameObject(obj);
			UIImageAnimation* anim = obj->AddComponent<UIImageAnimation>();
			for (int i = 0; i < 10; ++i)
			{
				anim->AddImage("atlas/number.png", "txt_battle_crit0" + String::ToString(i) + ".png");
			}
			anim->SetNativeSize(true);
		}
	}

	void OnMouseDown(const UIEventInfo& info)
	{
	}
};
DC_END_NAMESPACE
