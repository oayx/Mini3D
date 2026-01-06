 #pragma once

#include "TestBase.h"

DC_BEGIN_NAMESPACE
class TestComponentPrim : public Component
{
protected:
	virtual void Start()override
	{
		Debuger::Log("Start:%s", this->GetInstanceName().c_str());
	}
	virtual void Update()override
	{
		//Debuger::Log("Update");
	}
	virtual void LateUpdate()override
	{
		//Debuger::Log("LateUpdate");
	}
	virtual void OnEnable()override
	{
		Debuger::Log("OnEnable:%s", this->GetInstanceName().c_str());
	}
	virtual void OnDisable()override
	{
		Debuger::Log("OnDisable:%s", this->GetInstanceName().c_str());
	}
	virtual void OnTransformChanged()override
	{
		Debuger::Log("OnTransformChanged:%s", this->GetInstanceName().c_str());
	}
	virtual void OnDestroy()override
	{
		Debuger::Log("OnDestroy:%s", this->GetInstanceName().c_str());
	}
};
class TestPrim : public TestBase
{
	typedef TestBase base;
public:
	TestPrim()
	{
		std::function<void()> fun = CALLBACK_0(TestPrim::TestPoint, this);
		_menus.Add(std::make_pair("Point", fun));

		fun = CALLBACK_0(TestPrim::TestLine, this);
		_menus.Add(std::make_pair("Line", fun));

		fun = CALLBACK_0(TestPrim::TestQuad, this);
		_menus.Add(std::make_pair("Quad", fun));

		fun = CALLBACK_0(TestPrim::TestBox, this);
		_menus.Add(std::make_pair("Box", fun));

		fun = CALLBACK_0(TestPrim::TestSphere, this);
		_menus.Add(std::make_pair("Sphere", fun));

		fun = CALLBACK_0(TestPrim::TestPlane, this);
		_menus.Add(std::make_pair("Plane", fun));

		fun = CALLBACK_0(TestPrim::TestCustomPrim, this);
		_menus.Add(std::make_pair("CustomPrim", fun));

		fun = CALLBACK_0(TestPrim::TestSprite, this);
		_menus.Add(std::make_pair("Sprite", fun));

		fun = CALLBACK_0(TestPrim::TestBillboard, this);
		_menus.Add(std::make_pair("Billboard", fun));

		fun = CALLBACK_0(TestPrim::TestTrail, this);
		_menus.Add(std::make_pair("Trail", fun));

		fun = CALLBACK_0(TestPrim::TestInstance, this);
		_menus.Add(std::make_pair("Instance", fun));

		fun = CALLBACK_0(TestPrim::TestMeshFBX, this);
		_menus.Add(std::make_pair("Mesh(FBX)", fun));
	}
	virtual void OnEnter(int child_unit_index)override
	{
		base::OnEnter(child_unit_index);

		SceneManager::GetMainCamera()->SetClearColor(Color(58.0f / 255, 85.0f / 255, 128.0f / 255, 0));
		_rootObject = GameObject::Instantiation(this->GetTitle());
	}
	virtual void OnLeave()override
	{
		base::OnLeave();
	}
	virtual String GetTitle()override { return "Primitive"; }

