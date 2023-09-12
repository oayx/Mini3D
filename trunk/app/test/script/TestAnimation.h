 
#pragma once

#include "TestBase.h"

DC_BEGIN_NAMESPACE
class TestAnimation : public TestBase
{
	typedef TestBase base;

public:
	TestAnimation()
	{
	}
	virtual void OnEnter(int child_unit_index)override
	{
		base::OnEnter(child_unit_index);
		_rootObject = GameObject::Instantiation(this->GetTitle());
		Test();
	}
	virtual void OnLeave()override 
	{
		base::OnLeave();
	}
	virtual String GetTitle()override { return "Animation"; }

private:
	void Test()
	{
		GameObject* obj = GameObject::Instantiation("");
		this->AddGameObject(obj);

		Mesh* mesh = Mesh::Create("model/X_jijia_1010.FBX");
		//ClipAnimations clips;//由编辑器生成
		//clips.Add(ClipAnimationDesc("idle", 4, 6, true));
		//clips.Add(ClipAnimationDesc("run", 11, 35, true));
		//clips.Add(ClipAnimationDesc("die", 228, 261));
		//mesh->ClipAnimation(clips);
		
		SkinMeshRender * mesh_render = obj->AddComponent<SkinMeshRender>();
		Animation* animation = obj->AddComponent<Animation>();
		mesh_render->SetMesh(mesh);
		mesh_render->SetMaterial("internal/material/Character.material");
		mesh_render->GetMaterial(0)->SetTextureFile("model/X_jijia_1010_Dmap_H.png");
		animation->Play("run", true);

		Transform *node = obj->GetComponent<Transform>();
		node->SetLocalScale(Vector3(0.5f, 0.5f, 0.5f));
		//node->Rotate(Quaternion(Vector3(90, 90, 0)));
	}
	void TestCross()
	{
		GameObject* obj = GameObject::Instantiation("");
		this->AddGameObject(obj);

		Mesh* mesh = Mesh::Create("model/30010.FBX");
		mesh->ClearAnimations();
		mesh->LoadAnimation("model/30010@attack.FBX", "attack");
		mesh->LoadAnimation("model/30010@die.FBX", "die");
		mesh->LoadAnimation("model/30010@enter.FBX", "enter");
		mesh->LoadAnimation("model/30010@idle.FBX", "idle");
		mesh->LoadAnimation("model/30010@walk.FBX", "walk");
		
		SkinMeshRender * mesh_render = obj->AddComponent<SkinMeshRender>();
		Animation* animation = obj->AddComponent<Animation>();
		mesh_render->SetMesh(mesh);
		mesh_render->SetMaterial("internal/material/Character.material");
		mesh_render->GetMaterial(0)->SetTextureFile("model/30010.jpg");
		animation->SetSpeed(0.1f);
		animation->Play("die", false);
		uint timer_id = Timer::AddOnce(8, [animation]
			{
				animation->PlayCross("walk", 0.1f, false);
			});
		_timers.Add(timer_id);

		Transform *node = obj->GetComponent<Transform>();
		node->SetLocalScale(Vector3(0.01f, 0.01f, 0.01f));
	}
};
DC_END_NAMESPACE
