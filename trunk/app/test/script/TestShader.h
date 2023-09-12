 #pragma once

#include "TestBase.h"

DC_BEGIN_NAMESPACE
class TestShader : public TestBase
{
	typedef TestBase base;
public:
	TestShader()
	{
		std::function<void()> fun = CALLBACK_0(TestShader::TestGeometry, this);
		_menus.Add(std::make_pair("Geometry", fun));

		fun = CALLBACK_0(TestShader::TestStencil, this);
		_menus.Add(std::make_pair("Stencil", fun));
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
	virtual String GetTitle()override { return "Shader"; }

	void TestGeometry()
	{
		//point sprite
		{
			GameObject* obj = GameObject::Instantiation("PointRender");
			this->AddGameObject(obj);
			PointRender* mesh_render = obj->AddComponent<PointRender>();
			mesh_render->SetIsSprite();
			mesh_render->GetMaterial()->SetTextureFile("texture/box.jpg");
			Vector3v vertexs = { Vector3(0.0f, 0.5f, 0.5f), Vector3(0.5f, -0.5f, 0.5f), Vector3(-0.5f, -0.5f, 0.5f) };
			mesh_render->SetVertexs(std::move(vertexs), Color::Red, 0.5f);
			Camera* camera = SceneManager::GetMainCamera();
			camera->SetClearColor(Color::Gray);
		}
	}
	void TestStencil()
	{
		{//back
			GameObject* obj = GameObject::Instantiation("");
			this->AddGameObject(obj);
			QuadRender * mesh_render = obj->AddComponent<QuadRender>();
			Material* material = mesh_render->SetMaterial("internal/material/UnlitTexture.material");
			material->SetTextureFile("texture/box.jpg");

			material->SetRenderQueue(2000);
			material->SetStencilEnable(true);
			material->SetStencilCmpFun(StencilCmp::Always);
			material->SetStencilRef(9);
			material->SetStencilOp(StencilOp::Keep, StencilOp::Keep, StencilOp::Replace);

			Transform *node = obj->GetComponent<Transform>();
			node->SetLocalPosition(Vector3(0, 0, 0));
			node->SetLocalScale(Vector3(3, 3, 3));
		}

		{//mid
			GameObject* obj = GameObject::Instantiation("");
			this->AddGameObject(obj);
			MeshRender * mesh_render = obj->AddComponent<MeshRender>();
			mesh_render->SetMesh(Mesh::Create("internal/mesh/sphere.fbx"));
			Material* material = mesh_render->SetMaterial("internal/material/UnlitTexture.material");
			material->SetTextureFile("texture/ac_rock.png");

			material->SetRenderQueue(2010);
			material->SetDepth(true, false);
			material->SetStencilEnable(true);
			material->SetStencilCmpFun(StencilCmp::NotEqual);
			material->SetStencilRef(2);
			material->SetStencilOp(StencilOp::Keep, StencilOp::Keep, StencilOp::Replace);

			Transform *node = obj->GetComponent<Transform>();
			node->SetLocalPosition(Vector3(1, 0, 0));
			node->DoRotateAround(Vector3::zero, Vector3::forward, 30);
		}

		{//front
			GameObject* obj = GameObject::Instantiation("");
			this->AddGameObject(obj);
			QuadRender * mesh_render = obj->AddComponent<QuadRender>();
			Material* material = mesh_render->SetMaterial("internal/material/UnlitTexture.material");
			material->SetTextureFile("texture/box.jpg");

			material->SetRenderQueue(2020);
			material->SetStencilEnable(true);
			material->SetStencilCmpFun(StencilCmp::Equal);
			material->SetStencilRef(2);
			material->SetStencilOp(StencilOp::Keep, StencilOp::Keep, StencilOp::Keep);

			Transform *node = obj->GetComponent<Transform>();
			node->SetLocalPosition(Vector3(0, 0, 0));
			node->SetLocalScale(Vector3(5, 5, 5));
		}
	}
};
DC_END_NAMESPACE