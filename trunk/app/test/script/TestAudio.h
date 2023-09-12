 #pragma once

#include "TestBase.h"

DC_BEGIN_NAMESPACE
class TestAudio : public TestBase
{
	typedef TestBase base;

public:
	TestAudio()
	{
		std::function<void()> fun = CALLBACK_0(TestAudio::TestMusic, this);
		_menus.Add(std::make_pair("Music", fun));

		fun = CALLBACK_0(TestAudio::TestBackground, this);
		_menus.Add(std::make_pair("Background", fun));
	}
	virtual void OnEnter(int child_unit_index)override
	{
		base::OnEnter(child_unit_index);
		_rootObject = GameObject::Instantiation(this->GetTitle());
	}
	virtual void OnLeave()override 
	{
		base::OnLeave();
	}
	virtual String GetTitle()override { return "Audio"; }

private:
	//音效
	void TestMusic()
	{
		GameObject* obj = GameObject::Instantiation("");
		this->AddGameObject(obj);
		AudioClip* audio_clip1 = AudioClip::Create("sound/click.wav");
		auto audio_source = obj->AddComponent<AudioSource>();
		audio_source->SetClip(audio_clip1);
		audio_source->SetLoop(false);
		audio_source->Play();
	}
	//背景音乐
	void TestBackground()
	{
		GameObject* obj = GameObject::Instantiation("");
		this->AddGameObject(obj);
		AudioClip* audio_clip = AudioClip::Create("sound/music_bg.wav");
		auto audio_source = obj->AddComponent<AudioSource>();
		audio_source->SetClip(audio_clip);
		audio_source->SetLoop(true);
		audio_source->Play();
		audio_source->SetVolume(0.2f);
	}
};
DC_END_NAMESPACE