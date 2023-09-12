#include "ProjectManager.h"
#include "runtime/resources/Resources.h"
#include "runtime/Application.h"
 
DC_BEGIN_NAMESPACE
static const String ProjectConfigFilePath = "data/settings/ProjectConfig.asset";
/********************************************************************/
IMPL_REFECTION_TYPE(ProjectManager);
ProjectManager::Projectes ProjectManager::_projectes;
void ProjectManager::Initialize()
{
	Load();
}
void ProjectManager::Destroy()
{
	Project* curr_project = GetCurrProject();
	if (curr_project)
	{
		curr_project->Close();
	}

	for (auto project : _projectes)
	{
		SAFE_DELETE(project);
	}
	_projectes.Clear();
}
bool ProjectManager::OpenProject(const String& path)
{
	Project* old_project = GetCurrProject();

	String full_path = Path::ReplaceSplit(path);
	Project* project = Project::Create(full_path);
	if (project->Open())
	{
		//关闭当前项目
		if (old_project)old_project->Close();

		RemoveProject(full_path);
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
	String full_path = Path::ReplaceSplit(path);
	if (HasProject(full_path))
	{
		Debuger::Error("The project has exist");
		return false;
	}

	Project* old_project = GetCurrProject();
	Project* project = Project::Create(full_path);
	if (project->Create())
	{
		//关闭当前项目
		if (old_project)old_project->Close();
		
		RemoveProject(full_path);
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

	Project* curr_project = GetCurrProject();
	if (!curr_project)return false;

	String curr_asset_path = Path::Combine(curr_project->GetFullPath(), "assets");
	return path.StartsWith(curr_asset_path);
}
String ProjectManager::ToProjectAssetsPath(const String& path)
{
	if (path.IsEmpty())return "";

	Project* curr_project = GetCurrProject();
	if (!curr_project)return "";

	String curr_asset_path = Path::Combine(curr_project->GetFullPath(), "assets");
	if(!path.StartsWith(curr_asset_path))return "";

	return path.Substring(curr_asset_path.Size() + 1);
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
	String full_path = Resource::GetFullSavePath(ProjectConfigFilePath);
	if (File::Exist(full_path))
	{
		SerializeRead transfer(full_path);
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

	String full_path = Resource::GetFullSavePath(ProjectConfigFilePath);
	SerializeWrite transfer(full_path);
	{
		if (transfer.Push("Projects"))
		{
			TRANSFER_VECTOR(paths, "Project");
			transfer.Pop();
		}
	}
}
DC_END_NAMESPACE