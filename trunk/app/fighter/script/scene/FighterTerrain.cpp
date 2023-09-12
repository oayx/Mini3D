#include "FighterTerrain.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/	
FighterTerrain::FighterTerrain()
	: _moveSpeed(-1)
{

}
FighterTerrain::~FighterTerrain()
{

}
void FighterTerrain::Awake()
{
	return;
	Texture* tex = Texture::Create("airplane/img_bg_level_1.jpg");
	_terrainTileSize.Set(tex->GetWidth()*0.01f, tex->GetHeight()*0.01f);
	{
		GameObject* terrain_obj_1 = GameObject::Instantiation("Terrain1");
		terrain_obj_1->GetTransform()->SetParent(GetTransform(), false);
		terrain_obj_1->GetTransform()->SetLocalPosition(Vector3(0, -_terrainTileSize.height, 0));
		_terrains.Add(terrain_obj_1);

		Sprite* terrain_1 = terrain_obj_1->AddComponent<Sprite>();
		terrain_1->SetTexture(tex);
		terrain_1->SetSortLayer(FighterId::Layer_Terrain);
	}
	{
		GameObject* terrain_obj_2 = GameObject::Instantiation("Terrain2");
		terrain_obj_2->GetTransform()->SetParent(GetTransform(), false);
		_terrains.Add(terrain_obj_2);

		Sprite* terrain_2 = terrain_obj_2->AddComponent<Sprite>();
		terrain_2->SetTexture(tex);
		terrain_2->SetSortLayer(FighterId::Layer_Terrain);
	}
	{
		GameObject* terrain_obj_3 = GameObject::Instantiation("Terrain3");
		terrain_obj_3->GetTransform()->SetParent(GetTransform(), false);
		terrain_obj_3->GetTransform()->SetLocalPosition(Vector3(0, _terrainTileSize.height, 0));
		_terrains.Add(terrain_obj_3);

		Sprite* terrain_3 = terrain_obj_3->AddComponent<Sprite>();
		terrain_3->SetTexture(tex);
		terrain_3->SetSortLayer(FighterId::Layer_Terrain);
	}
}
void FighterTerrain::Update()
{
	return;
	for (auto terrain : _terrains)
	{
		float y = terrain->GetTransform()->GetLocalPosition().y;
		y += Time::GetDeltaTime() * _moveSpeed;
		terrain->GetTransform()->SetLocalPosition(Vector3(0, y, 0));
	}
	auto terrain = _terrains.First();
	if (terrain->GetTransform()->GetLocalPosition().y + _terrainTileSize.height*0.5f + Time::GetDeltaTime() * _moveSpeed < -Screen::GetHeight()*0.5f*0.01f)
	{
		auto last_terrain = _terrains.Last();
		float y = last_terrain->GetTransform()->GetLocalPosition().y + _terrainTileSize.height;
		terrain->GetTransform()->SetLocalPosition(Vector3(0, y, 0));
		_terrains.Remove(_terrains.begin());
		_terrains.Add(terrain);
	}
}
DC_END_NAMESPACE
