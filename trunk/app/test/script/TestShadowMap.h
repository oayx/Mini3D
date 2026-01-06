 #pragma once

#include "TestBase.h"

DC_BEGIN_NAMESPACE
class TestShadowMap : public TestBase
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
	virtual String GetTitle()override { return "ShadowMap"; }

private:
	void Test()
	{
#if 1//场景
		{
			GameObject* obj = GameObject::Instantiation("QuadRender-ShadowMap");
			this->AddGameObject(obj);
			QuadRender * plane = obj->AddComponent<QuadRender>();
			plane->SetMaterial("internal/material/ShadowMapReceive.material");
			plane->GetMaterial()->SetTextureFile("texture/cao_01.jpg");
			plane->GetMaterial()->SetTextureAddress(TextureAddress::Wrap, Vector2::one * 20);
			plane->GetMaterial()->SetCastShadow(false);
			plane->GetMaterial()->SetReceiveShadow(true);
			Transform *plane_node = obj->GetComponent<Transform>();
			plane_node->SetLocalScale(Vector3(100, 100, 1));
			plane_node->SetLocalPosition(Vector3(0, 0, 0));
			plane_node->Pitch(90);
		}

		{
			GameObject* obj = GameObject::Instantiation("Mesh-sphere");
			this->AddGameObject(obj);
			MeshRender* mesh = obj->AddComponent<MeshRender>();
			mesh->SetMaterial("internal/material/Diffuse.material");
			mesh->SetMesh(Mesh::Create("internal/mesh/sphere.fbx"));
			mesh->GetMaterial()->SetTextureFile("texture/ac_rock.png");
			Transform *node = obj->GetComponent<Transform>();
			node->SetLocalPosition(Vector3(0, 0.5f, 0));
		}

		{
			GameObject* obj = GameObject::Instantiation("box");
			this->AddGameObject(obj);
			CubeRender *box = obj->AddComponent<CubeRender>();
			box->SetMaterial("internal/material/Diffuse.material");
			box->GetMaterial()->SetTextureFile("texture/box.jpg");

			Transform *node = obj->GetComponent<Transform>();
			node->SetLocalPosition(Vector3(-3, 0.5f, 0));
		}

		Camera* camera = SceneManager::GetMainCamera();
		camera->GetTransform()->SetPosition(Vector3(0.0f, 10.0f, -10.0f));
		camera->GetTransform()->Pitch(45);
		camera->SetZFar(1000);
#endif

#if 1//平行光
		{
			GameObject* obj = GameObject::Instantiation("MainLight");
			this->AddGameObject(obj);
			obj->GetTransform()->SetPosition(Vector3(0.0f, 10.0f, -10.0f));
			obj->GetTransform()->LookTo(Vector3(1.0f, -1.0, 1.0f), Vector3::up);
			Light* light = obj->AddComponent<Light>();
			light->SetCullMask(LayerMask::Everything ^ LayerMask::GetMask(LayerMask::UI));
			light->mType = LightType::Direction;
			light->Diffuse = Color(0.8f, 0.8f, 0.8f);
			//light->SetShadowBias(-0.01f);
			//DX12如果是2048会报错
			light->EnableShadowMap(ShadowType::Soft, 2048, 2048, ColorFormat::DEPTH24_STENCIL8);
		}
#endif
	}
};
DC_END_NAMESPACE