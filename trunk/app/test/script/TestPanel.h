 
#pragma once

#include "TestAnimation.h"
#include "TestAudio.h"
#include "TestCore.h"
#include "TestDepthTexture.h"
#include "TestFile.h"
#include "TestLight.h"
#include "TestLua.h"
#include "TestParticle.h"
#include "TestPhysx.h"
#include "TestPhysx2d.h"
#include "TestPrim.h"
#include "TestPostProcess.h"
#include "TestReflect.h"
#include "TestShader.h"
#include "TestShadowMap.h"
#include "TestSkyBox.h"
#include "TestSocket.h"
#include "TestSpine.h"
#include "TestTerrain.h"
#include "TestTween.h"
#include "TestUI.h"
#include "TestVideo.h"
#include "TestViewport.h"

DC_BEGIN_NAMESPACE
class TestPanel : public Component
{
	TestBase* _currUnit = nullptr;
	Vector<TestBase*> _listUnits;
	int _unitIndex = -1;
	int _childUnitIndex = -1;

	GameObject* _uIRootObject = nullptr;
	GameObject* _menuRoot = nullptr;


public:
	virtual void Awake()override
	{
		{
			_listUnits.Add(this->GetGameObject()->AddComponent<TestCore>());
			_listUnits.Add(this->GetGameObject()->AddComponent<TestFile>());
			_listUnits.Add(this->GetGameObject()->AddComponent<TestUI>());
			_listUnits.Add(this->GetGameObject()->AddComponent<TestPrim>());
			_listUnits.Add(this->GetGameObject()->AddComponent<TestTween>());
			_listUnits.Add(this->GetGameObject()->AddComponent<TestReflect>());
			_listUnits.Add(this->GetGameObject()->AddComponent<TestAnimation>());
			_listUnits.Add(this->GetGameObject()->AddComponent<TestLight>());
			_listUnits.Add(this->GetGameObject()->AddComponent<TestSkyBox>());
			_listUnits.Add(this->GetGameObject()->AddComponent<TestTerrain>());
			_listUnits.Add(this->GetGameObject()->AddComponent<TestSpine>());
			_listUnits.Add(this->GetGameObject()->AddComponent<TestViewport>());
			_listUnits.Add(this->GetGameObject()->AddComponent<TestParticle>());
			_listUnits.Add(this->GetGameObject()->AddComponent<TestPhysx>());
			_listUnits.Add(this->GetGameObject()->AddComponent<TestPhysx2d>());
			_listUnits.Add(this->GetGameObject()->AddComponent<TestShader>());
			_listUnits.Add(this->GetGameObject()->AddComponent<TestPostProcess>());
			_listUnits.Add(this->GetGameObject()->AddComponent<TestShadowMap>());
			_listUnits.Add(this->GetGameObject()->AddComponent<TestDepthTexture>());
			_listUnits.Add(this->GetGameObject()->AddComponent<TestAudio>());
			_listUnits.Add(this->GetGameObject()->AddComponent<TestVideo>());
			_listUnits.Add(this->GetGameObject()->AddComponent<TestLua>());
			_listUnits.Add(this->GetGameObject()->AddComponent<TestSocket>());
		}

		{
			//UIAtlasManager::LoadAtlas("atlas/common.json");
			//UIAtlasManager::LoadAtlas("atlas/compnent.json");
			//UIAtlasManager::LoadAtlas("atlas/head.json");
			//UIAtlasManager::LoadAtlas("atlas/number.json");
		}

		{
			_uIRootObject = GameObject::Instantiation("UIRoot");
			_uIRootObject->GetTransform()->SetLocalScale(Vector3::one * 0.01f);
			Camera* camera = _uIRootObject->AddComponent<Camera>();
			UICanvas * canvas = _uIRootObject->AddComponent<UICanvas>();
			canvas->SetRenderMode(UIRenderMode::ScreenSpace_Camera);
			canvas->SetCamera(camera);
			camera->SetOrthographic(true);
			camera->SetDepth(10);
			camera->SetZNear(-100);
			camera->SetZFar(1000);
			camera->SetClearFlag(ClearFlag::DepthOnly);
			camera->SetCullMask(LayerMask::GetMask(LayerMask::UI));

			UICanvasScaler* scaler = _uIRootObject->AddComponent<UICanvasScaler>();
			scaler->SetReferenceResolution(iSize(Screen::GetWidth(), Screen::GetHeight()));
			scaler->SetScaleMatchMode(UIScaleMatchMode::Expand);
			scaler->SetMatchWidth(false);
		}

		{//返回按钮
			GameObject* back_obj = GameObject::Instantiation("BtnBack");
			back_obj->GetTransform()->SetParent(_uIRootObject->GetTransform(), false);
			back_obj->GetTransform()->SetLocalPosition(Vector3(Screen::GetWidth() * 0.5f - 50.0f, -Screen::GetHeight() * 0.5f + 50.0f, 0.0f));

			//按钮
			UIImage* btn = back_obj->AddComponent<UIImage>();
			btn->SetAtlas("atlas/compnent.png", "ui_14.png");
			btn->SetNativeSize();
			btn->SetEventListener(UIEventType::LMouseDown, [this](const UIEventInfo& info)
				{
					if (_unitIndex >= 0)
					{
						this->OnEnter(-1, -1);
					}
				});
		}
	}

