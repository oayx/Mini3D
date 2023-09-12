 #pragma once

#include "TestBase.h"

DC_BEGIN_NAMESPACE
class TestPostProcess : public TestBase
{
	typedef TestBase base;

public:
	TestPostProcess()
	{
		std::function<void()> fun = CALLBACK_0(TestPostProcess::TestColorAdjustEffect, this);
		_menus.Add(std::make_pair("ColorAdjust", fun));

		fun = CALLBACK_0(TestPostProcess::TestSimpleBlurEffect, this);
		_menus.Add(std::make_pair("SimpleBlur", fun));

		fun = CALLBACK_0(TestPostProcess::TestBloomEffect, this);
		_menus.Add(std::make_pair("Bloom", fun));
	}
	virtual void OnEnter(int child_unit_index)override
	{
		base::OnEnter(child_unit_index);
		_rootObject = GameObject::Instantiation(this->GetTitle());
		SceneManager::GetMainCamera()->GetGameObject()->AddComponent<PostProcess>();
		this->TestColorAdjustEffect();
	}
	virtual void OnLeave()override
	{
		Camera* camera = SceneManager::GetMainCamera();
		if (camera)
		{
			camera->ClearPostEffect();
		}
		SceneManager::GetMainCamera()->GetGameObject()->RemoveComponent<PostProcess>();
		base::OnLeave();
	}
	virtual String GetTitle()override { return "PostProcess"; }

	void TestColorAdjustEffect()
	{
		{
			GameObject* obj = GameObject::Instantiation("");
			this->AddGameObject(obj);
			QuadRender *mesh = obj->AddComponent<QuadRender>();
			mesh->SetMaterial("internal/material/UnlitTexture.material");
			mesh->GetMaterial()->SetTextureFile("texture/ac_rock.png");
			Transform *node = obj->GetComponent<Transform>();
			node->SetLocalScale(Vector3(2, 2, 2));
		}
		{
			Camera* camera = SceneManager::GetMainCamera();
			camera->AddPostEffect(typeof(ColorAdjustEffect));
			camera->SetClearColor(Color(0.5f, 0.5f, 0.5f, 0));
		}

		{
			GameObject* obj = GameObject::Instantiation("MainLight");
			obj->GetTransform()->SetPosition(Vector3(0.0f, 0.0f, -10.0f));
			obj->GetTransform()->LookTo(Vector3(0.0f, 0.0f, -1.0f), Vector3::up);
			this->AddGameObject(obj);
			Light* light = obj->AddComponent<Light>();
			light->mType = LightType::Direction;
			light->Diffuse = Color(1.0f, 1.0f, 1.0f);
		}
	}
	void TestSimpleBlurEffect()
	{
		{
			GameObject* obj = GameObject::Instantiation("");
			this->AddGameObject(obj);
			QuadRender *mesh = obj->AddComponent<QuadRender>();
			mesh->SetMaterial("internal/material/UnlitTexture.material");
			mesh->GetMaterial()->SetTextureFile("texture/ac_rock.png");
			Transform *node = obj->GetComponent<Transform>();
			node->SetLocalScale(Vector3(2, 2, 2));
		}

		{
			Camera* camera = SceneManager::GetMainCamera();
			camera->AddPostEffect(typeof(SimpleBlurEffect));
			camera->SetClearColor(Color(0.5f, 0.5f, 0.5f, 0));
		}

		{
			GameObject* obj = GameObject::Instantiation("MainLight");
			obj->GetTransform()->SetPosition(Vector3(0.0f, 0.0f, -10.0f));
			obj->GetTransform()->LookTo(Vector3(0.0f, 0.0f, -1.0f), Vector3::up);
			this->AddGameObject(obj);
			Light* light = obj->AddComponent<Light>();
			light->mType = LightType::Direction;
			light->Diffuse = Color(1.0f, 1.0f, 1.0f);
		}
	}
	void TestBloomEffect()
	{
		{
			GameObject* obj = GameObject::Instantiation("");
			this->AddGameObject(obj);
			QuadRender *mesh = obj->AddComponent<QuadRender>();
			mesh->SetMaterial("internal/material/UnlitTexture.material");
			mesh->GetMaterial()->SetTextureFile("texture/ac_rock.png");
			Transform *node = obj->GetComponent<Transform>();
			node->SetLocalScale(Vector3(2, 2, 2));
		}

		{
			Camera* camera = SceneManager::GetMainCamera();
			camera->AddPostEffect(typeof(BloomEffect));
			camera->SetClearColor(Color(0.5f, 0.5f, 0.5f, 0));
		}

		{
			GameObject* obj = GameObject::Instantiation("MainLight");
			obj->GetTransform()->SetPosition(Vector3(0.0f, 0.0f, -10.0f));
			obj->GetTransform()->LookTo(Vector3(0.0f, 0.0f, -1.0f), Vector3::up);
			this->AddGameObject(obj);
			Light* light = obj->AddComponent<Light>();
			light->mType = LightType::Direction;
			light->Diffuse = Color(1.0f, 1.0f, 1.0f);
		}
	}
};
DC_END_NAMESPACE
