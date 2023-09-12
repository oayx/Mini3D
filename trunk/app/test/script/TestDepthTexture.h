#pragma once

#include "TestBase.h"

DC_BEGIN_NAMESPACE
class TestDepthTexture : public TestBase
{
	typedef TestBase base;
public:
	TestDepthTexture()
	{
		std::function<void()> fun = CALLBACK_0(TestDepthTexture::TestDepthMap, this);
		_menus.Add(std::make_pair("DepthMap", fun));

		fun = CALLBACK_0(TestDepthTexture::TestDepthNormal, this);
		_menus.Add(std::make_pair("DepthNormal", fun));
	}
	virtual void OnEnter(int child_unit_index)override
	{
		base::OnEnter(child_unit_index);
		this->SetCameraCanMove(true);
		_rootObject = GameObject::Instantiation(this->GetTitle());
		SceneManager::GetMainCamera()->GetGameObject()->AddComponent<PostProcess>();
	}
	virtual void OnLeave()override
	{
		Camera* camera = SceneManager::GetMainCamera();
		camera->ClearDepthTextureMode();
		camera->ClearPostEffect();
		SceneManager::GetMainCamera()->GetGameObject()->RemoveComponent<PostProcess>();
		base::OnLeave();
	}
	virtual String GetTitle()override { return "DepthTexture"; }

	void TestDepthMap()
	{
#if 1//场景
		{
			GameObject* obj = GameObject::Instantiation("");
			this->AddGameObject(obj);
			MeshRender * mesh = obj->AddComponent<MeshRender>();
			mesh->SetMesh(Mesh::Create("model/dxbd/dxbd.FBX"));
			mesh->GetMaterial(0)->SetTextureFile("model/dxbd/dxbd.jpg");
			Transform *node = obj->GetComponent<Transform>();
			node->SetLocalPosition(Vector3(0, 0, 0));
			node->SetLocalScale(Vector3(0.001f, 0.001f, 0.001f));
		}

		//Quad
		{
			GameObject* obj = GameObject::Instantiation("");
			this->AddGameObject(obj);
			QuadRender * mesh = obj->AddComponent<QuadRender>();
			mesh->GetMaterial()->SetTextureAddress(TextureAddress::Wrap);
			Transform *node = obj->GetComponent<Transform>();
			node->SetLocalPosition(Vector3(0, 0, 0));
			node->SetLocalScale(Vector3(5, 5, 5));
		}

		{
			Camera* camera = SceneManager::GetMainCamera();
			camera->AddPostEffect(typeof(ShowDepthMapEffect));
			camera->GetTransform()->SetPosition(Vector3(0.0f, 0.0f, -20.0f));
			camera->SetZFar(50);
			camera->AddDepthTextureMode(DepthTextureMode::Depth);
		}
#endif
	}

	void TestDepthNormal()
	{
#if 1//场景
		{
			GameObject* obj = GameObject::Instantiation("");
			this->AddGameObject(obj);
			MeshRender * mesh = obj->AddComponent<MeshRender>();
			mesh->SetMesh(Mesh::Create("model/dxbd/dxbd.FBX"));
			mesh->GetMaterial(0)->SetTextureFile("model/dxbd/dxbd.jpg");
			Transform *node = obj->GetComponent<Transform>();
			node->SetLocalPosition(Vector3(0, 0, 0));
			node->SetLocalScale(Vector3(0.001f, 0.001f, 0.001f));
		}

		//Quad
		{
			GameObject* obj = GameObject::Instantiation("");
			this->AddGameObject(obj);
			QuadRender * mesh = obj->AddComponent<QuadRender>();
			mesh->GetMaterial()->SetTextureAddress(TextureAddress::Wrap);
			Transform *node = obj->GetComponent<Transform>();
			node->SetLocalPosition(Vector3(0, 0, 0));
			node->SetLocalScale(Vector3(5, 5, 5));
		}

		{
			Camera* camera = SceneManager::GetMainCamera();
			camera->AddPostEffect(typeof(ShowDepthNormalEffect));
			camera->GetTransform()->SetPosition(Vector3(0.0f, 0.0f, -20.0f));
			camera->SetZFar(50);
			camera->AddDepthTextureMode(DepthTextureMode::Depth | DepthTextureMode::Normals);
		}
#endif
	}
};
DC_END_NAMESPACE
