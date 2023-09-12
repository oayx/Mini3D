
/*****************************************************************************
* Author： hannibal
* Description： 目录监视
*****************************************************************************/
#pragma once

#include "core/Object.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class Win32FolderMonitor : public Object
{
	friend class Win32Platform;
	DISALLOW_COPY_ASSIGN(Win32FolderMonitor);
	BEGIN_DERIVED_REFECTION_TYPE(Win32FolderMonitor, Object)
	END_DERIVED_REFECTION_TYPE;

	static void Destroy();

	static bool Watch(const std::string& folder, const Function<void()>& callback);
	static void Run();

private:
	static bool _isClose;
	static std::thread	_handle;
	static std::string _watchFolder;
	static Function<void()> _callback;
};
DC_END_NAMESPACE