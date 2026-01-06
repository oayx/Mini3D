 
#pragma once

#include "TestBase.h"

DC_BEGIN_NAMESPACE
class TestPhysicsContact2d : public Component
{
public:
	TestPhysicsContact2d() {}
	~TestPhysicsContact2d() {}
protected:
	virtual void OnCollisionEnter2D(Collision2D* collisionInfo)override
	{
		Debuger::Log("enter");
	}
	virtual void OnCollisionStay2D(Collision2D* collisionInfo)override
	{
		Debuger::Log("stay");
		static int count = 0;
		if (++count == 10)
		{
			GameObject::Destroy(GameObject::Find("TestPhysx2d/floor"));
		}
	}
	virtual void OnCollisionExit2D(Collision2D* collisionInfo)override
	{
		Debuger::Log("exit");
	}
	virtual void Update()override
	{
		if (Input::GetMouseButtonDown(MouseBtnID::Left))
		{
			Camera* camera = SceneManager::GetMainCamera();
			Vector3 v_vec = camera->ScreenToWorldPoint(Vector3(Input::mousePosition.x, Input::mousePosition.y, 10));
			RaycastHit2D hitInfo;
			if (Physics2d::Raycast(Vector2::zero, Vector2(v_vec.x, v_vec.y), v_vec.Lenght(), LayerMask::GetMask(LayerMask::Default), hitInfo))
			{
				Debuger::Log("raycast:%s", hitInfo.gameobject->GetInstanceName().c_str());
			}

			//RigidBody2d* rigidBody = this->GetGameObject()->GetComponent<RigidBody2d>();
			//rigidBody->ApplyForce(Vector2(0, 200));
		}
	}
	virtual void OnDrawGizmos(Camera* camera)override
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

class TestPhysx2d : public TestBase
{
	typedef TestBase base;

public:
	TestPhysx2d()
	{
		std::function<void()> fun = CALLBACK_0(TestPhysx2d::TestBoxCollider, this);
		_menus.Add(std::make_pair("BoxCollider", fun));
	}
	virtual void OnEnter(int child_unit_index)override
	{
		base::OnEnter(child_unit_index);

		Physics2d::EnableDebug(true);
		Physics2d::SetUnitRatio(1.0f);
		_rootObject = GameObject::Instantiation(this->GetTitle());
	}
	virtual void OnLeave()override
	{
		base::OnLeave();
	}
	virtual String GetTitle()override { return "Physx2d"; }

	void TestBoxCollider()
	{
		Texture* tex = Texture::Create("texture/cao_01.jpg");
		{
			GameObject* obj = GameObject::Instantiation("floor");
			this->AddGameObject(obj);
			obj->GetTransform()->SetLocalPosition(Vector3(0, -3.2f, 0));
			obj->GetTransform()->SetScale(Vector3(1, 0.01f, 1));
			//obj->AddComponent<TestPhysicsContact2d>();

			BoxCollider2d* collider = obj->AddComponent<BoxCollider2d>();
			collider->SetScale(1, 0.0001f);
			collider->SetGroupIndex(-1);

			Sprite * floor = obj->AddComponent<Sprite>();
			floor->SetTexture(tex);
		}

		{
			GameObject* obj = GameObject::Instantiation("sprite");
			this->AddGameObject(obj);
			obj->GetTransform()->SetLocalPosition(Vector3(0, 0, 0));
			obj->GetTransform()->SetScale(Vector3(0.5f, 0.5f, 0.5f));
			//obj->GetTransform()->Roll(30);
			obj->AddComponent<TestPhysicsContact2d>();

			Sprite * mesh = obj->AddComponent<Sprite>();
			mesh->SetTexture(tex);

			BoxCollider2d* collider = obj->AddComponent<BoxCollider2d>();

			//CircleCollider2d* collider = obj->AddComponent<CircleCollider2d>();
			//collider->SetGroupIndex(-1);

			//PolygonCollider2d* collider = obj->AddComponent<PolygonCollider2d>();
			//collider->SetVertexs({ Vector2(-1,1), Vector2(1,1), Vector2(1,-1), Vector2(-1,-1) });

			RigidBody2d* rigidBody = obj->AddComponent<RigidBody2d>();
			rigidBody->SetCollisionDetection(CollisionDetection2d::Continuous);
		}
	}
};
DC_END_NAMESPACE
