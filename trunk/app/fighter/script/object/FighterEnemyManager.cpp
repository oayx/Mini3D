#include "FighterEnemyManager.h"
#include "FighterEnemyAirplane.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
void FighterEnemyManager::Awake()
{
	base::Awake();
}
void FighterEnemyManager::Update()
{
	base::Update();
	_lastCreateTime += Time::GetDeltaTime();
	if (_lastCreateTime >= _createRate)
	{
		_lastCreateTime = 0;
		CreateOne();
	}
}
void FighterEnemyManager::CreateOne()
{
	float x = Math::RandomRange(-Screen::GetWidth() * 0.4f * 0.01f, Screen::GetWidth() * 0.4f * 0.01f);
	float y = Screen::GetHeight() * 0.5f * 0.01f;

	GameObject* airplane_enemy = GameObject::Instantiation("AirplaneEnemy");
	airplane_enemy->GetTransform()->SetParent(GetTransform(), false);
	airplane_enemy->GetTransform()->SetLocalPosition(Vector3(x, y, 0));

	FighterEnemyAirplane* airplane = airplane_enemy->AddComponent<FighterEnemyAirplane>();
	airplane->SetImage("airplane/plane_enemy1.png");
	airplane->SetFiringRate(1.5f);
	airplane->SetHp(3);
}
DC_END_NAMESPACE
