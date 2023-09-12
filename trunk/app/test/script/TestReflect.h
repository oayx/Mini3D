 
#pragma once

#include "TestBase.h"

DC_BEGIN_NAMESPACE
class TestReflect : public TestBase
{
	typedef TestBase base;

public:
	virtual void OnEnter(int child_unit_index)override
	{
		base::OnEnter(child_unit_index);
		this->SetCameraCanMove(true);
		_rootObject = GameObject::Instantiation(this->GetTitle());
		this->Test();
	}
	virtual void OnLeave()override 
	{
		base::OnLeave();
	}
	virtual String GetTitle()override { return "Reflect"; }

private:
	void Test()
	{
		{
			GameObject* obj = GameObject::Instantiation("ReflectionProbe");
			this->AddGameObject(obj);
			obj->SetLayer(LayerMask::NameToLayer(LayerMask::IgnoreRaycast));
			ReflectionProbe* probe = obj->AddComponent<ReflectionProbe>();
			probe->SetMode(ReflectionMode::EveryFrame);
			probe->SetResolution(256);
			probe->SetClearColor(Color::White);

			CubeRender* mesh_render = obj->AddComponent<CubeRender>();
			mesh_render->SetMaterial("internal/material/ReflectionProbe.material");
			mesh_render->GetMaterial()->SetDepth(true, false);
		}
		{
			GameObject* obj = GameObject::Instantiation("CubeRender");
			this->AddGameObject(obj);
			CubeRender * mesh_render = obj->AddComponent<CubeRender>();
			mesh_render->SetMaterial("internal/material/UnlitTexture.material");
			mesh_render->GetMaterial()->SetTextureFile("texture/box.jpg");
			Transform* node = obj->GetComponent<Transform>();
			node->SetLocalPosition(Vector3(3, 0, 0));
		}
		{
			GameObject* obj = GameObject::Instantiation("Plane");
			this->AddGameObject(obj);
			PlaneRender * mesh_render = obj->AddComponent<PlaneRender>();
			mesh_render->SetMaterial("internal/material/UnlitTexture.material");
			mesh_render->SetPlane(200, 200, 40, 40, 40, 40);
			Material* material = mesh_render->GetMaterial();
			material->SetTextureFilter(TextureFilter::Trilinear, 16);
			material->SetTextureAddress(TextureAddress::Wrap);
			material->SetTextureFile("texture/ac_rock.png");
			material->SetRenderQueue(1900);
			Transform *node = obj->GetComponent<Transform>();
			node->Pitch(90);
			node->SetPosition(Vector3(0, -2, 0));
		}
	}
};
DC_END_NAMESPACE
