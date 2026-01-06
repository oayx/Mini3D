#include "EditorAppliction.h"
#include "EditorCursor.h"
#include "EditorMain.h"
#include "EditorMainMenu.h"
#include "core/time/Timer.h"
#include "runtime/input/Input.h"
#include "runtime/physics/Physics.h"
#include "runtime/physics2d/Physics2d.h"
#include "runtime/project/ProjectManager.h"
#include "runtime/resources/AssetsManager.h"
#include "runtime/resources/Resources.h"
#include "runtime/scene/Camera.h"
#include "runtime/ui/GUI.h"
#include "runtime/ui/UICanvas.h"
#include "runtime/Application.h"
#include "runtime/Screen.h"
#include "editor/help/EIMGUISetter.h"
#include "editor/main/EMain_GameView.h"
#include "editor/main/EMain_SceneView.h"
#include "editor/main/EMain_Tools.h"
#include "editor/window/EConsole.h"
 
DC_BEGIN_NAMESPACE

/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(EditorAppliction, Object);
void EditorAppliction::Initialize()
{
	EditorConfig::Load();
	_layoutType = (LayoutType)EditorConfig::LayoutIndex;

	Function<void(LogMsgType, const char*)> f;
	f.Bind(&EditorAppliction::OnRecvLogMessage);
	Debuger::RegisterLogMessageReceived(f);

	InitComponent();
	EditorMain::Initialize();
}
void EditorAppliction::Destroy()
{
	Function<void(LogMsgType, const char*)> f;
	f.Bind(&EditorAppliction::OnRecvLogMessage);
	Debuger::UnregisterLogMessageReceived(f);

	EditorMain::Destroy();
	EditorConfig::Save();
}
void EditorAppliction::Update()
{
	DC_PROFILE_FUNCTION;
	if (!Application::IsEditor())return;
	
	if (_layoutType != EditorConfig::LayoutIndex)
	{//切换编辑器布局
		EditorMain::Destroy();
		GUIContext::ReleaseImGUI();

		_layoutType = (LayoutType)EditorConfig::LayoutIndex;
		EIMGUISetter::SetLayout(_layoutType);

		GUIContext::InitImGUI();
		EditorMain::Initialize();
		return;
	}

	{//界面更新
		EMain_SceneView::Update();
		EMain_GameView::Update();
	}

	if (_isWatchFolderChange)
	{//有资源变动
		_isWatchFolderChange = false;
		AssetsManager::Refresh();
	}

	{//快捷键
		if (Input::GetKey(KeyCode::LeftControl) && Input::GetKeyDown(KeyCode::S))
		{
			SceneManager::SaveScene();
		}
		if (Input::GetKey(KeyCode::LeftControl) && Input::GetKey(KeyCode::LeftShift) && Input::GetKeyDown(KeyCode::S))
		{
			std::string selectPath = Platform::OpenFileDialog("Select Scene", ".", "", "scene", false);
			if (!selectPath.empty())
			{
				String path = ProjectManager::ToProjectAssetsPath(selectPath);
				SceneManager::SaveAsScene(path);
			}
		}
	}
}
void EditorAppliction::Render()
{
	DC_PROFILE_FUNCTION;
	if (Application::IsEditor())
	{
		EditorMain::Render();
	}
	else
	{
		EditorMainMenu::Render();
	}

	//清理
	EditorCursor::EndRender();
}
void EditorAppliction::Resize(const WindowResizeDesc& desc)
{
	EditorMain::Resize(desc);
}
bool EditorAppliction::CreateProject(const String& path)
{
	if (ProjectManager::CreateProject(path))
	{
		return true;
	}

	return false;
}
bool EditorAppliction::OpenProject(const String& path)
{
	Debuger::Log("Open project path:%s", path.c_str());
	FileInfo fileInfo(path, SearchOption::TopDirectoryOnly);
	if (!fileInfo.HasChildren("assets"))
	{
		Debuger::Log("Is not valid project path:%s", path.c_str());
		return false;
	}

	if (ProjectManager::OpenProject(path))
	{
		Function<void()> callback;
		callback.Bind<&EditorAppliction::OnWatchFolderChange>();
		Platform::WatchFolder(Path::Combine(path, "assets").c_str(), callback);

		return true;
	}
	
	return false;
}
void EditorAppliction::SetInspectorType(InspectorType type)
{
	OnUnselectObject(_inspectorId);

	_inspectorType = type;
	_inspectorId = 0;
	_inspectorName = "";
}
void EditorAppliction::SetInspectorType(InspectorType type, uint64 id)
{
	OnUnselectObject(_inspectorId);

	_inspectorType = type;
	_inspectorId = id;
	_inspectorName = "";

	OnSelectObject(_inspectorId);
}
void EditorAppliction::SetInspectorType(InspectorType type, const String& file)
{
	OnUnselectObject(_inspectorId);
	_inspectorType = type;
	_inspectorName = file;
	_inspectorId = 0;
}
void EditorAppliction::OnRecvLogMessage(LogMsgType type, const char* msg)
{
	EConsole::AddLogMessage(type, msg);
}
void EditorAppliction::OnWatchFolderChange()
{
	_isWatchFolderChange = true;
}
void EditorAppliction::InitComponent()
{
	const auto& types = Type::GetTypes();
	const Type* basetype = typeof(Component);
	for (const auto& obj : types)
	{
		const Type* type = obj.second;
		if (!type->HasFlag(TypeFlag::ShowInEditor))continue;
		if (type != basetype && type->Is(basetype))
		{
			VecString names;
			const std::string& componentGroup = type->GetEditorComponent();
			if (!componentGroup.empty())
			{
				names = String(componentGroup).Split("/");
				InitComponent(_components, names, type->GetName());
			}
		}
	}
}
void EditorAppliction::InitComponent(EComponents& parentComponent, VecString& names, const String& className)
{
	if (names.IsEmpty())return;

	String name = names.First();
	names.RemoveAt(0);
	auto it = parentComponent.Childrends.Find(name);
	if (it != parentComponent.Childrends.end())
	{
		InitComponent(it->second, names, className);
	}
	else
	{
		if (names.Size() > 0)
		{
			EComponents component;
			InitComponent(component, names, className);
			parentComponent.Childrends.Add(name, component);
		}
		else
		{
			parentComponent.Components.Add({ name , className});
		}
	}
}
void EditorAppliction::OnSelectObject(uint64 id)
{
	GameObject* selectObject = SceneManager::GetGameObject(id);
	if (selectObject)
	{
		UICanvas* canvas = selectObject->GetComponent<UICanvas>();
		if (canvas)
		{
			canvas->SetDrawGizmos(!EditorConfig::Is3D);
		}
		else
		{
			if (EditorConfig::Is3D)
			{
				Camera* camera = selectObject->GetComponent<Camera>();
				if (camera && selectObject->GetLayer() != LayerMask::NameToLayer(LayerMask::UI))
				{
					camera->SetDrawGizmos(true);
					selectObject->AddComponent<VirtualRenderer>();
					selectObject->InsertFlag(GameObjectFlag::AlwaysFindInScene);
				}
			}
		}
	}
}
void EditorAppliction::OnUnselectObject(uint64 id)
{
	GameObject* selectObject = SceneManager::GetGameObject(id);
	if (selectObject)
	{
		UICanvas* canvas = selectObject->GetComponent<UICanvas>();
		if (canvas)
		{
			canvas->SetDrawGizmos(false);
		}
		else
		{
			Camera* camera = selectObject->GetComponent<Camera>();
			if (camera)
			{
				camera->SetDrawGizmos(false);
				selectObject->RemoveComponent<VirtualRenderer>();
				selectObject->RemoveFlag(GameObjectFlag::AlwaysFindInScene);
			}
		}
	}
}
DC_END_NAMESPACE