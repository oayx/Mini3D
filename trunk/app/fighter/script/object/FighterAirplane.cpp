#include "FighterAirplane.h"
#include "FighterPlayerAirplane.h"
#include "FighterBullet.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(FighterAirplane, FighterActor);
void FighterAirplane::Awake()
{
	_bulletRoot = GameObject::Find("BulletRoot");
	if (_bulletRoot == nullptr)
	{
		_bulletRoot = GameObject::Instantiation("BulletRoot");
	}

	_airplane = GetGameObject()->AddComponent<Sprite>();
	_airplane->SetSortLayer(FighterId::Layer_Airplane);
	_collider = GetGameObject()->AddComponent<BoxCollider2d>();
}
void FighterAirplane::Update()
{
	base::Update();
	_lastFireTime += Time::GetDeltaTime();
	if (_lastFireTime >= _firingRate)
	{
		_lastFireTime = 0;
		HandleFire();
	}
}
void FighterAirplane::OnTransformChanged()
{
	base::OnTransformChanged();

	if (_collider != nullptr)
	{
		_collider->ModifyPosition(Vector2(GetTransform()->GetPosition().x, GetTransform()->GetPosition().y));
	}
}
void FighterAirplane::OnCollisionEnter2D(Collision2D* collisionInfo)
{
	base::OnCollisionEnter2D(collisionInfo);

	GameObject* gameObject = collisionInfo->otherCollider->GetGameObject();
	FighterBullet* actor = gameObject->GetComponent<FighterBullet>();
	if (actor != nullptr && actor->GetCamp() != _camp)
	{
		--_hp;
		if (this->Is<FighterPlayerAirplane>())
		{
			EventDispatcher::TriggerEvent(Evt_MainRole_Hp, String::ToString(_hp), String::ToString(_maxHp));
		}
		if (_hp <= 0)
		{
			GameObject::Destroy(this->GetGameObject());
		}
	}
}
void FighterAirplane::SetCamp(FighterCampType camp)
{
	base::SetCamp(camp);
	if (_collider != nullptr)
	{
		int mask = 0;
		int category = 0;
		switch (camp)
		{
		case dc::FighterCampType::Type_1:
			mask = 0x0001;
			category = 0x0002;
			break;
		case dc::FighterCampType::Type_2:
			mask = 0x0002;
			category = 0x0001;
			break;
		}
		_collider->SetMaskBits(mask);
		_collider->SetCategoryBits(category);
	}
}
void FighterAirplane::SetImage(const String& file)
{
	Texture* tex = Texture::Create(file);
	_airplane->SetTexture(tex);
}
void FighterAirplane::HandleFire()
{
	uint id = FighterBullet::SharedId();
	GameObject* bullet_root = GameObject::Instantiation("BulletObject_" + String::ToString(id));
	bullet_root->GetTransform()->SetParent(_bulletRoot->GetTransform(), false);

	Vector3 bullet_pos = GetTransform()->GetPosition();
	float rotate = 0;
	switch (_camp)
	{
	case dc::FighterCampType::Type_1:
		bullet_pos = GetTransform()->GetPosition() + Vector3::up * _airplane->GetSize().height * 0.5f;
		break;
	case dc::FighterCampType::Type_2:
		bullet_pos = GetTransform()->GetPosition() - Vector3::up * _airplane->GetSize().height * 0.5f;
		rotate = 180;
		break;
	}
	bullet_root->GetTransform()->SetPosition(bullet_pos);
	bullet_root->GetTransform()->Roll(rotate);

	FighterBullet* bullet = bullet_root->AddComponent<FighterBullet>();
	bullet->SetCamp(_camp);
	bullet->SetImage(_camp == FighterCampType::Type_1 ? "airplane/bullet11.png": "airplane/bullet21.png");
	bullet->SetSpeed(GetBulletSpeed());
}
DC_END_NAMESPACE
