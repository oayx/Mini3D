#include "Path.h"
#include "core/Debuger.h"
#include "core/Encoding.h"
#include "platform/PlatformDefine.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_REFECTION_TYPE(Path);
bool Path::SetCurrentProcessDirectory(const String& path)
{
	if (path.IsEmpty())
		return false;
	bool ret = false;
#if defined(DC_PLATFORM_WIN32)
	std::wstring w_path = Encoding::Utf8ToWChar(path.c_str(), path.Size());
	ret = ::SetCurrentDirectoryW(w_path.c_str()) != false;
#elif defined(DC_PLATFORM_LINUX) || defined(DC_PLATFORM_ANDROID) || defined(DC_PLATFORM_MAC) || defined(DC_PLATFORM_IOS)
	ret = chdir(path.c_str()) == 0;
#endif

#if DC_DEBUG
	if (!ret)Debuger::Error("Path::SetCurrentDirectory - failed:%s", path.c_str());
#endif
	return ret;
}
String Path::GetCurrentProcessDirectory()
{
	bool ret = false;
	char path[MAX_FILE_PATH] = { 0 };
#if defined(DC_PLATFORM_WIN32)
	wchar_t w_path[MAX_FILE_PATH] = { 0 };
	ret = ::GetCurrentDirectoryW(sizeof(w_path), w_path) != false;
	if (ret)Encoding::WCharToUtf8(w_path, path, sizeof(path));
#elif defined(DC_PLATFORM_LINUX) || defined(DC_PLATFORM_ANDROID) || defined(DC_PLATFORM_MAC) || defined(DC_PLATFORM_IOS)
	ret = getcwd(path, sizeof(path)) != NULL;
#endif

#if DC_DEBUG
	if (!ret)Debuger::Error("Path::GetCurrentDirectory - failed");
#endif

	if (ret)
		return String(path);
	else
		return "";
}
String Path::Combine(const String& path1, const String& path2) 
{ 
	if (path1.IsEmpty())return path2;
	if (path2.IsEmpty())return path1;

	String _path2 = Path::ReplaceSplit(path2);
	if (_path2[0] != '/')_path2 = "/" + _path2;

	return ReplaceSplit(path1 + _path2);
}
String Path::GetDirectoryName(const String& path)
{
	if (path.IsEmpty())return "";
	String str_path = ReplaceSplit(path);

	int index = str_path.LastIndexOf("/");
	if (index <= 0)return "";
	return str_path.Substring(0, index);
}
String Path::GetFolderName(const String& path)
{
	if (path.IsEmpty())return "";
	String str_path = ReplaceSplit(path);

	int index = str_path.LastIndexOf("/");
	if (index <= 0)return "";
	str_path = str_path.Substring(0, index);

	index = str_path.LastIndexOf("/");
	if (index <= 0)return str_path;
	return str_path.Substring(index+1);
}
String Path::GetExtension(const String& path)
{
	if (path.IsEmpty())return "";
	String str_path = ReplaceSplit(path);

	int index = str_path.LastIndexOf(".");
	if (index < 0 || index == path.Size() - 1)return "";
	int index_ = str_path.LastIndexOf("/");
	if (index_ >= 0 && index_ > index)return "";
	return str_path.Substring(index+1);
}
String Path::ReplaceExtension(const String& path, const String& new_ext)
{
	if (path.IsEmpty())return "";

	String file_name = GetFileNameWithoutExtension(path);
	return file_name + "." + new_ext;
}
String Path::GetFileName(const String& path)
{
	if (path.IsEmpty())return "";
	String str_path = ReplaceSplit(path);

	int index = str_path.LastIndexOf("/");
	if (index < 0)return path;
	else if (index == path.Size() - 1)return "";
	return str_path.Substring(index + 1);
}
String Path::GetFileNameWithoutExtension(const String& path)
{
	String file_name = GetFileName(path);
	if (file_name.IsEmpty())return "";

	int index = file_name.LastIndexOf(".");
	if (index < 0)return file_name;
	return file_name.Substring(0, index);
}
String Path::GetFullDataPath(const String& path)
{
	if (path.IsEmpty())return "";
	String str_path = ReplaceSplit(path);

	return str_path;
}
String Path::GetPathRoot(const String& path)
{
	if (path.IsEmpty())return "";
	String str_path = ReplaceSplit(path);
	if (str_path.IndexOf('/') >= 0)
	{
		String root_path = str_path.Substring(0, str_path.IndexOf('/'));
		return root_path;
	}
	return "";
}
bool Path::HasExtension(const String& path)
{
	String ext = GetExtension(path);
	return !ext.IsEmpty();
}
bool Path::IsFolder(const String& path)
{
	return !HasExtension(path);
}
String Path::ReplaceSplit(const String& path)
{
	if (path.IsEmpty())return "";
	return path.Replace('\\', '/');
}
DC_END_NAMESPACE