	void TestPoint()
	{
		PointRender* mesh_render = nullptr;
		//point
		{
			GameObject* obj = GameObject::Instantiation("PointRender");
			this->AddGameObject(obj);
			mesh_render = obj->AddComponent<PointRender>();
			Vector3v vertexs = { Vector3(0.0f, 0.5f, 0.5f), Vector3(0.5f, -0.5f, 0.5f), Vector3(-0.5f, -0.5f, 0.5f) };
			mesh_render->SetVertexs(std::move(vertexs), Color::Red, 1.0f);
		}

		//界面
		_gUICallback = std::bind([mesh_render]
			{
				ImGui::Begin("TestPoint", 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysAutoResize);
				ImGui::SetWindowPos(ImVec2(Screen::GetWidth() - 300, 50));

				static float size = 1.0f;
				if (ImGui::SliderFloat("PointSize", &size, 1.0f, 10.0f))
				{
					Vector3v vertexs = { Vector3(0.0f, 0.5f, 0.5f), Vector3(0.5f, -0.5f, 0.5f), Vector3(-0.5f, -0.5f, 0.5f) };
					mesh_render->SetVertexs(std::move(vertexs), Color::Red, size);
				}

				ImGui::End();
			});
	}
	void TestLine()
	{
		{
			GameObject* obj = GameObject::Instantiation("LineRender");
			this->AddGameObject(obj);
			LineRender* mesh_render = obj->AddComponent<LineRender>();
			Vector3v vertexs = { Vector3(0.0f, 0.5f, 0.5f), Vector3(0.5f, -0.5f, 0.5f), Vector3(-0.5f, -0.5f, 0.5f),Vector3(0.0f, 0.5f, 0.5f) };
			mesh_render->SetVertexs(std::move(vertexs), Color::Red);
		}
		{//线list
			GameObject* obj = GameObject::Instantiation("LineRender");
			this->AddGameObject(obj);
			LineRender* mesh_render = obj->AddComponent<LineRender>();
			Vector3v vertexs = { Vector3(0.5f, 1.5f, 0.5f), Vector3(-0.5f, 1.5f, 0.5f), Vector3(0.5f, 2.5f, 0.5f),Vector3(-0.5f, 2.5f, 0.5f) };
			mesh_render->SetVertexs(std::move(vertexs), Color::Blue);
			mesh_render->SetPrimitiveType(PrimitiveType::LineList);
		}
		LineRender* mesh_render = nullptr;
		{//宽度
			GameObject* obj = GameObject::Instantiation("LineRender");
			this->AddGameObject(obj);
			mesh_render = obj->AddComponent<LineRender>();
			Vector3v vertexs = { Vector3(-1.0f, -1.5f, 0.0f), Vector3(1.0f, -1.5f, 0.0f) };
			mesh_render->SetVertexs(std::move(vertexs), Color::Blue);
			mesh_render->SetWidth(1.0f);
		}

		//界面
		_gUICallback = std::bind([mesh_render]
			{
				ImGui::Begin("TestLine", 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysAutoResize);
				ImGui::SetWindowPos(ImVec2(Screen::GetWidth() - 300, 50));

				static float line_width = 1.0f;
				if (ImGui::SliderFloat("LineWidth", &line_width, 0.1f, 10.0f))
				{
					mesh_render->SetWidth(line_width);
				}

				ImGui::End();
			});
	}
	void TestQuad()
	{
		{
			GameObject* obj = GameObject::Instantiation("");
			this->AddGameObject(obj);
			QuadRender * mesh_render = obj->AddComponent<QuadRender>();
			mesh_render->SetMaterial("internal/material/UnlitTexture.material");
			mesh_render->GetMaterial()->SetTextureAddress(TextureAddress::Wrap);
			mesh_render->GetMaterial()->SetDepth(true, true, StencilCmp::LEqual);
			mesh_render->SetColor(Color::Red);
			Transform *node = obj->GetComponent<Transform>();
			node->SetLocalPosition(Vector3(0, 0, 0));
			node->SetLocalScale(Vector3(5, 5, 5));
		}
		{
			GameObject* obj = GameObject::Instantiation("");
			this->AddGameObject(obj);
			QuadRender * mesh_render = obj->AddComponent<QuadRender>();
			mesh_render->SetMaterial("internal/material/UnlitTexture.material");
			mesh_render->GetMaterial()->SetTextureAddress(TextureAddress::Wrap);
			mesh_render->GetMaterial()->SetDepth(true, true, StencilCmp::LEqual);
			mesh_render->SetColor(Color::Blue);
			Transform *node = obj->GetComponent<Transform>();
			node->SetLocalPosition(Vector3(1, 0, 0));
			node->SetLocalScale(Vector3(5, 5, 5));
			node->DoRotateAround(Vector3::zero, Vector3::forward, 20);
		}
	}
	void TestBox()
	{
		//box
		{
			GameObject* obj = GameObject::Instantiation("CubeRender");
			this->AddGameObject(obj);
			CubeRender * mesh_render = obj->AddComponent<CubeRender>();
			mesh_render->SetMaterial("internal/material/UnlitTexture.material");
			mesh_render->GetMaterial()->SetTextureFilter(TextureFilter::Trilinear);
			mesh_render->GetMaterial()->SetTextureFile("texture/ac_rock.png");
			//mesh_render->GetMaterial()->SetTextureFile("texture/ac_rock.astc");		
			//mesh_render->GetMaterial()->SetTextureFile("texture/ac_rock.pkm");
			//mesh_render->GetMaterial()->SetTextureFile("texture/ac_rock.astc");
			mesh_render->GetMaterial()->SetAlphaBlend(true);
			mesh_render->GetMaterial()->SetAlphaTest(true, 0.1f);
		}
	}
	void TestSphere()
	{
		{
			GameObject* obj = GameObject::Instantiation("");
			this->AddGameObject(obj);
			SphereRender* mesh_render = obj->AddComponent<SphereRender>();
			mesh_render->SetMaterial("internal/material/UnlitTexture.material");
			Transform *node = obj->GetComponent<Transform>();
			//node->SetLocalScale(Vector3(2, 2, 2));
		}
	}
	void TestPlane()
	{
		{
			GameObject* obj = GameObject::Instantiation("");
			this->AddGameObject(obj);
			PlaneRender * mesh_render = obj->AddComponent<PlaneRender>();
			mesh_render->SetMaterial("internal/material/UnlitTexture.material");
			mesh_render->SetPlane(200, 200, 40, 40, 40, 40);
			Material* material = mesh_render->GetMaterial();
			material->SetTextureFilter(TextureFilter::Trilinear, 16);
			material->SetTextureAddress(TextureAddress::Wrap);
			material->SetTextureFile("texture/cao_01.jpg");
			material->SetRenderQueue(1900);
			Transform *node = obj->GetComponent<Transform>();
			node->Pitch(90);
			node->SetPosition(Vector3(0, -2, 0));
		}
		base::SetCameraCanMove(true);
	}
	void TestCustomPrim()
	{
		{
			GameObject* obj = GameObject::Instantiation("");
			this->AddGameObject(obj);
			ManualRender* mesh_render = obj->AddComponent<ManualRender>();
			mesh_render->SetMaterial("internal/material/UnlitColor.material");
			Vector3v vertexs = { Vector3(0.0f, 0.5f, 0.5f), Vector3(0.5f, -0.5f, 0.5f), Vector3(-0.5f, -0.5f, 0.5f) };
			mesh_render->SetVertexs(std::move(vertexs));
			mesh_render->SetColors({ 0xff00ff00 ,0xff00ff00 ,0xffff0000 });
			uint index[] = { 0,1,2 };
			mesh_render->SetIndices(3, index);
			mesh_render->UploadData();
		}
	}
	void TestSprite()
	{
		{
			GameObject* obj = GameObject::Instantiation("sprite");
			this->AddGameObject(obj);
			Sprite * mesh_render = obj->AddComponent<Sprite>();
			mesh_render->GetMaterial()->SetTextureFile("texture/box.jpg");
		}
	}
	void TestBillboard()
	{
		{//plane
			GameObject* obj = GameObject::Instantiation("");
			this->AddGameObject(obj);
			PlaneRender * mesh_render = obj->AddComponent<PlaneRender>();
			mesh_render->SetMaterial("internal/material/UnlitTexture.material");
			mesh_render->SetPlane(50, 50, 10, 10, 10, 10);
			Material* material = mesh_render->GetMaterial();
			material->SetTextureFilter(TextureFilter::Trilinear, 16);
			material->SetTextureAddress(TextureAddress::Wrap);
			material->SetTextureFile("texture/cao_01.jpg");
			material->SetRenderQueue(1900);
			Transform *node = obj->GetComponent<Transform>();
			node->SetLocalPosition(Vector3(0, -1, 0));
			node->Pitch(90);
		}
		{//tree
			Texture* tex = Texture::Create("texture/terrain/fld_cao04_z.png");

			for (int row = -5; row <= 5; row += 5)
			{
				for (int col = -5; col <= 5; col += 5)
				{
					GameObject* obj = GameObject::Instantiation("billboard");
					this->AddGameObject(obj);
					Billboard * mesh_render = obj->AddComponent<Billboard>();
					mesh_render->SetMaterial("internal/material/UnlitTexture.material");
					mesh_render->GetMaterial()->SetAlphaBlend(true);
					mesh_render->GetMaterial()->SetAlphaTest(true, 0.1f);
					mesh_render->GetMaterial()->SetTexture(tex);
					Transform *node = obj->GetComponent<Transform>();
					node->SetLocalPosition(Vector3(col, 0, row));
				}
			}
		}
		{
			Camera* camera = SceneManager::GetMainCamera();
			camera->GetTransform()->DoRotateAround(Vector3::zero, Vector3::up, 20);
		}
	}
	void TestTrail()
	{
		{
			GameObject* obj = GameObject::Instantiation("");
			this->AddGameObject(obj);
			TrailRender * mesh_render = obj->AddComponent<TrailRender>();
			mesh_render->SetMaterial("internal/material/UnlitTexture.material");
			mesh_render->SetWidth(1.0f, 0.5f);
			mesh_render->SetDurationTime(1.0f);
			Material* material = mesh_render->GetMaterial();
			material->SetTextureAddress(TextureAddress::Wrap);
			material->SetTextureFile("texture/trail.png");
			material->SetAlphaBlend(true);
			material->SetCullMode(CullMode::Back);
			Transform *node = obj->GetComponent<Transform>();
			node->SetLocalPosition(Vector3(1, 0, 0));
			node->DoRotateAround(Vector3::zero, Vector3::forward, 20);
			//node->DOMove(Vector3(-2, 0, 0), 1);
			node->DoRotateAround(Vector3::zero, Vector3::up, 20);
		}
	}
	void TestInstance()
	{
		{//plane
			GameObject* obj = GameObject::Instantiation("");
			this->AddGameObject(obj);
			PlaneRender * mesh_render = obj->AddComponent<PlaneRender>();
			mesh_render->SetMaterial("internal/material/UnlitTexture.material");
			mesh_render->SetPlane(50, 50, 10, 10, 10, 10);
			Material* material = mesh_render->GetMaterial();
			material->SetTextureFilter(TextureFilter::Trilinear, 16);
			material->SetTextureAddress(TextureAddress::Wrap);
			material->SetTextureFile("texture/cao_01.jpg");
			material->SetRenderQueue(1900);
			Transform *node = obj->GetComponent<Transform>();
			node->SetLocalPosition(Vector3(0, -1, 0));
			node->Pitch(90);
		}
		{
			GameObject* obj = GameObject::Instantiation("CubeRender");
			this->AddGameObject(obj);
			CubeRender * mesh_render = obj->AddComponent<CubeRender>();

			Vector<Matrix4> mats;
			for (int row = -5; row <= 5; row += 2)
			{
				for (int col = -5; col <= 5; col += 2)
				{
					Matrix4 mat; mat.Translate(col, 0, row); mats.Add(mat);
				}
			}
			mesh_render->SetInstanceTransform(mats);

			mesh_render->SetMaterial("internal/material/UnlitTextureInstance.material");
			mesh_render->GetMaterial()->SetTextureFile("texture/ac_rock.png");
		}
		{
			Camera* camera = SceneManager::GetMainCamera();
			camera->GetTransform()->DoRotateAround(Vector3::zero, Vector3::up, 20);
		}
	}
	void TestMeshFBX()
	{
		{
			GameObject* obj = GameObject::Instantiation("");
			this->AddGameObject(obj);
			MeshRender * mesh_render = obj->AddComponent<MeshRender>();
			mesh_render->SetMesh(Mesh::Create("model/F1001_WU01.FBX"));
			mesh_render->SetMaterial("internal/material/UnlitTexture.material");
			mesh_render->GetMaterial(0)->SetTextureFile("model/F1001_WU02_D.png");
			mesh_render->GetMaterial(1)->SetTextureFile("model/F1001_WU01_D.png");
			mesh_render->GetMaterial(2)->SetTextureFile("model/F1001_WU01_D.png");
			mesh_render->GetMaterial(3)->SetTextureFile("model/F1001_WU01_D.png");
			mesh_render->GetMaterial(4)->SetTextureFile("model/F1001_WU02_D.png");
			mesh_render->GetMaterial(5)->SetTextureFile("model/F1001_WU01_D.png");
			mesh_render->GetMaterial(6)->SetTextureFile("model/F1001_WU02_D.png");
			mesh_render->GetMaterial(7)->SetTextureFile("model/F1001_WU01_D.png");
			Transform *node = obj->GetComponent<Transform>();
			node->SetLocalScale(Vector3(0.001f, 0.001f, 0.001f));
			node->SetLocalPosition(Vector3(0, -2, 0));
		}
		{
			Camera* camera = SceneManager::GetMainCamera();
			camera->GetTransform()->DoRotateAround(Vector3::zero, Vector3::up, 20);
		}
	}
	void TestParticleMaterial()
	{
		{
			GameObject* obj = GameObject::Instantiation("CubeRender");
			this->AddGameObject(obj);
			QuadRender* mesh_render = obj->AddComponent<QuadRender>();
			mesh_render->SetMaterial("internal/material/ParticleAdd.material");
			mesh_render->GetMaterial()->SetTextureFile("texture/wenhao.png");
			mesh_render->GetMaterial()->SetTextureAddress(TextureAddress::Wrap);
			Transform* node = obj->GetComponent<Transform>();
			node->SetLocalPosition(Vector3(1, 0, 0));
			node->SetLocalScale(Vector3(2, 2, 2));
		}
		{
			GameObject* obj = GameObject::Instantiation("CubeRender");
			this->AddGameObject(obj);
			QuadRender* mesh_render = obj->AddComponent<QuadRender>();
			mesh_render->SetMaterial("internal/material/ParticleAlphaBlend.material");
			mesh_render->GetMaterial()->SetTextureFile("texture/wenhao.png");
			Transform* node = obj->GetComponent<Transform>();
			node->SetLocalPosition(Vector3(-1, 0, 0));
			node->SetLocalScale(Vector3(2, 2, 2));
		}
		{
			GameObject* obj = GameObject::Instantiation("CubeRender");
			this->AddGameObject(obj);
			QuadRender* mesh_render = obj->AddComponent<QuadRender>();
			mesh_render->SetMaterial("internal/material/ParticleMultiply.material");
			mesh_render->GetMaterial()->SetTextureFile("texture/wenhao.png");
			Transform* node = obj->GetComponent<Transform>();
			node->SetLocalPosition(Vector3(0, 1, 0));
			node->SetLocalScale(Vector3(2, 2, 2));
		}
	}
};
DC_END_NAMESPACE