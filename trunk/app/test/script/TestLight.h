 #pragma once

#include "TestBase.h"

DC_BEGIN_NAMESPACE
class TestLight : public TestBase
{
	typedef TestBase base;
public:
	TestLight()
	{
		std::function<void()> fun = CALLBACK_0(TestLight::TestDirectioin, this);
		_menus.Add(std::make_pair("Direction", fun));

		fun = CALLBACK_0(TestLight::TestPoint, this);
		_menus.Add(std::make_pair("Point", fun));

		fun = CALLBACK_0(TestLight::TestSpot, this);
		_menus.Add(std::make_pair("Spot", fun));
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
	virtual String GetTitle()override { return "Light"; }

	//normap
	void TestDirectioin()
	{
		MeshRender* cube_render = nullptr;
		{
			GameObject* obj = GameObject::Instantiation("");
			this->AddGameObject(obj);
			cube_render = obj->AddComponent<MeshRender>();
			cube_render->SetMesh(Mesh::Create("internal/mesh/cube.fbx"));
			cube_render->SetMaterial("internal/material/NormalMap.material");
			cube_render->GetMaterial()->SetTextureFile("texture/terrain/dibiao3.png", TextureType::D2, TextureFlag::sRGB, 0, 0);
			cube_render->GetMaterial()->SetTextureFile("texture/terrain/dibiao3_n.png", TextureType::D2, TextureFlag::NONE, 0, 1);
			Transform *node = obj->GetComponent<Transform>();
			node->SetLocalPosition(Vector3(1, 0, 0));

			RotateComponent* rotate = obj->AddComponent<RotateComponent>();
			rotate->SetRotate(Vector3(0, 36, 0));
		}
		PlaneRender* plane_render = nullptr;
		{
			GameObject* obj = GameObject::Instantiation("");
			this->AddGameObject(obj);
			plane_render = obj->AddComponent<PlaneRender>();
			plane_render->SetPlane(30, 30, 4, 4, 4, 4);
			plane_render->SetMaterial("internal/material/NormalMapSpec.material");
			plane_render->GetMaterial()->SetTextureFile("texture/terrain/dibiao3.png", TextureType::D2, TextureFlag::sRGB, 0, 0);
			plane_render->GetMaterial()->SetTextureFile("texture/terrain/dibiao3_n.png", TextureType::D2, TextureFlag::NONE, 0, 1);
			plane_render->GetMaterial()->SetTextureFilter(TextureFilter::Trilinear, 16);
			plane_render->GetMaterial()->SetTextureAddress(TextureAddress::Wrap, Vector2::one, 0, 0);
			plane_render->GetMaterial()->SetTextureAddress(TextureAddress::Wrap, Vector2::one, 0, 1);
			plane_render->GetMaterial()->SetRenderQueue(1900);
			Transform *node = obj->GetComponent<Transform>();
			node->Pitch(90);
			node->SetPosition(Vector3(0, -4, 0));
		}
		{
			GameObject* obj = GameObject::Instantiation("MainLight");
			obj->GetTransform()->SetPosition(Vector3(0.0f, 10.0f, -10.0f));
			obj->GetTransform()->LookTo(Vector3(1.0f, -1.0, 1.0f), Vector3::up);
			this->AddGameObject(obj);
			Light* light = obj->AddComponent<Light>();
			light->mType = LightType::Direction;
			light->Diffuse = Color(1.0f, 1.0f, 1.0f);
		}
		//界面
		_gUICallback = std::bind([cube_render, plane_render]
			{
				ImGui::Begin("TestDirectioin", 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysAutoResize);
				ImGui::SetWindowPos(ImVec2(Screen::GetWidth() - 400, 50));

				static bool enable_power = false;
				static ImVec2 size(110, 40);
				if (ImGui::Button("UnlitTexture", size))
				{
					{
						cube_render->SetMaterial("internal/material/UnlitTexture.material");
						cube_render->GetMaterial()->SetTextureFile("texture/terrain/dibiao3.png");
					}
					{
						plane_render->SetMaterial("internal/material/UnlitTexture.material");
						plane_render->GetMaterial()->SetTextureFile("texture/terrain/dibiao3.png");
						plane_render->GetMaterial()->SetTextureFilter(TextureFilter::Trilinear, 16);
						plane_render->GetMaterial()->SetTextureAddress(TextureAddress::Wrap, Vector2::one, 0, 0);
						plane_render->GetMaterial()->SetTextureAddress(TextureAddress::Wrap, Vector2::one, 0, 1);
						plane_render->GetMaterial()->SetRenderQueue(1900);
					}
					enable_power = false;
				}
				ImGui::SameLine(120);
				if (ImGui::Button("NormalTexture", size))
				{
					{
						cube_render->SetMaterial("internal/material/NormalMap.material");
						cube_render->GetMaterial()->SetTextureFile("texture/terrain/dibiao3.png", TextureType::D2, TextureFlag::sRGB, 0, 0);
						cube_render->GetMaterial()->SetTextureFile("texture/terrain/dibiao3_n.png", TextureType::D2, TextureFlag::NONE, 0, 1);
					}
					{
						plane_render->SetMaterial("internal/material/NormalMap.material");
						plane_render->GetMaterial()->SetTextureFile("texture/terrain/dibiao3.png", TextureType::D2, TextureFlag::sRGB, 0, 0);
						plane_render->GetMaterial()->SetTextureFile("texture/terrain/dibiao3_n.png", TextureType::D2, TextureFlag::NONE, 0, 1);
						plane_render->GetMaterial()->SetTextureFilter(TextureFilter::Trilinear, 16);
						plane_render->GetMaterial()->SetTextureAddress(TextureAddress::Wrap, Vector2::one, 0, 0);
						plane_render->GetMaterial()->SetTextureAddress(TextureAddress::Wrap, Vector2::one, 0, 1);
						plane_render->GetMaterial()->SetRenderQueue(1900);
					}
					enable_power = false;
				}
				ImGui::SameLine(232);
				if (ImGui::Button("SpecNormalTexture", size))
				{
					{
						cube_render->SetMaterial("internal/material/NormalMapSpec.material");
						cube_render->GetMaterial()->SetTextureFile("texture/terrain/dibiao3.png", TextureType::D2, TextureFlag::sRGB, 0, 0);
						cube_render->GetMaterial()->SetTextureFile("texture/terrain/dibiao3_n.png", TextureType::D2, TextureFlag::NONE, 0, 1);
					}
					{
						plane_render->SetMaterial("internal/material/NormalMapSpec.material");
						plane_render->GetMaterial()->SetTextureFile("texture/terrain/dibiao3.png", TextureType::D2, TextureFlag::sRGB, 0, 0);
						plane_render->GetMaterial()->SetTextureFile("texture/terrain/dibiao3_n.png", TextureType::D2, TextureFlag::NONE, 0, 1);
						plane_render->GetMaterial()->SetTextureFilter(TextureFilter::Trilinear, 16);
						plane_render->GetMaterial()->SetTextureAddress(TextureAddress::Wrap, Vector2::one, 0, 0);
						plane_render->GetMaterial()->SetTextureAddress(TextureAddress::Wrap, Vector2::one, 0, 1);
						plane_render->GetMaterial()->SetRenderQueue(1900);
					}
					enable_power = true;
				}
				if (enable_power)
				{
					static float power = 0.5f;
					if (ImGui::SliderFloat("SpecularPower", &power, 0.0f, 1.0f))
					{
						cube_render->GetMaterial()->SetFloat("_SpecularPower", power);
						plane_render->GetMaterial()->SetFloat("_SpecularPower", power);
					}
				}
				ImGui::End();
			});
	}
	//点光源
	void TestPoint()
	{
		{
			GameObject* obj = GameObject::Instantiation("");
			this->AddGameObject(obj);
			PlaneRender * mesh_render = obj->AddComponent<PlaneRender>();
			mesh_render->SetMaterial("internal/material/Standard.material");
			mesh_render->SetPlane(20, 20, 10, 10, 10, 10);
			Material* material = mesh_render->GetMaterial();
			material->SetTextureFile("texture/terrain/dibiao3.png", TextureType::D2, TextureFlag::sRGB, 0, 0);
			material->SetTextureFile("texture/terrain/dibiao3_n.png", TextureType::D2, TextureFlag::NONE, 0, 1);
			material->SetTextureFilter(TextureFilter::Trilinear, 5);
			material->SetTextureAddress(TextureAddress::Wrap);
			material->SetTextureAddress(TextureAddress::Wrap, Vector2::one, 0, 1);
			material->SetRenderQueue(1900);
			Transform *node = obj->GetComponent<Transform>();
			node->Pitch(90);
			node->SetPosition(Vector3(0, -2, 0));
		}
		{
			GameObject* obj = GameObject::Instantiation("PointLight");
			obj->GetTransform()->SetPosition(Vector3(0.0f, 3.0f, 0.0f));
			obj->GetTransform()->LookTo(Vector3(0.0f, -1.0f, 0.0f), Vector3::up);
			this->AddGameObject(obj);
			Light* light = obj->AddComponent<Light>();
			light->mType = LightType::Point;
			light->Diffuse = Color(0.0f, 1.0f, 0.0f);
			light->Range = 10;
			light->Intensity = 5.0f;
		}
	}
	//聚光灯
	void TestSpot()
	{
		{
			GameObject* obj = GameObject::Instantiation("");
			this->AddGameObject(obj);
			PlaneRender * mesh_render = obj->AddComponent<PlaneRender>();
			mesh_render->SetMaterial("internal/material/Standard.material");
			mesh_render->SetPlane(20, 20, 10, 10, 10, 10);
			Material* material = mesh_render->GetMaterial();
			material->SetTextureFile("texture/terrain/dibiao3.png", TextureType::D2, TextureFlag::sRGB, 0, 0);
			material->SetTextureFile("texture/terrain/dibiao3_n.png", TextureType::D2, TextureFlag::NONE, 0, 1);
			material->SetTextureFilter(TextureFilter::Trilinear, 5);			
			material->SetTextureAddress(TextureAddress::Wrap);
			material->SetTextureAddress(TextureAddress::Wrap, Vector2::one, 0, 1);
			material->SetRenderQueue(1900);
			Transform *node = obj->GetComponent<Transform>();
			node->Pitch(90);
			node->SetPosition(Vector3(0, -2, 0));
		}
		{
			GameObject* obj = GameObject::Instantiation("SpotLight");
			obj->GetTransform()->SetPosition(Vector3(0.0f, 3.0f, 0.0f));
			obj->GetTransform()->LookTo(Vector3(0.0f, -1.0f, 0.0f), Vector3::up);
			this->AddGameObject(obj);
			Light* light = obj->AddComponent<Light>();
			light->mType = LightType::Spot;
			light->Diffuse = Color(1.0f, 0.0f, 0.0f);
			light->Range = 10;
			light->SpotAngle = 60;
			light->Intensity = 5.0f;
		}
	}
};
DC_END_NAMESPACE
