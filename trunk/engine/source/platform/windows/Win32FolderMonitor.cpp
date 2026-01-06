#include "Win32FolderMonitor.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(Win32FolderMonitor, Object);
bool Win32FolderMonitor::Watch(const std::string& folder, const Function<void()>& callback)
{
	if (folder.empty())return false;
	//先关闭
	if (!_isClose && !_watchFolder.empty())
	{
		_isClose = true;
		_handle.join();
	}

	_isClose = false;
	_watchFolder = folder;
	_callback = callback;
	_handle = std::thread(&Win32FolderMonitor::Run);
	return true;
}
void Win32FolderMonitor::Destroy()
{
	if (!_isClose && _handle.joinable())
	{
		_isClose = true;
		_handle.join();
	}
}
void Win32FolderMonitor::Run()
{
	const DWORD dwNotificationFlags = FILE_NOTIFY_CHANGE_CREATION
		| FILE_NOTIFY_CHANGE_FILE_NAME;

	// 只监视实际的写入
	std::wstring wPath = Encoding::Utf8ToWChar(_watchFolder);
	HANDLE hFind = ::FindFirstChangeNotification(wPath.c_str(), TRUE, dwNotificationFlags);
	if (INVALID_HANDLE_VALUE == hFind)
	{
		return;
	}

	while (!_isClose)
	{
		DWORD dwWait = ::WaitForSingleObject(hFind, 1000);
		if (WAIT_OBJECT_0 == dwWait)
		{
			//有改变
			_callback();

			if (!::FindNextChangeNotification(hFind))
			{
				::FindCloseChangeNotification(hFind);
				hFind = NULL;
				return;
			}
		}
	}

	::FindCloseChangeNotification(hFind);
	hFind = NULL;
}
DC_END_NAMESPACE