#include "FighterBullet.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(FighterBullet, FighterActor);
uint FighterBullet::_bulletId = 0;
void FighterBullet::Awake()
{
	_bullet = GetGameObject()->AddComponent<Sprite>();
	_bullet->SetSortLayer(FighterId::Layer_Bullet);

	_collider = GetGameObject()->AddComponent<BoxCollider2d>();

	RigidBody2d* rigid_body = GetGameObject()->AddComponent<RigidBody2d>();
	rigid_body->UseGravity(false);
	rigid_body->SetCollisionDetection(CollisionDetection2d::Continuous);
}
void FighterBullet::Update()
{
	if (IsOutScreen())
	{
		GameObject::Destroy(this->GetGameObject());
	}
	else
	{
		Vector3 y = GetTransform()->GetLocalPosition() + Vector3::up * Time::GetDeltaTime() * _speed;
		GetTransform()->SetLocalPosition(y);
	}
}
void FighterBullet::OnTransformChanged()
{
	if (_collider != nullptr)
	{
		_collider->ModifyPosition(Vector2(GetTransform()->GetPosition().x, GetTransform()->GetPosition().y));
	}
}
void FighterBullet::OnCollisionEnter2D(Collision2D* collisionInfo)
{
	GameObject* gameObject = collisionInfo->otherCollider->GetGameObject();
	FighterActor* actor = gameObject->GetComponent<FighterActor>();
	if (actor != nullptr && actor->GetCamp() != _camp)
	{
		GameObject::Destroy(this->GetGameObject());
	}
}
void FighterBullet::SetCamp(FighterCampType camp) 
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
		_collider->SetGroupIndex(-1);
	}
}
void FighterBullet::SetImage(const String& file)
{
	Texture* tex = Texture::Create(file);
	_bullet->SetTexture(tex);
}
bool FighterBullet::IsOutScreen()
{
	Vector3 pos = GetTransform()->GetPosition();
	float y = pos.y * 100.0f;
	if (y < -Screen::GetHeight() * 0.5f || y > Screen::GetHeight() * 0.5f)
	{
		return true;
	}
	return false;
}
DC_END_NAMESPACE
