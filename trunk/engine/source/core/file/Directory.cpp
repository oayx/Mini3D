#include "Directory.h"
#include "File.h"
#include "Path.h"
#include "core/Debuger.h"
#include "core/Utility.h"
#include "platform/PlatformDefine.h"
#include <filesystem>
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_REFECTION_TYPE(Directory);
bool Directory::Create(const String& path)
{
	DC_PROFILE_FUNCTION;
	std::error_code ec;
	std::filesystem::create_directories(path.c_str(), ec);
	if (ec)
	{
#if DC_DEBUG
		Debuger::Error("create directory:%s, error:%s", path.c_str(), ec.message().c_str());
#endif
		return false;
	}
	return true;
}
bool Directory::Delete(const String& path)
{
	DC_PROFILE_FUNCTION;
	std::error_code ec;
	std::filesystem::remove_all(path.c_str(), ec);
	if (ec)
	{
#if DC_DEBUG
		Debuger::Error("delete directory:%s, error:%s", path.c_str(), ec.message().c_str());
#endif
		return false;
	}
	return true;
}
bool Directory::Exist(const String& path)
{
	DC_PROFILE_FUNCTION;
	if (path.IsEmpty())return false;
	std::error_code ec;
	bool result = std::filesystem::exists(path.c_str(), ec);
	if (ec)
	{
#if DC_DEBUG
		Debuger::Error("exist dir:%s error:%s", path.c_str(), ec.message().c_str());
#endif
		return false;
	}
	return result;
}
bool Directory::IsDirectory(const String& path)
{
	DC_PROFILE_FUNCTION;
	if (!Exist(path))return false;
	return std::filesystem::is_directory(path.c_str());
}
bool Directory::Move(const String& srcPath, const String& dstPath)
{
	DC_PROFILE_FUNCTION;
	if (!Exist(srcPath))
		return false;

	std::error_code ec;
	std::filesystem::rename(srcPath.c_str(), dstPath.c_str(), ec);
	if (ec)
	{
#if DC_DEBUG
		Debuger::Error("rename dir:%s -> %s error:%s", srcPath.c_str(), dstPath.c_str(), ec.message().c_str());
#endif
		return false;
	}
	return true;
}
bool Directory::Rename(const String& srcPath, const String& new_name)
{
	DC_PROFILE_FUNCTION;
	if (!Exist(srcPath))
		return false;

	std::string dstPath = Path::GetDirectoryName(srcPath) + "/" + new_name;

	std::error_code ec;
	std::filesystem::rename(srcPath.c_str(), dstPath, ec);
	if (ec)
	{
#if DC_DEBUG
		Debuger::Error("rename dir:%s -> %s error:%s", srcPath.c_str(), dstPath.c_str(), ec.message().c_str());
#endif
		return false;
	}
	return true;
}

//flags:1-file,2-dir,3 file and dir
static void GetDirFiles(const String& path, SearchOption op, int flags, VecString& list)
{
	if (path.IsEmpty())return;

	if (op == SearchOption::AllDirectories)
	{
		for (auto& entry : std::filesystem::recursive_directory_iterator(path.c_str()))
		{
			if (((flags & 1) && entry.is_regular_file()) || ((flags & 2) && entry.is_directory()))
			{
				std::string fullPath = entry.path().string();
#if defined(DC_PLATFORM_WIN32)
				std::replace(fullPath.begin(), fullPath.end(), '\\', '/');
#endif
				list.Add(fullPath);
			}
		}
	}
	else
	{
		for (auto& entry : std::filesystem::directory_iterator(path.c_str()))
		{
			if (((flags & 1) && entry.is_regular_file()) || ((flags & 2) && entry.is_directory()))
			{
				std::string fullPath = entry.path().string();
#if defined(DC_PLATFORM_WIN32)
				std::replace(fullPath.begin(), fullPath.end(), '\\', '/');
#endif
				list.Add(fullPath);
			}
		}
	}
}
void Directory::GetDirectorys(const String& path, SearchOption op, VecString& list)
{
	DC_PROFILE_FUNCTION;
	list.Clear();
	GetDirFiles(path, op, 2, list);
}

void Directory::GetFiles(const String& path, SearchOption op, VecString& list)
{
	list.Clear();
	GetDirFiles(path, op, 3, list);
}
void Directory::GetFileInfos(const String& path, SearchOption op, Vector<FileInfo>& list)
{
	DC_PROFILE_FUNCTION;
	list.Clear();
	VecString list_files;
	GetDirFiles(path, SearchOption::TopDirectoryOnly, 3, list_files);
	for (const auto& file : list_files)
	{
		if (op == SearchOption::AllDirectories)
		{
			FileInfo info(file, op);
			list.Add(info);
		}
		else
		{

			FileInfo info(file);
			list.Add(info);
		}
	}
}
DC_END_NAMESPACE