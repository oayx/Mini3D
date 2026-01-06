#include "ProjectManager.h"
#include "runtime/resources/Resources.h"
#include "runtime/Application.h"
 
DC_BEGIN_NAMESPACE
static const String ProjectConfigFilePath = "data/settings/ProjectConfig.asset";
/********************************************************************/
IMPL_REFECTION_TYPE(ProjectManager);
void ProjectManager::Initialize()
{
	Load();
}
void ProjectManager::Destroy()
{
	Project* currProject = GetCurrProject();
	if (currProject)
	{
		currProject->Close();
	}

	for (auto project : _projectes)
	{
		SAFE_DELETE(project);
	}
	_projectes.Clear();
}
bool ProjectManager::OpenProject(const String& path)
{
	Project* oldProject = GetCurrProject();

	String fullPath = Path::ReplaceSplit(path);
	Project* project = Project::Create(fullPath);
	if (project->Open())
	{
		//关闭当前项目
		if (oldProject)oldProject->Close();

		RemoveProject(fullPath);
		_projectes.Add(project);
		Save();
		return true;
	}
	else
	{
		Debuger::Log("Open project failed:%s", path.c_str());
		SAFE_DELETE(project);
		return false;
	}
}
bool ProjectManager::CreateProject(const String& path)
{
	String fullPath = Path::ReplaceSplit(path);
	if (HasProject(fullPath))
	{
		Debuger::Error("The project has exist");
		return false;
	}

	Project* oldProject = GetCurrProject();
	Project* project = Project::Create(fullPath);
	if (project->Create())
	{
		//关闭当前项目
		if (oldProject)oldProject->Close();
		
		RemoveProject(fullPath);
		_projectes.Add(project);
		Save();
		return true;
	}
	else
	{
		SAFE_DELETE(project);
		return false;
	}
}
Project* ProjectManager::GetCurrProject()
{
	if (_projectes.IsEmpty())
		return nullptr;
	for (auto it = _projectes.rbegin(); it != _projectes.rend(); ++it)
	{
		Project* project = *it;
		if (project && project->IsOpen())
		{
			return project;
		}
	}
	return nullptr;
}
bool ProjectManager::IsCurrProjectAssets(const String& path)
{
	if (path.IsEmpty())return false;

	Project* currProject = GetCurrProject();
	if (!currProject)return false;

	String currAssetPath = Path::Combine(currProject->GetFullPath(), "assets");
	return path.StartsWith(currAssetPath);
}
String ProjectManager::ToProjectAssetsPath(const String& path)
{
	if (path.IsEmpty())return "";

	Project* currProject = GetCurrProject();
	if (!currProject)return "";

	String currAssetPath = Path::Combine(currProject->GetFullPath(), "assets");
	if(!path.StartsWith(currAssetPath))return "";

	return path.Substring(currAssetPath.Size() + 1);
}
bool ProjectManager::HasProject(const String& path)
{
	for (const auto& project : _projectes)
	{
		if (project->GetFullPath().Equals(path, true))
			return true;
	}
	return false;
}
void ProjectManager::RemoveProject(const String& path)
{
	for (auto it = _projectes.begin(); it != _projectes.end(); ++it)
	{
		Project* project = *it;
		if (project->GetFullPath().Equals(path, true))
		{
			SAFE_DELETE(project);
			_projectes.Remove(it);
			break;
		}
	}
}	
void ProjectManager::Load()
{
	if (!Application::IsEditor())return;

	_projectes.Clear();

	VecString paths;
	String fullPath = Resource::GetFullSavePath(ProjectConfigFilePath);
	if (File::Exist(fullPath))
	{
		SerializeRead transfer(fullPath);
		{
			if (transfer.Push("Projects"))
			{
				TRANSFER_VECTOR(paths, "Project");
				transfer.Pop();
			}
		}

		_projectes.Reserve(paths.Size());
		for (const auto& path : paths)
		{
			if (HasProject(path) || !File::Exist(path))continue;
			Project* project = Project::Create(path);
			_projectes.Add(project);
		}
	}
	//TODO:如果没有项目，打开默认的，临时代码，后期删除
	if (_projectes.IsEmpty())
	{
		String path = Path::GetCurrentProcessDirectory();
		path = path.Replace('\\', '/');
		if (path.LastIndexOf("/") >= 0)
		{
			path = path.Substring(0, path.LastIndexOf("/"));
			if (path.LastIndexOf("/") >= 0)
			{
				path = path.Substring(0, path.LastIndexOf("/"));
			}
		}
		if (File::Exist(path))
		{
			Project* project = Project::Create(path);
			_projectes.Add(project);
		}
	}
}
void ProjectManager::Save()
{
	if (!Application::IsEditor())return;

	VecString paths;
	for (auto it = _projectes.begin(); it != _projectes.end(); ++it)
	{
		Project* project = *it;
		if (project)
		{
			paths.Add(project->GetFullPath());
		}
	}

	String fullPath = Resource::GetFullSavePath(ProjectConfigFilePath);
	SerializeWrite transfer(fullPath);
	{
		if (transfer.Push("Projects"))
		{
			TRANSFER_VECTOR(paths, "Project");
			transfer.Pop();
		}
	}
}
DC_END_NAMESPACE