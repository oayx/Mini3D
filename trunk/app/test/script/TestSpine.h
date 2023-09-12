 
#pragma once

#include "TestBase.h"

DC_BEGIN_NAMESPACE
class TestSpine : public TestBase
{
	typedef TestBase base;

public:
	TestSpine()
	{
		std::function<void()> fun = CALLBACK_0(TestSpine::Test1, this);
		_menus.Add(std::make_pair("Test1", fun));
	}
	virtual void OnEnter(int child_unit_index)override
	{
		base::OnEnter(child_unit_index);
		_rootObject = GameObject::Instantiation(this->GetTitle());
		_rootObject->GetTransform()->SetLocalScale(Vector3(0.003f, 0.003f, 0.003f));
	}
	virtual void OnLeave()override 
	{
		base::OnLeave();
	}
	virtual String GetTitle()override { return "Spine"; }

private:
	void Test1()
	{
		Skeleton2d* skeleton = _rootObject->AddComponent<Skeleton2d>();
		{
			skeleton->LoadFromBinary("Spine/spineboy/spineboy-pro.skel", "Spine/spineboy/spineboy.atlas");
			VecString anims = skeleton->GetAnimations();
			skeleton->SetAnimation(0, "walk", true);
			//skeleton->SetAnimation(1, "shoot", true);
		}
		{
			//skeleton->LoadFromBinary("Spine/goblins/goblins-pro.skel", "Spine/goblins/goblins.atlas");
			//skeleton->SetAnimation(0, "walk", true);
			//skeleton->SetSkin("goblin");
		}
	}
};
DC_END_NAMESPACE
