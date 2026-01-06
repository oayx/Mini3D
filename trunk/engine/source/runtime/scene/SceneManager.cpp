#include "SceneManager.h"
#include "Camera.h"
#include "Light.h"
#include "tree/Scenetree.h"
#include "runtime/component/Transform.h"
#include "runtime/component/GameObject.h"
#include "runtime/ui/GUI.h"
#include "runtime/ui/UICanvas.h"
#include "runtime/graphics/ShadowMap.h"
#include "runtime/graphics/ReflectionProbe.h"
#include "runtime/graphics/null/GraphicsDevice.h"
#include "runtime/graphics/null/SwapChain.h"
#include "runtime/renderable/Gizmos.h"
#include "runtime/Application.h"
#include "runtime/Screen.h"
#include "platform/RenderWindow.h"
#include "editor/EditorAppliction.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_REFECTION_TYPE(SceneManager);
void SceneManager::Initialize()
{
	CreateSceneTree(SceneTreeType::Octree);
}
void SceneManager::Destroy()
{
	UnloadScenes();
	DestroySceneTree();
	for (auto obj : _objects)
	{
		if (obj.second->ActiveSelf())
		{
			obj.second->PostComponentAction(ComponentAction::ApplicationQuit, true, false);
		}
	}

	_cameras.Clear();
	_lights.Clear();
	for (auto obj : _objects)
	{
		if (obj.second == _engineObject)continue;
		SAFE_DELETE(obj.second);
	}
	_objects.Clear();

	//在最后销毁
	SAFE_DELETE(_engineObject);
}
void SceneManager::Update()
{
	DC_PROFILE_FUNCTION;
	UpdateScene();
	UpdateUI();
}
void SceneManager::UpdateScene()
{
	DC_PROFILE_FUNCTION;
	GetSceneTree()->RemoveAllObjects();
	//移除
	if (!_removedObjects.IsEmpty())
	{
		Objects removed = _removedObjects;
		_removedObjects.Clear();

		for (auto obj : removed)
		{
			for (auto it = _objects.begin(); it != _objects.end(); ++it)
			{
				if (obj->GetInstanceId() == it->first)
				{
					_objects.Remove(it);
					break;
				}
			}
			SAFE_DELETE(obj);
		}
	}

	//添加
	if (!_addedObjects.IsEmpty())
	{
		Objects added = _addedObjects;
		_addedObjects.Clear();
		for (const auto& obj : added)
		{
			_objects.Add(obj->GetInstanceId(), obj);
		}
	}

	//Update
	for (const auto& obj : _objects)
	{
		GameObject* object = obj.second;
		object->Update();
		if (object->ActiveInHierarchy())
		{
			if (object->HasFlag(GameObjectFlag::DonotPutInScene))
				continue;

			//UI节点只把Canvas加入
			if (object->GetRectTransform() == nullptr || object->GetComponent<UICanvas>() != nullptr)
			{
				GetSceneTree()->AddObject(object);
			}
		}
	}
	//LateUpdate
	for (auto& obj : _objects)
	{
		if (obj.second->ActiveInHierarchy())
		{
			obj.second->PostComponentAction(ComponentAction::LateUpdate, true, false);
		}
	}
}
void SceneManager::UpdateUI()
{
	DC_PROFILE_FUNCTION;
	for (auto& canvas : _uICanvas)
	{
		if(canvas->GetRenderMode() != UIRenderMode::None)
			canvas->Refresh();
	}
}
void SceneManager::Resize(const WindowResizeDesc& desc)
{
	DC_PROFILE_FUNCTION;
	for (auto& camera : _cameras)
	{
		camera->Resize(desc);
	}
}
void SceneManager::Render(RenderWindow* window)
{
	DC_PROFILE_FUNCTION;
	PreRender(window);
	RenderScene(window);
	RenderGUI(window);
	PostRender(window);
}
void SceneManager::PreRender(RenderWindow* window)
{
	DC_PROFILE_FUNCTION;
	_oldDrawTriangles = _nDrawTriangles;
	_oldRenderBatch = _nRenderBatch;
	_oldSetPassCall = _nSetPassCall;
	_oldDrawCall = _nDrawCall;
	_nDrawTriangles = 0;
	_nRenderBatch = 0;
	_nSetPassCall = 0;
	_nDrawCall = 0;
	Application::GetGraphics()->PreRender(window);
}
void SceneManager::RenderScene(RenderWindow* window)
{
	DC_PROFILE_FUNCTION;
	//1.先对相机按照深度排序
	List<Camera*> list_camera;
	for (const auto& camera : _cameras)
	{
		if (!camera->IsEnable() || !camera->GetGameObject()->ActiveInHierarchy())
			continue;

		bool isInsert = false;
		for (List<Camera*>::iterator it = list_camera.begin(); it != list_camera.end(); ++it)
		{
			if ((*it)->GetDepth() > camera->GetDepth())
			{
				list_camera.Insert(it, camera);
				isInsert = true;
				break;
			}
		}
		if (!isInsert)list_camera.Add(camera);
	}

	//2.再渲染
	{
		//先使用默认清除(TODO:这样有可能导致重复清除缓冲区，但是如果没有相机或相机只是渲染到纹理的情况下，可以保证GUI渲染正确)
		{//TODO:需要移除
			RenderFrameDesc desc;
			desc.viewPort = ViewPortDesc(window->GetWidth(), window->GetHeight());
			desc.clearFlag = ClearFlag::SolidColor;//清除所有
			desc.clearColor = Color::Clear;
			Application::GetGraphics()->GetSwapChain()->BeginFrame(desc);
			Application::GetGraphics()->GetSwapChain()->EndFrame();
		}
		//再执行相机渲染
		if (list_camera.Size() > 0)
		{
			for (Camera* camera : list_camera)
			{//TODO:如果渲染过程中，删除相机，可能导致
				if (camera->GetTargetDisplay() == window->GetTargetDisplay())
				{
					camera->PreRender();
					camera->Render();
					camera->PostRender();
				}
			}
		}
	}
}
void SceneManager::RenderGUI(RenderWindow* window)
{
	DC_PROFILE_FUNCTION;
	RenderFrameDesc desc;
	desc.viewPort = ViewPortDesc(window->GetWidth(), window->GetHeight());
	desc.clearFlag = ClearFlag::DonotClear;//不要清除缓冲区，只把交换链指向默认
	desc.clearColor = Color::Clear;
	Application::GetGraphics()->GetSwapChain()->BeginFrame(desc);
	{
		GUIContext::PreRender();
		EditorAppliction::Render();
		for (const auto& obj : _objects)
		{
			if (obj.second->ActiveInHierarchy())
			{
				obj.second->PostComponentAction(ComponentAction::GUI, true, false);
			}
		}

		GUIContext::Render();
		GUIContext::PostRender();
	}
	Application::GetGraphics()->GetSwapChain()->EndFrame();
}
void SceneManager::PostRender(RenderWindow* window)
{
	DC_PROFILE_FUNCTION;
	Application::GetGraphics()->PostRender(window);
	Application::GetGraphics()->Present(window);
}
void SceneManager::AddCamera(Camera *camera)
{
	for (const auto& obj : _cameras)
	{
		if (camera->GetInstanceId() == obj->GetInstanceId())return;
	}
	_cameras.Add(camera);
}
bool SceneManager::RemoveCamera(Camera *camera)
{
	if (camera == nullptr)
		return false;

	for (auto it = _cameras.begin(); it != _cameras.end(); ++it)
	{
		if ((*it)->GetInstanceId() == camera->GetInstanceId())
		{
			_cameras.Remove(it);
			break;
		}
	}
	if (_mainCamera != nullptr && camera->GetInstanceId() == _mainCamera->GetInstanceId())
		_mainCamera = nullptr;
	return false;
}
GameObject* SceneManager::GetEngineObject()
{
	if (_engineObject)
		return _engineObject;
	_engineObject = GameObject::Instantiation("EngineObject");
	_engineObject->InsertFlag(GameObjectFlag::AlwaysFindInScene);
	return _engineObject;
}
Camera* SceneManager::GetCamera(int index)
{
	if (index < 0 || index >= _cameras.Size())
		return nullptr;
	return _cameras[index];
}
Camera*	SceneManager::GetCamera(const String& name)
{
	for (const auto& obj : _cameras)
	{
		if (obj->GetInstanceName().Equals(name, true))return obj;
	}
	return nullptr;
}
Camera* SceneManager::GetMainCamera()
{
	if (_mainCamera == nullptr)
	{
		for (const auto& obj : _cameras)
		{
			if (obj->GetGameObject()->HasFlag(GameObjectFlag::EditorCamera))continue;
			if (obj->GetInstanceName() == "MainCamera" || obj->GetInstanceName() == "Main Camera")
			{
				_mainCamera = obj;
				break;
			}
		}
	}
	return _mainCamera;
}
ReflectionProbe* SceneManager::GetProbe()
{
	for (const auto& obj : _cameras)
	{
		Camera* camera = obj;
		if (camera->IsSame<ReflectionProbe>())
			return (ReflectionProbe*)(camera);
	}
	return nullptr;
}
void SceneManager::AddLight(Light* light)
{
	for (const auto& obj : _lights)
	{
		if (light->GetInstanceId() == obj->GetInstanceId())return;
	}
	_lights.Add(light);
}
bool SceneManager::RemoveLight(Light* light)
{
	if (light == nullptr)
		return false;

	for (auto it = _lights.begin(); it != _lights.end(); ++it)
	{
		if (*it == light)
		{
			_lights.Remove(it);
			return true;
		}
	}
	return false;
}
Light* SceneManager::GetLight(const String& name)
{
	for (const auto& obj : _lights)
	{
		if (obj->GetInstanceName().Equals(name, true))return obj;
	}
	return nullptr;
}
Light* SceneManager::GetLight(int index)
{
	if (index < 0 || index >= _lights.Size())
		return nullptr;
	return _lights[index];
}
GameObject*	SceneManager::GetGameObject(uint64 id)
{
	GameObject* obj;
	if (_objects.TryGet(id, &obj))
	{
		return obj;
	}
	return nullptr;
}
void SceneManager::AddUICanvas(UICanvas* canvas)
{
	for (const auto& obj : _uICanvas)
	{
		if (canvas->GetInstanceId() == obj->GetInstanceId())return;
	}
	_uICanvas.Add(canvas);
}
bool SceneManager::RemoveUICanvas(UICanvas* canvas)
{
	for (auto it = _uICanvas.begin(); it != _uICanvas.end(); ++it)
	{
		if (*it == canvas)
		{
			_uICanvas.Remove(it);
			return true;
		}
	}
	return false;
}
void SceneManager::AddGameObject(GameObject* obj)
{
	if (obj == nullptr)return;
	if (std::find(_addedObjects.begin(), _addedObjects.end(), obj) == _addedObjects.end())
	{
		_addedObjects.Add(obj);
	}
	else
	{
		Debuger::Warning("Already have the same object when added:%s", obj->GetInstanceName().c_str());
	}
}
void SceneManager::RemoveGameObject(GameObject* obj)
{
	if (obj == nullptr)return;
	_addedObjects.Remove(obj);
	if (std::find(_removedObjects.begin(), _removedObjects.end(), obj) == _removedObjects.end())
	{
		_removedObjects.Add(obj);
	}
	else
	{
		Debuger::Warning("Already have the same object when deleted:%s", obj->GetInstanceName().c_str());
	}
}
List<GameObject*> SceneManager::FindGameObjects(const Ray& ray)
{
	List<GameObject*> list;
	GetSceneTree()->FindObjects(list, ray);
	return list;
}
bool SceneManager::CreateScene(const String& name)
{
	if (Application::IsEditor() && Application::IsPlay())
	{
		Debuger::Message("Error", "Cannot create scene when running");
		return false;
	}

	if (name.IsEmpty())return false;

	if (_currScene)
	{
		UnloadScene(_currScene->GetName());
	}

	Scene* scene = Scene::Create(name);
	_scenes.Add(scene);
	_currScene = scene;

	if (Application::IsEditor())
	{
		_currScene->CreateDefault();
	}

	return true;
}
bool SceneManager::LoadScene(const String& file, LoadSceneMode mode)
{
	if (Application::IsEditor() && Application::IsPlay())
	{
		Debuger::Message("Error", "Cannot load scene when running");
		return false;
	}

	AssertEx(!_isLoadScening, "The scene is loading");
	if (file.IsEmpty())return false;

	_isLoadScening = true;
	String fullPath = file;
	if (_currScene && mode == LoadSceneMode::Single)
	{
		UnloadScene(_currScene->GetName());
	}

	//防止加载同名场景
	String name = Path::GetFileNameWithoutExtension(fullPath);
	for (const auto& it : _scenes)
	{
		if (it->GetName().Equals(name, true))
		{
			AssertEx(false, "Insert scene error:%s", fullPath.c_str());
			return false;
		}
	}

	Scene* scene = Scene::Create(name);
	if (!scene->Load(fullPath))
	{
		_isLoadScening = false;
		SAFE_DELETE(scene);
		return false;
	}
	_scenes.Add(scene);
	_currScene = scene;
	_isLoadScening = false;

	return true;
}
void SceneManager::UnloadScene(const String& name)
{
	for (auto it = _scenes.begin(); it != _scenes.end(); ++it)
	{
		Scene* scene = *it;
		if (scene && scene->GetName().Equals(name, true))
		{
			bool is_curr_scene = (scene == _currScene);
			SAFE_DELETE(scene);
			if (is_curr_scene)_currScene = nullptr;
			_scenes.Remove(it);
			break;
		}
	}
}
void SceneManager::UnloadScenes()
{
	AssertEx(!_isLoadScening, "UnloadScenes");
	for (auto it = _scenes.begin(); it != _scenes.end(); ++it)
	{
		Scene* scene = *it;
		if (scene)
		{
			SAFE_DELETE(scene);
		}
	}
	_scenes.Clear();
	_currScene = nullptr;
}
void SceneManager::SaveScene()
{
	if (Application::IsEditor() && Application::IsPlay())
	{
		Debuger::Message("Error", "Cannot save scene when running");
		return;
	}
	if (_currScene)
	{
		_currScene->Save();
	}
}
void SceneManager::SaveAsScene(const String& file)
{
	if (Application::IsEditor() && Application::IsPlay())
	{
		Debuger::Message("Error", "Cannot save scene when running");
		return;
	}
	if (_currScene)
	{
		_currScene->SaveAs(file);
	}
}
Scene* SceneManager::GetScene(const String& name)
{
	for (const auto& it : _scenes)
	{
		if (it->GetName().Equals(name, true))
		{
			return it;
		}
	}
	return nullptr;
}
Scene* SceneManager::GetScene(int index)
{
	if (index < 0 || index >= _scenes.Size())
		return nullptr;
	return _scenes[index];
}
DC_END_NAMESPACE