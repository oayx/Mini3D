
#include <iostream>
#include "engine_header.h"
using namespace dc;

int main()
{
	Application::Initialize();

	sCreateWinInfo info;
	info.name = "pc";
	info.width = 800;
	info.height = 800;
	info.fullscreen = false;
	info.resizable = true;
	info.fps = 60;
	info.antiAlias = 4;
	info.sync = 0;
	RenderWindow* win = Application::CreateRenderWindow(info);
	if (win == nullptr)
		return false;

	//相机
	{
		GameObject* camera_obj = GameObject::Instance("MainCamera");
		camera_obj->GetTransform()->SetLocalPosition(Vector3(0, 0, -10));
		Camera* camera = camera_obj->AddComponent<Camera>();
		camera->SetAspect(win->GetWinSize().width / win->GetWinSize().height);
		camera->SetBackgroundColor(Color(0.5f, 0.5f, 0.5f, 0));
		uint mask = Math::RemoveFlag(LayerMask::Everything, LayerMask::GetMask(LayerMask::UI));
		camera->SetCullMask(mask);
	}

	Application::Start();
}