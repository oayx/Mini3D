 
#pragma once

#include "TestBase.h"

DC_BEGIN_NAMESPACE
class TestTween : public TestBase
{
	typedef TestBase base;
	Vector<Tween*> _tweens;

public:
	TestTween()
	{
		std::function<void()> fun = CALLBACK_0(TestTween::Test1, this);
		_menus.Add(std::make_pair("Test1", fun));
	}
	virtual void OnEnter(int child_unit_index)override
	{
		base::OnEnter(child_unit_index);
		_rootObject = GameObject::Instantiation(this->GetTitle());
	}
	virtual void OnLeave()override 
	{
		for (auto tween : _tweens)
		{
			TweenManager::Stop(tween);
		}
		_tweens.Clear();

		base::OnLeave();
	}
	virtual String GetTitle()override { return "Tween"; }

private:
	void Test1()
	{
		float v = Easy::Do(EasyType::Linear, 0, 10, 0.2f);
		v = Easy::Do(EasyType::SineIn, 0, 10, 0.2f);
		v = Easy::Do(EasyType::SineOut, 0, 10, 0.2f);
		//tween
		{
			GameObject* obj = GameObject::Instantiation("CubeRender");
			this->AddGameObject(obj);
			CubeRender* mesh_render = obj->AddComponent<CubeRender>();
			mesh_render->SetMaterial("internal/material/UnlitTexture.material");
			mesh_render->GetMaterial()->SetTextureFile("texture/cao_01.jpg");

			//Time::SetTimeScale(2);
			//TweenManager::UseUnscaledTime(false);
			Transform* node = obj->GetComponent<Transform>();

			//{
			//	Tween* tween = node->DOLocalMove(Vector3(3, 0, 0), 3)->SetEasy(EasyType::Cubicin)->SetDelay(1)->OnComplete([] {Debuger::Log("tween1 compnent:%d", Time::GetFrameCount()); });
			//	//tween->Join(node->GetGameObject()->DOBlink(1, 5, false));
			//	tween = tween->Append(node->DoScale(Vector3::one * 2, 3))->SetDelay(-2)->OnComplete([] {Debuger::Log("tween2 compnent:%d", Time::GetFrameCount()); });
			//	tween = tween->Append(node->DoPlace(Vector3(0, 0, 0)))->OnComplete([] {Debuger::Log("tween3 compnent:%d", Time::GetFrameCount()); });
			//	//tween->Append(node->GetGameObject()->DOVisible(false, 3));
			//	Timer::AddOnce(1, [tween]
			//		{
			//			//TweenManager::Stop(tween->Root());
			//		});
			//}
			//{//from
			//	Tween* tween = node->DOMove(Vector3(3, 0, 0), 3);
			//	tween->From();
			//	tween->SetDelay(3);
			//	tween->OnStart([] {Debuger::Log("tween start:%d", Time::GetFrameCount()); });
			//}
			//{//loop
			//	Tween* tween = node->DOLocalMove(Vector3(3, 0, 0), 3);
			//	tween->Join(node->DoScale(Vector3(2, 2, 2), 3))->OnComplete([] {Debuger::Log("scale tween compnent:%d", Time::GetFrameCount()); });
			//	tween->Join(node->DoLocalRotate(Vector3(0, 90, 0), 3))->OnComplete([] {Debuger::Log("rotate tween compnent:%d", Time::GetFrameCount()); });
			//	tween->SetLoop(3)->OnComplete([] {Debuger::Log("loop tween compnent:%d", Time::GetFrameCount()); });
			//	tween->OnStepComplete([] {Debuger::Log("loop tween step compnent:%d", Time::GetFrameCount()); });
			//}
			{//暂停与恢复
				Tween* tween = node->DOMove(Vector3(3, 0, 0), 3);
				_tweens.Add(tween);

				uint timer_id = Timer::AddOnce(1, [tween]
					{
						tween->Pause();
					});
				_timers.Add(timer_id);

				timer_id = Timer::AddOnce(2, [tween]
					{
						tween->Resume();
					});
				_timers.Add(timer_id);
			}
		}
	}
};
DC_END_NAMESPACE