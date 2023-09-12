#include "FighterApp.h"
#include "gui/UIManager.h"
#include "panel/FighterMainPanel.h"
#include "panel/FighterResultPanel.h"
#include "scene/FighterTerrain.h"
#include "object/FighterPlayerAirplane.h"
#include "object/FighterEnemyAirplane.h"
#include "object/FighterEnemyManager.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
FighterApp::~FighterApp()
{
	UnRegisterEvent();
}
void FighterApp::Start()
{
#if defined(DC_PLATFORM_WIN32) || defined(DC_PLATFORM_LINUX)
	Application::SetAssetsPath("../../assets");
	ProjectManager::OpenProject("../..");
#else
	ProjectManager::OpenProject(Path::GetDirectoryName(Application::GetAssetsPath()));
#endif
	SceneManager::CreateScene("TestMain");

	//添加默认听众
	GameObject::Instantiation("AudioListener")->AddComponent<AudioListener>();

	RegisterEvent();

	//相机
	GameObject* camera_obj = GameObject::Instantiation("MainCamera");
	camera_obj->GetTransform()->SetLocalPosition(Vector3(0, 0, -10));
	Camera* camera = camera_obj->AddComponent<Camera>();
	camera->SetOrthographic(true);
	camera->SetOrthographicSize(512 * 0.01f, ((Screen::GetHeight()*512.0f) / Screen::GetWidth())*0.01f);
	camera->SetAspect(Screen::GetWidth() / Screen::GetHeight());
	camera->SetClearColor(Color(0.5f, 0.5f, 0.5f, 0));
	uint mask = LayerMask::Everything ^ LayerMask::GetMask(LayerMask::UI);
	camera->SetCullMask(mask);

	//UI
	GameObject* ui_root = GameObject::Instantiation("UIRoot");
	ui_root->GetTransform()->SetLocalScale(Vector3::one * 0.01f);
	Camera* ui_camera = ui_root->AddComponent<Camera>();
	UICanvas * ui_canvas = ui_root->AddComponent<UICanvas>();
	ui_canvas->SetRenderMode(UIRenderMode::ScreenSpace_Camera);
	ui_canvas->SetCamera(ui_camera);
	ui_camera->SetOrthographic(true);
	ui_camera->SetDepth(10);
	ui_camera->SetZNear(-100);
	ui_camera->SetZFar(1000);
	ui_camera->SetClearFlag(ClearFlag::DepthOnly);
	ui_camera->SetCullMask(LayerMask::GetMask(LayerMask::UI));

	UICanvasScaler* ui_scaler = ui_root->AddComponent<UICanvasScaler>();
	ui_scaler->SetReferenceResolution(iSize(750, 1334));
	ui_scaler->SetMatchWidth(true);

	//物理引擎
	Physics2d::EnableDebug(true);

	//加载图集
	UIAtlasManager::LoadAtlas("atlas/common.png");
	UIAtlasManager::LoadAtlas("atlas/head.png");
	UIAtlasManager::LoadAtlas("atlas/number.png");
	UIAtlasManager::LoadAtlas("atlas/compnent.png");

	//背景音乐
	AudioClip* audio_clip = AudioClip::Create("sound/music_bg.wav");
	auto audio_source = GameObject::Instantiation("music_bg")->AddComponent<AudioSource>();
	audio_source->SetClip(audio_clip);
	audio_source->SetLoop(true);
	audio_source->Play();

	//显示地图
	GameObject* terrain_root = GameObject::Instantiation("TerrainRoot");
	terrain_root->AddComponent<FighterTerrain>();

	//我方飞机
	GameObject* airplane_root = GameObject::Instantiation("AirplaneRoot");
	{
		GameObject* airplane_player = GameObject::Instantiation("AirplanePlayer");
		airplane_player->GetTransform()->SetParent(airplane_root->GetTransform(), false);
		FighterPlayerAirplane* airplane = airplane_player->AddComponent<FighterPlayerAirplane>();
		airplane->SetImage("airplane/player_plane15.png");
		airplane->SetFiringRate(0.2f);
		airplane->SetHp(10);
	}
	//敌方
	{
		GameObject* enemy_root = GameObject::Instantiation("AirplaneEnemyRoot");
		enemy_root->AddComponent<FighterEnemyManager>();
	}

	//显示主界面
	UIManager::Instance()->Show<FighterMainPanel>();
}
void FighterApp::RegisterEvent()
{
	EventDispatcher::AddEventListener(Evt_Restart, this, MakeEventHandler(FighterApp::OnRestart));
	EventDispatcher::AddEventListener(Evt_MainRole_Hp, this, MakeEventHandler2(FighterApp::OnRoleHp));
	Timer::AddLoop(0, CALLBACK_0(FighterApp::Update, this));
}
void FighterApp::UnRegisterEvent()
{
	EventDispatcher::RemoveEventListener(Evt_Restart, this, MakeEventHandler(FighterApp::OnRestart));
	EventDispatcher::RemoveEventListener(Evt_MainRole_Hp, this, MakeEventHandler2(FighterApp::OnRoleHp));
}
void FighterApp::Update()
{
	if (Input::GetKey(KeyCode::Escape))
	{//退出
		Application::Quit();
	}
}
void FighterApp::OnRestart()
{
	GameObject* airplane_root = GameObject::Find("AirplaneRoot");

	GameObject* airplane_player = GameObject::Instantiation("AirplanePlayer");
	airplane_player->GetTransform()->SetParent(airplane_root->GetTransform(), false);
	FighterPlayerAirplane* airplane = airplane_player->AddComponent<FighterPlayerAirplane>();
	airplane->SetImage("airplane/player_plane15.png");
	airplane->SetFiringRate(0.2f);
	airplane->SetHp(10);
}
void FighterApp::OnRoleHp(const String& arg1, const String& arg2)
{
	int cur_hp = arg1.ToInt();
	if (cur_hp <= 0)
	{
		UIManager::Instance()->Show<FighterResultPanel>();
	}
}
DC_END_NAMESPACE
