
/*****************************************************************************
* Author： hannibal
* Description：
*****************************************************************************/
#pragma once

#include "core/BaseType.h"

DC_BEGIN_NAMESPACE
class RenderWindow;
/********************************************************************/
class ENGINE_DLL PlatformBase : public object
{
	friend class Application;
	DISALLOW_COPY_ASSIGN(PlatformBase);
	BEGIN_REFECTION_TYPE(PlatformBase)
	END_REFECTION_TYPE;

	static void Initialize() {}
	static void Destroy() {}

public://文件
	//打开文件
	static std::string OpenFileDialog(const std::string& title, const std::string& directory, const std::string& defaultName, const std::string& extension, bool open) { return ""; }
	//打开目录
	static std::string OpenFolderDialog(const std::string& title, const std::string& initialFolder) { return ""; }

	//打开文件所在文件夹
	static void OpeninExplorer(const std::string& path) {}

public://监视目录
	static bool WatchFolder(const std::string& folder, const Function<void()>& callback) { return false; }

public://window
	//调整窗口大小
	static void AdjustWindowRect(int& left, int& top, int& width, int& height, int screenWidth, int screenHeight, bool fullScreen);

public:
	static bool Is64BitPlatform();
	static PlatformType GetPlatformType();
	static RendererAPI GetRendererAPI();
};
DC_END_NAMESPACE