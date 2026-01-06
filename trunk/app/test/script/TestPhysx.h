 
#pragma once

#include "TestBase.h"

DC_BEGIN_NAMESPACE
class TestPhysicsContact : public Component
{
public:
	TestPhysicsContact() {}
	~TestPhysicsContact() {}
protected:
	virtual void OnCollisionEnter(Collision2D* collisionInfo) 
	{
		Debuger::Log("enter");
	}
	virtual void OnCollisionStay(Collision2D* collisionInfo) 
	{
		Debuger::Log("stay");
		static int count = 0;
		if (++count == 10)
		{
			GameObject::Destroy(GameObject::Find("floor"));
		}
	}
	virtual void OnCollisionExit(Collision2D* collisionInfo) 
	{
		Debuger::Log("exit");
	}
	virtual void Update()
	{
		if (Input::GetMouseButtonDown(MouseBtnID::Left))
		{
			Camera* camera = SceneManager::GetMainCamera();
			Vector3 v_vec = camera->ScreenToWorldPoint(Vector3(Input::mousePosition.x, Input::mousePosition.y, 10));
			RaycastHit hitInfo;
			if (Physics::Raycast(Vector3::zero, v_vec, v_vec.Lenght(), LayerMask::GetMask(LayerMask::Default), hitInfo))
			{
				hitInfo.gameobject->GetComponent<RigidBody>()->SetActive(false);
				Debuger::Log("raycast:%s", hitInfo.gameobject->GetInstanceName().c_str());


				RigidBody* rigidBody = hitInfo.gameobject->GetComponent<RigidBody>();
				rigidBody->AppleTorque(Vector3(2000, 0, 0));
			}
		}
	}
	virtual void OnDrawGizmos(Camera* camera)
	{
		//Gizmos::SetColor(Color::Red);
		//Gizmos::SetMatrix(this->GetTransform()->GetLocalToWorldMatrix());
		//const Aabb& aabb = this->GetTransform()->GetLocalBoundingBox();
		////Gizmos::DrawCube(camera, Vector3::zero, aabb.GetSize());
		////Gizmos::DrawSphere(camera, Vector3::zero, aabb.GetSize().x*0.5f);
		//Gizmos::DrawRect(camera, Vector3::zero, Vector2(aabb.GetSize().x, aabb.GetSize().y));
		////Gizmos::DrawCircle(camera, Vector3::zero, aabb.GetSize().x*0.5f);

		////std::vector<Vector3> list;
		////list.push_back(Vector3(0, 0, 0));
		////list.push_back(Vector3(0, 1, 0));
		////list.push_back(Vector3(1, 1, 0));
		////list.push_back(Vector3(1, 0, 0));
		////list.push_back(Vector3(0, 0, 0));
		////Gizmos::DrawPolygon(camera, list);

		////Gizmos::DrawTransform(camera, 2);
	}
};

class TestPhysx : public TestBase
{
	typedef TestBase base;
public:
	TestPhysx()
	{
		std::function<void()> fun = CALLBACK_0(TestPhysx::TestBoxCollider, this);
		_menus.Add(std::make_pair("BoxCollider", fun));
	}
	virtual void OnEnter(int child_unit_index)override
	{
		base::OnEnter(child_unit_index);

		Physics::EnableDebug(true);
		_rootObject = GameObject::Instantiation(this->GetTitle());
	}
	virtual void OnLeave()override
	{
		base::OnLeave();
	}
	virtual String GetTitle()override { return "Physx"; }

	void TestBoxCollider()
	{
		{
			GameObject* obj = GameObject::Instantiation("floor");
			this->AddGameObject(obj);
			obj->GetTransform()->SetLocalPosition(Vector3(0, -3, 0));
			obj->GetTransform()->Pitch(90);
			obj->AddComponent<TestPhysicsContact>();

			PlaneCollider* collider = obj->AddComponent<PlaneCollider>();

			PlaneRender * mesh_render = obj->AddComponent<PlaneRender>();
			mesh_render->SetMaterial("internal/material/UnlitTexture.material");
			mesh_render->GetMaterial()->SetTextureFile("texture/cao_01.jpg");
		}

		{
			GameObject* obj = GameObject::Instantiation("sprite1");
			this->AddGameObject(obj);
			obj->GetTransform()->SetLocalPosition(Vector3(2, 0, 0));

			BoxCollider* collider = obj->AddComponent<BoxCollider>();
			RigidBody* rigidBody = obj->AddComponent<RigidBody>();

			CubeRender * mesh_render = obj->AddComponent<CubeRender>();
			mesh_render->SetMaterial("internal/material/UnlitTexture.material");
			mesh_render->GetMaterial()->SetTextureFile("texture/cao_01.jpg");
		}
		{
			GameObject* obj = GameObject::Instantiation("sprite2");
			this->AddGameObject(obj);
			obj->GetTransform()->SetLocalPosition(Vector3(-2, 0, 0));

			BoxCollider* collider = obj->AddComponent<BoxCollider>();
			RigidBody* rigidBody = obj->AddComponent<RigidBody>();

			CubeRender * mesh_render = obj->AddComponent<CubeRender>();
			mesh_render->SetMaterial("internal/material/UnlitTexture.material");
			mesh_render->GetMaterial()->SetTextureFile("texture/cao_01.jpg");
		}
	}
};
DC_END_NAMESPACE
