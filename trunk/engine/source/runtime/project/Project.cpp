#include "Project.h"
#include "QualitySettings.h"
#include "runtime/physics/Physics.h"
#include "runtime/physics2d/Physics2d.h"
#include "runtime/resources/AssetsManager.h"
#include "runtime/resources/Resources.h"
#include "runtime/scene/LayerMask.h"
#include "runtime/scene/SceneManager.h"
#include "runtime/scene/TagManager.h"
#include "runtime/Application.h"
 
DC_BEGIN_NAMESPACE
static const String ProjectFilePath = "settings/ProjectSettings.asset";
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(Project, Object);
void Project::Serialize()
{
	String fullPath = Resource::GetFullDataPath(ProjectFilePath);
	SerializeRead transfer(fullPath);
	this->Transfer(transfer);
}
void Project::Deserialize()
{
	String fullPath = Resource::GetFullDataPath(ProjectFilePath);
	SerializeWrite transfer(fullPath);
	this->Transfer(transfer);
}
bool Project::Create()
{
	if (File::Exist(_fullPath))
	{
		Debuger::Error("Create project must in empty path");
		return false;
	}

	if (!Directory::Create(_fullPath))
	{
		Debuger::Error("Create path error:%s", _fullPath.c_str());
		return false;
	}

	//创建完毕后直接打开
	return Open();
}
bool Project::Open()
{
	//卸载旧的
	SceneManager::UnloadScenes();

	//设置资源目录
	Application::SetAssetsPath(_fullPath + "/assets");

	//打开新的
	AssetsManager::Refresh();
	QualitySettings::Load();
	LayerMask::Load();
	TagManager::Load();
	Physics::Initialize();
	Physics2d::Initialize();
	this->Serialize();

	//打开场景
	if (!_currScene.IsEmpty())
	{
		SceneManager::LoadScene(_currScene, LoadSceneMode::Single);
	}
	else
	{
		SceneManager::CreateScene("New Scane");
	}

	_isOpen = true;
	return true;
}
void Project::Close() 
{ 
	if (_isOpen)
	{
		Physics::Destroy();
		Physics2d::Destroy();

		_isOpen = false;
		this->Deserialize();
	}
}
/********************************************************************/
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(Project);
template<class TransferFunction> inline
void Project::Transfer(TransferFunction& transfer, void* ptr)
{
	String SceneName = _currScene;
	if (!transfer.IsRead())
	{
		AssetMeta* meta = AssetsManager::GetMetaByFile(SceneName);
		if (meta)
			SceneName = meta->GetGUID();
		else
			SceneName = "";
	}

	TRANSFER_SIMPLE(SceneName);

	if (transfer.IsRead())
	{
		AssetMeta* meta = AssetsManager::GetMeta(SceneName);
		if (meta)
			_currScene = meta->GetFullPath();
		else
			_currScene = "";
	}
}
DC_END_NAMESPACE