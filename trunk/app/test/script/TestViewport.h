 #pragma once

#include "TestBase.h"

DC_BEGIN_NAMESPACE
class TestViewport : public TestBase
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

		Camera* camera = SceneManager::GetMainCamera();
		camera->SetViewport(0.0f, 0.0f, 1.0f, 1.0f);
	}
	virtual String GetTitle()override { return "Viewport"; }

	void Test()
	{
		{//相机1
			Camera* camera = SceneManager::GetMainCamera();
			camera->SetClearColor(Color(0, 1, 0, 0));
			camera->SetClearFlag(ClearFlag::SolidColor);
			camera->SetViewport(0.0f, 0.0f, 0.5f, 1.0f);
		}
		{//相机2
			GameObject* obj = GameObject::Instantiation("MainCamera2");
			this->AddGameObject(obj);
			obj->GetTransform()->SetLocalPosition(Vector3(0, 0, -10));

			Camera* camera = obj->AddComponent<Camera>();
			camera->SetClearColor(Color(1, 0, 0, 0));
			camera->SetClearFlag(ClearFlag::DepthOnly);
			camera->SetViewport(0.5f, 0.0f, 0.5f, 1.0f);
		}

		{//plane
			GameObject* obj = GameObject::Instantiation("");
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
