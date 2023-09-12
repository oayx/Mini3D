#include "FighterEnemyAirplane.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(FighterEnemyAirplane, FighterAirplane);
void FighterEnemyAirplane::Awake()
{
	base::Awake();
	SetCamp(FighterCampType::Type_2);
}
void FighterEnemyAirplane::Update()
{
	base::Update();
	if (IsOutScreen())
	{
		GameObject::Destroy(GetGameObject());
		return;
	}
	float y = _moveSpeed * Time::GetDeltaTime();
	GetTransform()->SetLocalPosition(GetTransform()->GetLocalPosition() - Vector3::up * y);
}
bool FighterEnemyAirplane::IsOutScreen()
{
	Vector3 pos = GetTransform()->GetPosition();
	float y = pos.y * 100.0f;
	if (y < -Screen::GetHeight() || y > Screen::GetHeight())
	{
		return true;
	}
	return false;
}
DC_END_NAMESPACE
