
/*****************************************************************************
* Author： hannibal
* Description：
*****************************************************************************/
#pragma once
#include "platform/PlatformBase.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class Win32Platform final : public PlatformBase
{
	friend class Application;
	DEFAULT_CREATE(Win32Platform);
	FRIEND_CONSTRUCT_DESTRUCT(Win32Platform);
	DISALLOW_COPY_ASSIGN(Win32Platform);
	BEGIN_DERIVED_REFECTION_TYPE(Win32Platform, PlatformBase)
	END_REFECTION_TYPE;

	static void Initialize();
	static void Destroy();

public://文件
	static std::string OpenFileDialog(const std::string& title, const std::string& directory, const std::string& defaultName, const std::string& extension, bool open);
	static std::string OpenFolderDialog(const std::string& title, const std::string& initialFolder);
	
	static void OpeninExplorer(const std::string& path);

public://监视目录
	static bool WatchFolder(const std::string& folder, const Function<void()>& callback);

public://window
	//调整窗口大小
	static void AdjustWindowRect(int& left, int& top, int& width, int& height, int screenWidth, int screenHeight, bool fullScreen);
};
DC_END_NAMESPACE