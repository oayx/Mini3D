 
#pragma once

#include "TestBase.h"

DC_BEGIN_NAMESPACE
class TestSkyBox : public TestBase
{
	typedef TestBase base;

public:
	virtual void OnEnter(int child_unit_index)override
	{
		base::OnEnter(child_unit_index);
		_rootObject = GameObject::Instantiation(this->GetTitle());
		this->SetCameraCanMove(true);
		this->Test();
	}
	virtual void OnLeave()override 
	{
		SceneManager::GetMainCamera()->SetClearFlag(ClearFlag::Color);
		base::OnLeave();
	}
	virtual String GetTitle()override { return "SkyBox"; }

private:
	void Test()
	{
		SceneManager::GetMainCamera()->SetSkyBox();

		//box
		{
			GameObject* obj = GameObject::Instantiation("CubeRender");
			this->AddGameObject(obj);
			CubeRender * mesh_render = obj->AddComponent<CubeRender>();
			mesh_render->SetMaterial("internal/material/SkyBoxReflect.material");
			mesh_render->GetMaterial()->SetTextureFile("texture/ac_rock.png");
		}
	}
};
DC_END_NAMESPACE
