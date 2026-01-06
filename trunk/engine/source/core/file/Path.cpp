#include "Path.h"
#include "core/Debuger.h"
#include "core/Encoding.h"
#include "platform/PlatformDefine.h"
#include <filesystem>
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_REFECTION_TYPE(Path);
bool Path::SetCurrentProcessDirectory(const String& path)
{
	if (path.IsEmpty())
		return false;

	std::error_code ec; // 用于处理错误
	std::filesystem::current_path(path.c_str(), ec);
	if (ec) 
	{
#if DC_DEBUG
		Debuger::Error("set current process directory:%s, error:%s", path.c_str(), ec.message().c_str());
#endif
		return false;
	}
	return true;
}
String Path::GetCurrentProcessDirectory()
{
	std::filesystem::path currentPath = std::filesystem::current_path();
	return currentPath.string();
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
	std::filesystem::path temp = path.c_str();
	return temp.parent_path().string();
}
String Path::GetFolderName(const String& path)
{
	if (path.IsEmpty())return "";
	String strPath = ReplaceSplit(path);

	int index = strPath.LastIndexOf("/");
	if (index <= 0)return "";
	strPath = strPath.Substring(0, index);

	index = strPath.LastIndexOf("/");
	if (index <= 0)return strPath;
	return strPath.Substring(index + 1);
}
String Path::GetExtension(const String& path)
{
	if (path.IsEmpty())return "";
	std::filesystem::path temp = path.c_str();
	return temp.extension().string();
}
String Path::ReplaceExtension(const String& path, const String& new_ext)
{
	if (path.IsEmpty())return "";
	std::filesystem::path temp = path.c_str();
	temp.replace_extension(new_ext.c_str());
	return temp.string();
}
String Path::GetFileName(const String& path)
{
	if (path.IsEmpty())return "";
	std::filesystem::path temp = path.c_str();
	return temp.filename().string();
}
String Path::GetFileNameWithoutExtension(const String& path)
{
	if (path.IsEmpty())return "";
	std::filesystem::path temp = path.c_str();
	return temp.filename().stem().string();
}
String Path::GetFullDataPath(const String& path)
{
	if (path.IsEmpty())return "";
	return ReplaceSplit(path);
}
String Path::GetPathRoot(const String& path)
{
	std::filesystem::path temp = path.c_str();
	return temp.filename().root_name().string();
}
String Path::GetAbsolutePath(const String& path)
{
	if (path.IsEmpty())return "";
	std::filesystem::path absolutePath = std::filesystem::absolute(path.c_str());
	return absolutePath.string();
}
String Path::GetRelativePath(const String& file, const String& path)
{
	std::filesystem::path releative_path = std::filesystem::relative(file.c_str(), path.c_str());
	return releative_path.string();
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
