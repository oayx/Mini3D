#include "FighterPlayerAirplane.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(FighterPlayerAirplane, FighterAirplane);
void FighterPlayerAirplane::Awake()
{
	base::Awake();
	SetCamp(FighterCampType::Type_1);
}
void FighterPlayerAirplane::Update()
{
	base::Update();

	static bool is_draging = false;
	if (Input::GetMouseButtonDown(MouseBtnID::Left))
	{
		is_draging = true;
	}
	if (Input::GetMouseButtonUp(MouseBtnID::Left))
	{
		is_draging = false;
	}
	if (Input::GetMouseButton(MouseBtnID::Left))
	{
		if (is_draging)
		{
			Vector3 w_vec = SceneManager::GetMainCamera()->ScreenToWorldPoint(Vector3(Input::mousePosition.x, Input::mousePosition.y, 10));
			GetTransform()->SetLocalPosition(Vector3(w_vec.x, w_vec.y, GetTransform()->GetLocalPosition().z));
		}
	}
}
void FighterPlayerAirplane::OnCollisionEnter2D(Collision2D* collisionInfo)
{
	base::OnCollisionEnter2D(collisionInfo);

	GameObject* gameObject = collisionInfo->otherCollider->GetGameObject();
	FighterAirplane* actor = gameObject->GetComponent<FighterAirplane>();
	if (actor != nullptr && actor->GetCamp() != _camp)
	{
		_hp -= 10;
		if (_hp <= 0)
		{
			GameObject::Destroy(this->GetGameObject());
		}
		EventDispatcher::TriggerEvent(Evt_MainRole_Hp, String::ToString(_hp), String::ToString(_maxHp));
	}
}
void FighterPlayerAirplane::SetHp(int hp)
{
	base::SetHp(hp);
	EventDispatcher::TriggerEvent(Evt_MainRole_Hp, String::ToString(_hp), String::ToString(_maxHp));
}
DC_END_NAMESPACE
