
/*****************************************************************************
* Author： hannibal
* Description：项目管理
*****************************************************************************/
#pragma once

#include "Project.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL ProjectManager final : public object
{
	friend class Application;
	typedef Vector<Project*> Projectes;
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(ProjectManager);
	BEGIN_REFECTION_TYPE(ProjectManager)
	END_FINAL_REFECTION_TYPE;

	static void Initialize();
	static void Destroy();

public:
	//打开项目，path为绝对路径
	static bool OpenProject(const String& path);
	//创建项目，path为绝对路径
	static bool CreateProject(const String& path);
	//当前项目
	static Project* GetCurrProject();
	static Project* GetProject(int index) { CHECK_RETURN_PTR_NULL(index >= 0 && index < _projectes.Size()); return _projectes[index]; }
	static int		GetProjectCount() { return _projectes.Size(); }

	//是否当前项目下的asset资源
	static bool		IsCurrProjectAssets(const String& path);
	//转到项目目录
	static String	ToProjectAssetsPath(const String& path);



private:
	static bool HasProject(const String& path);
	static void RemoveProject(const String& path);

	static void Load();
	static void Save();

private:
	inline static Projectes _projectes;
};
DC_END_NAMESPACE