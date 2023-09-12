 
#pragma once

#include "TestBase.h"

DC_BEGIN_NAMESPACE
class TestTerrain : public TestBase
{
	typedef TestBase base;

public:
	virtual void OnEnter(int child_unit_index)override
	{
		base::OnEnter(child_unit_index);
		this->SetCameraCanMove(true);
		this->SetCameraSpeed(5.0f);
		_rootObject = GameObject::Instantiation(this->GetTitle());
		this->Test();
	}
	virtual void OnLeave()override 
	{
		base::OnLeave();
	}
	virtual String GetTitle()override { return "Terrain"; }

private:
	void Test()
	{
		{
			GameObject* obj = GameObject::Instantiation("DirectionLight");
			obj->GetTransform()->LookTo(Vector3(1.0f, -1.0, 0.0f), Vector3::up);
			this->AddGameObject(obj);
			Light* light = obj->AddComponent<Light>();
			light->mType = LightType::Direction;
			light->Diffuse = Color(1.0f, 1.0f, 1.0f);
		}
		//terrain
		{
			GameObject* obj = GameObject::Instantiation("Terrain");
			this->AddGameObject(obj);
			Terrain* mesh_render = obj->AddComponent<Terrain>();
			mesh_render->SetHeightMap("texture/heightmap/height.png", 50.0f, 2, 2);
			Material* material = mesh_render->SetMaterial("internal/material/Terrain.material");
			Pass* pass = material->GetPass();
			{
				material->SetTextureFile("texture/terrain/fld_dx01.png", TextureType::D2, TextureFlag::sRGB, 0, 0);
				material->SetTextureFile("texture/terrain/fld_dx01_n.png", TextureType::D2, TextureFlag::NONE, 0, 1);
				pass->SetTextureAddress(0, TextureAddress::Wrap, Vector2(30, 30));
				pass->SetTextureAddress(1, TextureAddress::Wrap, Vector2(30, 30));
				pass->SetTextureFilter(0, TextureFilter::Trilinear, 16);
				pass->SetTextureFilter(1, TextureFilter::Trilinear, 16);
			}
			//{
			//	material->SetTextureFile("texture/terrain/fld_dx02.png", TextureType::D2, TextureFlag::sRGB, 0, 2);
			//	material->SetTextureFile("texture/terrain/fld_dx02_n.png", TextureType::D2, TextureFlag::NONE, 0, 3);
			//	pass->SetTextureAddress(2, TextureAddress::Wrap, Vector2(30, 30));
			//	pass->SetTextureAddress(3, TextureAddress::Wrap, Vector2(30, 30));
			//	pass->SetTextureFilter(2, TextureFilter::Trilinear, 16);
			//	pass->SetTextureFilter(3, TextureFilter::Trilinear, 16);
			//}
			//{
			//	material->SetTextureFile("texture/terrain/fld_dx03.png", TextureType::D2, TextureFlag::sRGB, 0, 4);
			//	material->SetTextureFile("texture/terrain/fld_dx03_n.png", TextureType::D2, TextureFlag::NONE, 0, 5);
			//	pass->SetTextureAddress(4, TextureAddress::Wrap, Vector2(30, 30));
			//	pass->SetTextureAddress(5, TextureAddress::Wrap, Vector2(30, 30));
			//	pass->SetTextureFilter(4, TextureFilter::Trilinear, 16);
			//	pass->SetTextureFilter(5, TextureFilter::Trilinear, 16);
			//}
			//{
			//	material->SetTextureFile("texture/terrain/fenglingdu01.png", TextureType::D2, TextureFlag::NONE, 0, 6);
			//	pass->SetTextureAddress(6, TextureAddress::Clamp);
			//}
			obj->GetTransform()->SetScale(Vector3(3, 1, 3));
			obj->GetTransform()->SetPosition(Vector3(0, -50, 0));
			obj->GetTransform()->Yaw(90);
		}
	}
};
DC_END_NAMESPACE
