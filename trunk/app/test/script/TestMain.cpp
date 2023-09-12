#include "TestMain.h"
#include "TestPanel.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
bool TestMain::Start()
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

	//相机
	GameObject* camera_obj = GameObject::Instantiation("MainCamera");
	camera_obj->GetTransform()->SetLocalPosition(Vector3(0, 0, -10));
	Camera* camera = camera_obj->AddComponent<Camera>();
	camera->SetAspect(Screen::GetWidth() / Screen::GetHeight());
	camera->SetClearColor(Color(0.5f, 0.5f, 0.5f, 1));
	//camera->SetClearFlag(ClearFlag::Skybox);
	//camera->EnableHDR(true);
	//camera->SetOrthographic(true);
	//camera->SetOrthographicSize(8.00f, 8.00f);
	//TextureDesc desc;
	//desc.width = 2048; desc.height = 2048; desc.antiAlias = 1; desc.flags = TextureFlag::COLOR_AND_DEPTH;
	//camera->CreateRenderTexture(desc);
	
	uint mask = LayerMask::Everything ^ LayerMask::GetMask(LayerMask::UI);
	camera->SetCullMask(mask);

	_rootObject = GameObject::Instantiation("MainObject");
	_testPanel = _rootObject->AddComponent<TestPanel>();
	_testPanel->OnEnter(-1, -1);
	
	RegisterEvent();

	return true;
}

void TestMain::Release()
{
	if (_testPanel)
	{
		GameObject::Destroy(_rootObject);
		_testPanel = nullptr;
	}

	UnRegisterEvent();
}
void TestMain::RegisterEvent()
{
	Timer::AddLoop(0, CALLBACK_0(TestMain::Update, this));
}
void TestMain::UnRegisterEvent()
{
}
void TestMain::Update()
{
	{
		if (Input::GetKeyDown(KeyCode::R))
		{//渲染模式切换
			static bool bWire = false;
			Camera* camera = SceneManager::GetMainCamera()->GetComponent<Camera>();
			if (camera)
			{
				camera->SetFillMode(!bWire ? FillMode::Wire : FillMode::Solid);
			}
			bWire = !bWire;
		}
		
		if (Input::GetKey(KeyCode::LeftAlt) &&Input::GetKeyDown(KeyCode::Return))
		{//全屏
			static bool fullscreen = false;
			fullscreen = !fullscreen;
			WindowResizeDesc desc(nullptr, 800, 800, fullscreen, true);
			Application::Resize(desc);
		}

		if (Input::GetKey(KeyCode::Escape))
		{//退出
			Application::Quit();
		}

		float move_speed = 0.3f;
		if (Input::GetKey(KeyCode::W))
		{
			Camera* camera = SceneManager::GetMainCamera()->GetComponent<Camera>();
			camera->MoveForwardBack(move_speed);
		}
		if (Input::GetKey(KeyCode::S))
		{
			Camera* camera = SceneManager::GetMainCamera()->GetComponent<Camera>();
			camera->MoveForwardBack(-move_speed);
		}
		if (Input::GetKey(KeyCode::A))
		{
			Camera* camera = SceneManager::GetMainCamera()->GetComponent<Camera>();
			camera->MoveLeftRight(-move_speed);
		}
		if (Input::GetKey(KeyCode::D))
		{
			Camera* camera = SceneManager::GetMainCamera()->GetComponent<Camera>();
			camera->MoveLeftRight(move_speed);
		}
	}
	if(Input::IsMouseMove() && (_testPanel == nullptr || _testPanel->GetCurrUnit() == nullptr || _testPanel->GetCurrUnit()->CameraCanMove()))
	{
		float offset_x = Input::mousePositionDelta.x;
		float offset_y = -Input::mousePositionDelta.y;
		if (Math::Abs(offset_x) > 0 || Math::Abs(offset_y) > 0)
		{
			//Debuger::Log("%f - %f", offset_x, offset_y);
			Camera* camera = SceneManager::GetMainCamera()->GetComponent<Camera>();
			camera->Rotate(offset_x, offset_y);
		}
	}
	if (false && Input::GetMouseButtonDown(MouseBtnID::Left))
	{
		Debuger::Log("mouse position:%s", Input::mousePosition.ToString().c_str());
		Camera* camera = SceneManager::GetMainCamera();
		Vector3 v_vec = camera->ScreenToViewportPoint(Input::mousePosition);
		Debuger::Log("ScreenToViewportPoint:%s", v_vec.ToString().c_str());
		Vector3 w_vec = camera->ScreenToWorldPoint(Vector3(Input::mousePosition.x, Input::mousePosition.y,10));
		Debuger::Log("ScreenToWorldPoint:%s", w_vec.ToString().c_str());

		Ray ray = camera->ScreenPointToRay(Input::mousePosition);
		Debuger::Log("ScreenPointToRay:%s", ray.ToString().c_str());
		List<GameObject*> list = SceneManager::FindGameObjects(ray);
		for (auto obj : list)
		{
			//if (obj->GetName() == "sprite")
			{
				Debuger::Log("hit object:%s", obj->GetInstanceName().c_str());
				SetFocusNode(obj->GetTransform());
			}
		}
	}
	if (false && Input::GetTouchCount() > 0)
	{
		Touch touch;
		if (Input::GetTouch(0, touch))
		{
			Camera* camera = SceneManager::GetMainCamera();
			Ray ray = camera->ScreenPointToRay(Input::mousePosition);
			Debuger::Log("ScreenPointToRay:%s", ray.ToString().c_str());
			List<GameObject*> list = SceneManager::FindGameObjects(ray);
			for (auto obj : list)
			{
				SetFocusNode(obj->GetTransform());
			}

			switch (touch.phase)
			{
			case TouchPhase::Began:
				break;
			case TouchPhase::Stationary:
				if (_pFocusNode != nullptr)
				{
					float offset_x = touch.deltaPosition.x*0.4f;
					float offset_y = touch.deltaPosition.y*0.4f;
					Quaternion q(Vector3(offset_y, offset_x, 0));
					_pFocusNode->Rotate(q);
				}
				break;
			case TouchPhase::Ended:
				break;
			default:
				break;
			}
		}
	}
}
DC_END_NAMESPACE
