#include "Scene.h"
#include "runtime/component/GameObject.h"
#include "runtime/graphics/Material.h"
#include "runtime/project/ProjectManager.h"
#include "runtime/renderable/CubeRender.h"
#include "runtime/resources/AssetsManager.h"
#include "runtime/mesh/MeshRender.h"
#include "runtime/scene/Camera.h"
#include "runtime/scene/Light.h"
#include "runtime/Screen.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(Scene, Object);
Scene::Scene(const String& name)
	: _name(name)
{
	_rootObject = GameObject::Instantiation(name);
}
Scene::~Scene()
{
	if (_rootObject)
	{
		GameObject::Destroy(_rootObject);
		_rootObject = nullptr;
	}
}
bool Scene::Load(const String& file)
{
	Debuger::Log("Load scene:%s", file.c_str());
	_file = file;
	this->Serialize();

	Project* project = ProjectManager::GetCurrProject();
	if (project)
	{
		project->SetCurrScene(file);
	}
	return true;
}
bool Scene::Save()
{
	if (_file.IsEmpty())
	{
		std::string file = Platform::OpenFileDialog("New Scene", ".", "", "scene", false);
		if (!file.empty())
		{
			if (!ProjectManager::IsCurrProjectAssets(file))
			{
				Debuger::Error("Is not current project path:%s", file.c_str());
				return false;
			}

			String scene_path = ProjectManager::ToProjectAssetsPath(file);
			if (SaveToFile(scene_path))
			{
				Project* project = ProjectManager::GetCurrProject();
				if (project)
				{
					project->SetCurrScene(scene_path);
					_name = Path::GetFileNameWithoutExtension(_file);
				}
				return true;
			}
		}
		return false;
	}
	else
	{
		return SaveToFile(_file);
	}
}
bool Scene::SaveAs(const String& file)
{
	bool result = SaveToFile(file);
	if (result)
	{
		Project* project = ProjectManager::GetCurrProject();
		if (project)
		{
			project->SetCurrScene(_file);
			_name = Path::GetFileNameWithoutExtension(_file);
		}
	}
	return result;
}
bool Scene::SaveToFile(const String& file)
{
	if (file.IsEmpty())return false;

	String asset_path = Resource::GetFullDataPath(file);
	//if (!File::Delete(asset_path))return false;
	//if (!File::Create(asset_path))return false;

	_file = file;
	this->Deserialize();

	Debuger::Log("Save scene:%s", file.c_str());
	return true;
}
void Scene::Serialize()
{
	String full_path = Resource::GetFullDataPath(_file);
	SerializeRead transfer(full_path);
	_MY_TRY
	{
		this->Transfer(transfer);
	}_MY_CATCH;
}
void Scene::Deserialize()
{
	String full_path = Resource::GetFullDataPath(_file);
	SerializeWrite transfer(full_path);
	_MY_TRY
	{
		this->Transfer(transfer);
	}_MY_CATCH;
}
template<class TransferFunction> inline
void Scene::Transfer(TransferFunction& transfer, void* ptr)
{
	if(transfer.Push("Lighting"))
	{
		{//环境光
			TRANSFER_SIMPLE(_ambientColor);
		}

		{//天空盒
			String SkyboxMaterial = "";
			if (!transfer.IsRead())
			{
				AssetMeta* meta = AssetsManager::GetMetaByFile(_skyboxMaterial);
				if (meta)SkyboxMaterial = meta->GetGUID();
			}
			TRANSFER_SIMPLE(SkyboxMaterial);
			if (transfer.IsRead())
			{
				AssetMeta* meta = AssetsManager::GetMeta(SkyboxMaterial);
				if (meta)_skyboxMaterial = meta->GetFullPath();
			}
		}

		{//反射
			TRANSFER_SIMPLE(_reflectionsIntensity);
		}
	}
	_fogDesc.Transfer(transfer);
	if (transfer.Push("GameObject"))
	{
		_rootObject->Transfer(transfer, ptr);
		transfer.Pop();
	}
}
void Scene::CreateDefault()
{
	{
		GameObject* camera_obj = GameObject::Instantiation("Main Camera");
		camera_obj->GetTransform()->SetLocalPosition(Vector3(0, 10, -10));
		camera_obj->GetTransform()->LookAt(Vector3(0, 0, 0), Vector3::up);
		Camera* camera = camera_obj->AddComponent<Camera>();
		camera->SetAspect(Screen::GetWidth() / Screen::GetHeight());
		camera->SetClearColor(Color(0.5f, 0.5f, 0.5f, 0));


		GameObject* light_obj = GameObject::Instantiation("Directional Light");
		light_obj->AddComponent<Light>();
	}
}
Transform* Scene::GetRootNode()const
{ 
	return _rootObject->GetTransform();
}
DC_END_NAMESPACE