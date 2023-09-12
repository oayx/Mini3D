 
#pragma once

#include "TestBase.h"

DC_BEGIN_NAMESPACE
class TestParticle : public TestBase
{
	typedef TestBase base;

public:
	TestParticle()
	{
		std::function<void()> fun = CALLBACK_0(TestParticle::TestShape_Sphere, this);
		_menus.Add(std::make_pair("Shape(Sphere)", fun));

		fun = CALLBACK_0(TestParticle::TestShape_SphereShell, this);
		_menus.Add(std::make_pair("Shape(SphereShell)", fun));

		fun = CALLBACK_0(TestParticle::TestShape_HemiSphere, this);
		_menus.Add(std::make_pair("Shape(HemiSphere)", fun));

		fun = CALLBACK_0(TestParticle::TestShape_HemiSphereShell, this);
		_menus.Add(std::make_pair("Shape(HemiSphereShell)", fun));

		fun = CALLBACK_0(TestParticle::TestShape_Cone, this);
		_menus.Add(std::make_pair("Shape(Cone)", fun));

		fun = CALLBACK_0(TestParticle::TestShape_Box, this);
		_menus.Add(std::make_pair("Shape(Box)", fun));

		fun = CALLBACK_0(TestParticle::TestShape_BoxEdge, this);
		_menus.Add(std::make_pair("Shape(BoxEdge)", fun));

		fun = CALLBACK_0(TestParticle::TestShape_Circle, this);
		_menus.Add(std::make_pair("Shape(Circle)", fun));

		fun = CALLBACK_0(TestParticle::TestShape_CircleEdge, this);
		_menus.Add(std::make_pair("Shape(CircleEdge)", fun));

		fun = CALLBACK_0(TestParticle::Test1, this);
		_menus.Add(std::make_pair("Test1", fun));
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
	virtual String GetTitle()override { return "Particle"; }

private:
	void TestShape_Sphere()
	{
		GameObject* obj = GameObject::Instantiation();
		this->AddGameObject(obj);
		ParticleSystem* ps = obj->AddComponent<ParticleSystem>();
		ps->Main.Loop = true;
		ps->Main.StartLifeTime = 5;
		ps->Main.StartSpeed = 0;
		ps->Main.ScalingMode = PScalingMode::Local;
		ps->Main.Space = SimulationSpace::World;
		ps->Main.GravityModifier = 0.0f;

		ps->Emission.RateOverTime = 10;

		ps->Shape.Type = PShapeType::Sphere;
		ps->Shape.Scale = Vector3(2, 2, 2);

		ParticleRender* pr = obj->GetComponent<ParticleRender>();
		Material* mat = pr->SetMaterial("internal/material/ParticleAdd.material");
		mat->SetTextureFile("internal/texture/Default-Particle.png");
	}
	void TestShape_SphereShell()
	{
		GameObject* obj = GameObject::Instantiation();
		this->AddGameObject(obj);
		ParticleSystem* ps = obj->AddComponent<ParticleSystem>();
		ps->Main.Loop = true;
		ps->Main.StartLifeTime = 5;
		ps->Main.StartSpeed = 0;
		ps->Main.ScalingMode = PScalingMode::Local;
		ps->Main.Space = SimulationSpace::World;
		ps->Main.GravityModifier = 0.0f;

		ps->Emission.RateOverTime = 10;

		ps->Shape.Type = PShapeType::SphereShell;
		ps->Shape.Scale = Vector3(2, 2, 2);

		ParticleRender* pr = obj->GetComponent<ParticleRender>();
		Material* mat = pr->SetMaterial("internal/material/ParticleAdd.material");
		mat->SetTextureFile("internal/texture/Default-Particle.png");
	}
	void TestShape_HemiSphere()
	{
		GameObject* obj = GameObject::Instantiation();
		this->AddGameObject(obj);
		ParticleSystem* ps = obj->AddComponent<ParticleSystem>();
		ps->Main.Loop = true;
		ps->Main.StartLifeTime = 5;
		ps->Main.StartSpeed = 0;
		ps->Main.ScalingMode = PScalingMode::Local;
		ps->Main.Space = SimulationSpace::World;
		ps->Main.GravityModifier = 0.0f;

		ps->Emission.RateOverTime = 10;

		ps->Shape.Type = PShapeType::HemiSphere;
		ps->Shape.Scale = Vector3(2, 2, 2);

		ParticleRender* pr = obj->GetComponent<ParticleRender>();
		Material* mat = pr->SetMaterial("internal/material/ParticleAdd.material");
		mat->SetTextureFile("internal/texture/Default-Particle.png");

		Transform *node = obj->GetComponent<Transform>();
		node->Pitch(-90);
	}
	void TestShape_HemiSphereShell()
	{
		GameObject* obj = GameObject::Instantiation();
		this->AddGameObject(obj);
		ParticleSystem* ps = obj->AddComponent<ParticleSystem>();
		ps->Main.Loop = true;
		ps->Main.StartLifeTime = 5;
		ps->Main.StartSpeed = 0;
		ps->Main.ScalingMode = PScalingMode::Local;
		ps->Main.Space = SimulationSpace::World;
		ps->Main.GravityModifier = 0.0f;

		ps->Emission.RateOverTime = 10;

		ps->Shape.Type = PShapeType::HemiSphereShell;
		ps->Shape.Scale = Vector3(2, 2, 2);

		ParticleRender* pr = obj->GetComponent<ParticleRender>();
		Material* mat = pr->SetMaterial("internal/material/ParticleAdd.material");
		mat->SetTextureFile("internal/texture/Default-Particle.png");

		Transform *node = obj->GetComponent<Transform>();
		node->Pitch(-90);
	}
	void TestShape_Cone()
	{
		GameObject* obj = GameObject::Instantiation();
		this->AddGameObject(obj);
		ParticleSystem* ps = obj->AddComponent<ParticleSystem>();
		ps->Main.Loop = true;
		ps->Main.StartLifeTime = 5;
		ps->Main.StartSpeed = 2;
		ps->Main.ScalingMode = PScalingMode::Local;
		ps->Main.Space = SimulationSpace::World;
		ps->Main.GravityModifier = 0.0f;

		ps->Emission.RateOverTime = 10;

		ps->Shape.Type = PShapeType::Cone;
		ps->Shape.Scale = Vector3(2, 2, 2);
		ps->Shape.Degree = 30;

		ParticleRender* pr = obj->GetComponent<ParticleRender>();
		Material* mat = pr->SetMaterial("internal/material/ParticleAdd.material");
		mat->SetTextureFile("internal/texture/Default-Particle.png");

		Transform *node = obj->GetComponent<Transform>();
		node->Pitch(-90);
		node->SetPosition(Vector3(0, -2, 0));
	}
	void TestShape_Box()
	{
		GameObject* obj = GameObject::Instantiation();
		this->AddGameObject(obj);
		ParticleSystem* ps = obj->AddComponent<ParticleSystem>();
		ps->Main.Loop = true;
		ps->Main.StartLifeTime = 5;
		ps->Main.StartSpeed = 0;
		ps->Main.ScalingMode = PScalingMode::Local;
		ps->Main.Space = SimulationSpace::World;
		ps->Main.GravityModifier = 0.0f;

		ps->Emission.RateOverTime = 10;

		ps->Shape.Type = PShapeType::Box;
		ps->Shape.Scale = Vector3(2, 2, 2);

		ParticleRender* pr = obj->GetComponent<ParticleRender>();
		Material* mat = pr->SetMaterial("internal/material/ParticleAdd.material");
		mat->SetTextureFile("internal/texture/Default-Particle.png");

		Transform *node = obj->GetComponent<Transform>();
		node->Pitch(-90);
	}
	void TestShape_BoxEdge()
	{
		GameObject* obj = GameObject::Instantiation();
		this->AddGameObject(obj);
		ParticleSystem* ps = obj->AddComponent<ParticleSystem>();
		ps->Main.Loop = true;
		ps->Main.StartLifeTime = 5;
		ps->Main.StartSpeed = 0;
		ps->Main.ScalingMode = PScalingMode::Local;
		ps->Main.Space = SimulationSpace::World;
		ps->Main.GravityModifier = 0.0f;

		ps->Emission.RateOverTime = 10;

		ps->Shape.Type = PShapeType::BoxEdge;
		ps->Shape.Scale = Vector3(2, 2, 2);

		ParticleRender* pr = obj->GetComponent<ParticleRender>();
		Material* mat = pr->SetMaterial("internal/material/ParticleAdd.material");
		mat->SetTextureFile("internal/texture/Default-Particle.png");

		Transform *node = obj->GetComponent<Transform>();
		node->Pitch(-90);
		node->SetPosition(Vector3(0, 0, 0));
	}
	void TestShape_Circle()
	{
		GameObject* obj = GameObject::Instantiation();
		this->AddGameObject(obj);
		ParticleSystem* ps = obj->AddComponent<ParticleSystem>();
		ps->Main.Loop = true;
		ps->Main.StartLifeTime = 5;
		ps->Main.StartSpeed = 0;
		ps->Main.ScalingMode = PScalingMode::Local;
		ps->Main.Space = SimulationSpace::World;
		ps->Main.GravityModifier = 0.0f;

		ps->Emission.RateOverTime = 10;

		ps->Shape.Type = PShapeType::Circle;
		ps->Shape.Scale = Vector3(2, 2, 2);

		ParticleRender* pr = obj->GetComponent<ParticleRender>();
		Material* mat = pr->SetMaterial("internal/material/ParticleAdd.material");
		mat->SetTextureFile("internal/texture/Default-Particle.png");
	}
	void TestShape_CircleEdge()
	{
		GameObject* obj = GameObject::Instantiation();
		this->AddGameObject(obj);
		ParticleSystem* ps = obj->AddComponent<ParticleSystem>();
		ps->Main.Loop = true;
		ps->Main.StartLifeTime = 5;
		ps->Main.StartSpeed = 0;
		ps->Main.ScalingMode = PScalingMode::Local;
		ps->Main.Space = SimulationSpace::World;
		ps->Main.GravityModifier = 0.0f;

		ps->Emission.RateOverTime = 10;

		ps->Shape.Type = PShapeType::CircleEdge;
		ps->Shape.Scale = Vector3(2, 2, 2);

		ParticleRender* pr = obj->GetComponent<ParticleRender>();
		Material* mat = pr->SetMaterial("internal/material/ParticleAdd.material");
		mat->SetTextureFile("internal/texture/Default-Particle.png");
	}
	void Test1()
	{
		GameObject* obj = GameObject::Instantiation();
		this->AddGameObject(obj);
		ParticleSystem* ps = obj->AddComponent<ParticleSystem>();
		ps->Main.Loop = true;
		ps->Main.StartLifeTime = 5;
		ps->Main.StartSpeed = 1;
		ps->Main.ScalingMode = PScalingMode::Local;
		ps->Main.Space = SimulationSpace::World;
		ps->Main.GravityModifier = 0.2f;

		ps->Emission.RateOverTime = 10;

		ps->Shape.Type = PShapeType::Cone;
		ps->Shape.Scale = Vector3(5, 5, 5);
		ps->Shape.Degree = 30;

		ps->VelocityOverLifetime.Enabled = true;
		ps->VelocityOverLifetime.Velocity = Vector3(0, 1, 0);
		ps->VelocityOverLifetime.SpeedModifier = 2;

		ps->LimitVelocityOverLifetime.Enabled = false;
		ps->LimitVelocityOverLifetime.Speed = 3;
		ps->LimitVelocityOverLifetime.Dampen = 1;

		ps->ForceOverLifetime.Enabled = true;
		ps->ForceOverLifetime.Force = Vector3(0, -1, 0);

		ps->ColorOverLifetime.Enabled = true;
		ps->ColorOverLifetime.TargetColor = Color::Red;

		ps->SizeOverLifetime.Enabled = true;
		ps->SizeOverLifetime.TargetSize = Vector3::one * 2.0f;

		ParticleRender* pr = obj->GetComponent<ParticleRender>();
		Material* mat = pr->SetMaterial("internal/material/ParticleAdd.material");
		mat->SetTextureFile("internal/texture/Default-Particle.png");

		Transform *node = obj->GetComponent<Transform>();
		node->Pitch(-90);
		node->SetPosition(Vector3(0, -2, 0));
	}
};
DC_END_NAMESPACE
