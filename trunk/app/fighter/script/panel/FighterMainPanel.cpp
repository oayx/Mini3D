#include "FighterMainPanel.h"
#include "FighterSetterPanel.h"
#include "core/event/EventDispatcher.h"
#include "runtime/ui/UIImageList.h"
#include "gui/UIManager.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
void FighterMainPanel::Awake()
{
	GetGameObject()->GetRectTransform()->SetStretchMode(StretchMode::All);
	//右上工具栏
	{
		GameObject* tools_obj = GameObject::InstantiationUI("ToolsRoot");
		tools_obj->GetTransform()->SetParent(GetTransform(), false);
		tools_obj->GetRectTransform()->SetAlige(AligeType::RightTop);

		GameObject* set_obj = GameObject::InstantiationUI("Setter");
		set_obj->GetTransform()->SetParent(tools_obj->GetTransform(), false);
		set_obj->GetTransform()->SetLocalPosition(Vector3(-50, -50, 0));
		UIImage* set_img = set_obj->AddComponent<UIImage>();
		set_img->SetAtlas("atlas/compnent.png", "ui_27.png");
		set_img->SetNativeSize();
		set_img->SetEventListener(UIEventType::LMouseClick, [this](const UIEventInfo& info)
			{
				UIManager::Instance()->Show<FighterSetterPanel>();
			});

		GameObject* stop_obj = GameObject::InstantiationUI("Stop");
		stop_obj->GetTransform()->SetParent(tools_obj->GetTransform(), false);
		stop_obj->GetTransform()->SetLocalPosition(Vector3(-130, -50, 0));
		UIImageList* stop_img = stop_obj->AddComponent<UIImageList>();
		stop_img->AddImage("atlas/compnent.png", "ui_21.png");
		stop_img->AddImage("atlas/compnent.png", "ui_22.png");
		stop_img->SetImage(0);
		stop_img->SetNativeSize(true);
		stop_img->SetEventListener(UIEventType::LMouseClick, [this, stop_img](const UIEventInfo& info)
			{
				int index = stop_img->GetImage();
				stop_img->SetImage(index == 0 ? 1 : 0);
				if (stop_img->GetImage() == 0)
				{
					Time::SetTimeScale(1.0f);
				}
				else
				{
					Time::SetTimeScale(0.0f);
				}
			});
	}

	//顶部血条
	{
		GameObject* hp_root = GameObject::InstantiationUI("HPRoot");
		hp_root->GetTransform()->SetParent(GetTransform(), false);
		hp_root->GetRectTransform()->SetAlige(AligeType::Top);
		hp_root->GetTransform()->SetLocalPosition(Vector3(0, -60, 0));

		GameObject* hp_bg_obj = GameObject::InstantiationUI("HPbg");
		hp_bg_obj->GetTransform()->SetParent(hp_root->GetTransform(), false);
		UIImage* hp_bg_img = hp_bg_obj->AddComponent<UIImage>();
		hp_bg_img->SetAtlas("atlas/compnent.png", "ui_progress_bg_1.png");
		hp_bg_img->SetSize(365, 47);

		GameObject* hp_obj = GameObject::InstantiationUI("HProgress");
		hp_obj->GetTransform()->SetParent(hp_root->GetTransform(), false);
		_imgHp = hp_obj->AddComponent<UIImage>();
		_imgHp->SetAtlas("atlas/compnent.png", "ui_progress_1.png");
		_imgHp->SetNativeSize();
		_imgHp->SetImageType(UIImageType::Filled);
		_imgHp->SetFillMethod(UIImageFillMethod::Horizontal, (int)UIImageOriginHorizontal::Left);
	}
}
void FighterMainPanel::RegisterEvent()
{
	EventDispatcher::AddEventListener(Evt_MainRole_Hp, this, MakeEventHandler2(FighterMainPanel::OnRoleHp));
}
void FighterMainPanel::UnRegisterEvent()
{
	EventDispatcher::RemoveEventListener(Evt_MainRole_Hp, this, MakeEventHandler2(FighterMainPanel::OnRoleHp));
}
void FighterMainPanel::OnRoleHp(const String& arg1, const String& arg2)
{
	int cur_hp = arg1.ToInt();
	int max_hp = arg2.ToInt();
	cur_hp = Math::Clamp<int>(cur_hp, 0, cur_hp);
	max_hp = Math::Clamp<int>(max_hp, 1, max_hp);
	_imgHp->SetFillAmount(float(cur_hp) / float(max_hp));
}
DC_END_NAMESPACE
