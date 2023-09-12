 #pragma once

#include "TestBase.h"

DC_BEGIN_NAMESPACE
class TestVideo : public TestBase
{
	typedef TestBase base;

public:
	virtual void OnEnter(int child_unit_index)override
	{
		base::OnEnter(child_unit_index);
		_rootObject = GameObject::Instantiation(this->GetTitle());
		this->Test();
	}
	virtual void OnLeave()override 
	{
		base::OnLeave();
	}
	virtual String GetTitle()override { return "Video"; }

private:
	void Test()
	{
		GameObject* obj = GameObject::Instantiation("Video");
		this->AddGameObject(obj);
		auto video = obj->AddComponent<VideoPlayer>();
		video->Play(Resource::GetFullDataPath("video/BigBuckBunny_360p30.mp4"), false);
		video->OnComplete([]
			{
				Debuger::Log("play complete");
			});

		GameObject* obj_sprite = GameObject::Instantiation("sprite");
		this->AddGameObject(obj_sprite);
		Sprite * mesh = obj_sprite->AddComponent<Sprite>();
		mesh->SetTexture(video->GetTexture());
	}
};
DC_END_NAMESPACE