	void OnEnter(int unit_index, int child_unit_index)
	{
		_unitIndex = unit_index;
		_childUnitIndex = child_unit_index;

		if (_currUnit)
		{
			_currUnit->OnLeave();
			_currUnit = nullptr;
		}

		this->RefreshList(unit_index, child_unit_index);
	}
	void OnLeave()
	{

	}

	//刷新界面显示
	void RefreshList(int unit_index, int child_unit_index)
	{
		_unitIndex = unit_index;
		_childUnitIndex = child_unit_index;

		GameObject::Destroy(_menuRoot);

		MenuCallbacks callbacks;
		if (unit_index < 0)
		{
			callbacks = GetMenus();
		}
		else
		{
			_currUnit = _listUnits[unit_index];
			_currUnit->OnEnter(child_unit_index);
			callbacks = _currUnit->GetMenus();
		}

		_menuRoot = GameObject::Instantiation("MenuRoot");
		_menuRoot->GetTransform()->SetParent(_uIRootObject->GetTransform(), false);

		float totalHeight = Screen::GetHeight();
		float width_scale = Screen::GetWidth() / 1334.0f;
		float height_scale = Screen::GetHeight() / 750.0f;
		float per_width = 300 * width_scale;
		float per_height = 40.0f * height_scale;
		float offsetX = 30 * width_scale;
		float offsetY = 10 * height_scale;
		float start_offset_x = -Screen::GetWidth() * 0.5f + per_width * 0.5f + 20.0f;
		float start_offset_y = Math::Min<float>((callbacks.Size() * (per_height + offsetY)) * 0.5f, totalHeight * 0.5f - per_height);
		int row = 0, col = 0;

		for (uint i = 0; i < callbacks.Size(); ++i)
		{
			auto callback = callbacks[i];

			GameObject* menu_obj = GameObject::Instantiation("MenuObject:" + std::to_string(i));
			menu_obj->GetTransform()->SetParent(_menuRoot->GetTransform(), false);
			float pos_x = start_offset_x + (per_width + offsetX) * col;
			float pos_y = start_offset_y - row * (per_height + offsetY) - per_height * 0.5f;
			menu_obj->GetTransform()->SetLocalPosition(Vector3(pos_x, pos_y, 0.0f));

			//按钮
			UIImage* btn = menu_obj->AddComponent<UIImage>();
			btn->SetAtlas("atlas/common.png", "btn_card_filter02.png");
			btn->SetSize(per_width, per_height);
			btn->SetImageType(UIImageType::Sliced);
			btn->SetImageBorder(Vector4(10, 10, 10, 10));
			btn->SetEventListener(UIEventType::LMouseDown, [this,i, callback](const UIEventInfo& info)
				{
					Debuger::Log("hit index:%d", i);
					if (_unitIndex < 0)
					{
						this->RefreshList(i, -1);
					}
					else
					{
						_currUnit->OnLeave();
						_currUnit->OnEnter(i);
						callback.second();
					}
				});

			//文字
			GameObject* txt_obj = GameObject::Instantiation("BtnTxt");
			txt_obj->GetTransform()->SetParent(menu_obj->GetTransform(), false);
			UILabel* txt = txt_obj->AddComponent<UILabel>();
			txt->SetText(callback.first);
			txt->SetFontSize(20);
			txt->SetColor(Color::Black);
			txt->SetSize(per_width, per_height);
			txt->SetAlignment(UITextAnchor::MiddleCenter);
			txt->SetHorizontalOverflow(UIHorizontalWrapMode::Overflow);

			//换行列
			row++;
			if (pos_y - (per_height + offsetY) <= -totalHeight * 0.5f)
			{
				row = 0;
				col++;
			}
		}

	}
	//获取默认菜单
	MenuCallbacks GetMenus()
	{
		MenuCallbacks list;

		for (uint i = 0; i < _listUnits.Size(); ++i)
		{
			TestBase* unit = _listUnits[i];
			std::function<void()> fun = CALLBACK_0(TestPanel::OnClickMenu, this);
			list.Add(std::make_pair(unit->GetTitle(), fun));
		}

		return list;
	}
	//点击菜单
	void OnClickMenu()
	{

	}
	TestBase* GetCurrUnit()const
	{
		return _currUnit;
	}
};
DC_END_NAMESPACE