#include "Win32Platform.h"
#include "Win32FolderMonitor.h"
#include "core/Encoding.h"
#include "core/file/Path.h"
#include <windows.h>
#include <commdlg.h>
#include <ShlObj.h>
#include <shlwapi.h>
#include <shellapi.h>

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(Win32Platform, PlatformBase);
void Win32Platform::Initialize()
{
}
void Win32Platform::Destroy()
{
	Win32FolderMonitor::Destroy();
}
std::string Win32Platform::OpenFileDialog(const std::string& title, const std::string& directory, const std::string& defaultName, const std::string& extension, bool open)
{
	std::string folder = String(directory).Replace('/', '\\');

	std::wstring initialDir = Encoding::Utf8ToWChar(folder);
	std::wstring titleWide = Encoding::Utf8ToWChar(title);
	wchar_t widePathName[MAX_PATH] = { 0 };
	wchar_t extensionWide[MAX_PATH] = { 0 };
	wchar_t szMyFilter[MAX_PATH] = { 0 };
	wchar_t szFileTitle[MAX_PATH] = { 0 };

	Encoding::Utf8ToWChar(defaultName.c_str(), widePathName, MAX_PATH);

	if (!extension.empty())
	{
		Encoding::Utf8ToWChar(extension.c_str(), extensionWide, MAX_PATH);
		::wsprintfW(szMyFilter, L"%s?*.%s?All files?*.*??", extensionWide, extensionWide);
	}
	else
	{
		extensionWide[0] = 0;
		::wcscpy(szMyFilter, L"All files?*.*??");
	}

	int stlen = ::wcslen(szMyFilter);
	for (int i = 0; i < stlen; i++)
		if (szMyFilter[i] == L'?')
			szMyFilter[i] = 0;

	OPENFILENAMEW ofn = { 0 };
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = nullptr;
	ofn.hInstance = ::GetModuleHandle(0);
	ofn.lpstrTitle = titleWide.c_str();
	ofn.lpstrFilter = szMyFilter;
	ofn.lpstrFile = widePathName;
	ofn.nMaxFile = 4 * MAX_PATH;
	ofn.lpstrFileTitle = szFileTitle;
	ofn.nMaxFileTitle = MAX_PATH;
	ofn.lpstrInitialDir = initialDir.c_str();
	ofn.lpstrDefExt = extensionWide;
	ofn.Flags = OFN_ENABLEHOOK;

	std::string result = "";
	if (open)
	{
		ofn.Flags |= OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_HIDEREADONLY;
		if (::GetOpenFileNameW(&ofn) != 0)
		{
			result = Encoding::WCharToUtf8(ofn.lpstrFile);
		}
	}
	else
	{
		ofn.Flags |= OFN_EXPLORER | OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT;
		if (::GetSaveFileNameW(&ofn) != 0)
		{
			result = Encoding::WCharToUtf8(ofn.lpstrFile);
		}
	}
	std::replace(result.begin(), result.end(), '\\', '/');
	return result;
}

static int CALLBACK OnBrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	switch (uMsg)
	{
	case BFFM_INITIALIZED:
	{
		::SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
		break;
	}
	case BFFM_SELCHANGED:
	{
		wchar_t curr[MAX_PATH];
		::SHGetPathFromIDList((LPCITEMIDLIST)lParam, curr);
		::SendMessage(hwnd, BFFM_SETSTATUSTEXT, 0, (LPARAM)curr);
		break;
	}
	}

	return 0;
}
std::string Win32Platform::OpenFolderDialog(const std::string& title, const std::string& initialFolder)
{
	std::wstring titleWide = Encoding::Utf8ToWChar(title);
	std::wstring path = Encoding::Utf8ToWChar(initialFolder);
	ConvertSeparatorsToWindows(path);

	wchar_t result[MAX_PATH] = { 0 };

	BROWSEINFO bi = { 0 };
	bi.hwndOwner = NULL;
	bi.pszDisplayName = result;
	bi.lpszTitle = titleWide.c_str();
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_RETURNFSANCESTORS | BIF_VALIDATE | BIF_NEWDIALOGSTYLE | BIF_DONTGOBELOWDOMAIN;
	bi.lpfn = OnBrowseCallbackProc;
	bi.lParam = (LPARAM)path.c_str();

	LPITEMIDLIST idl = ::SHBrowseForFolder(&bi);
	if (idl)
	{
		if (!::SHGetPathFromIDList(idl, result))
		{
			result[0] = '\0';
		}
	}

	std::string ret_path = Encoding::WCharToUtf8(result);
	std::replace(ret_path.begin(), ret_path.end(), '\\', '/');
	return ret_path;
}
void Win32Platform::OpeninExplorer(const std::string& path)
{
	String folder = path;
	if (!Path::GetExtension(folder).IsEmpty())
	{
		folder = Path::GetDirectoryName(folder);
	}

	std::wstring w_path = folder.ToWString();
	std::replace(w_path.begin(), w_path.end(), L'/', L'\\');
	::ShellExecuteW(NULL, L"open", L"explorer.exe", w_path.c_str(), NULL, SW_SHOWNORMAL);
}
bool Win32Platform::WatchFolder(const std::string& folder, const Function<void()>& callback)
{
	return Win32FolderMonitor::Watch(folder, callback);
}
void Win32Platform::AdjustWindowRect(int& left, int& top, int& width, int& height, int screen_width, int screen_height, bool full_screen)
{
	int init_width = width; int init_height = height;
	//限定窗口大小不超过屏幕尺寸
	if (!full_screen)
	{
		if (init_width == 0 || init_height == 0)
		{//最大化
			HWND hwnd = ::FindWindow(L"Shell_TrayWnd", L"");
			if (hwnd)
			{//任务栏
				RECT rect;
				::GetWindowRect(hwnd, &rect);
				if (rect.left == 0 && rect.right > (screen_width >> 1))
				{//任务栏在上下
					width = rect.right - rect.left;
					height = screen_height - (rect.bottom - rect.top);

					left = 0;
					top = rect.bottom == screen_height ? 0 : rect.bottom;
				}
				else
				{//任务栏在左右
					width = screen_width - (rect.right - rect.left);
					height = rect.bottom - rect.top;

					left = rect.right == screen_width ? 0 : rect.right;
					top = 0;
				}
			}
		}
		else
		{
			if (width > screen_width) width = screen_width;
			if (height > screen_height) height = screen_height;
		}
	}
	else
	{
		width = screen_width;
		height = screen_height;
	}

	//获得最终窗口大小
	RECT rect = { 0, 0, width, height };
	::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
	width = rect.right - rect.left;
	height = rect.bottom - rect.top;

	//某个方向的位置为负数则使窗口剧中
	if (full_screen)
	{
		left = top = 0;
	}
	else
	{
		if (init_width != 0 && init_height != 0)
		{
			if (left < 0)left = (screen_width - width) / 2;
			if (top < 0)top = (screen_height - height) / 2;
		}
	}
}
DC_END_NAMESPACE