#include "Directory.h"
#include "File.h"
#include "Path.h"
#include "core/Debuger.h"
#include "core/Utility.h"
#include "platform/PlatformDefine.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_REFECTION_TYPE(Directory);
#if defined(DC_PLATFORM_WIN32)
//flags:1-file,2-dir,3 file and dir
static void GetDirFiles(VecString& list, const String& path, SearchOption op, int flags = 1)
{
	if (path.IsEmpty())return;
	String search_path = Path::Combine(path, "*.*");
	std::wstring root_dir = Encoding::Utf8ToWChar(search_path.c_str(), search_path.Size());

	WIN32_FIND_DATAW find_data = {0};
	HANDLE find_handle = ::FindFirstFileW(root_dir.c_str(), &find_data);
	if (INVALID_HANDLE_VALUE == find_handle)return;
	do
	{
		if (::wcscmp(find_data.cFileName, L".") && ::wcscmp(find_data.cFileName, L"..") && ::wcscmp(find_data.cFileName, L".svn")) //如果不是"." ".."目录  
		{
			std::wstring w_file_path = find_data.cFileName;
			std::string file_path = Encoding::WCharToUtf8(find_data.cFileName);
			if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				String sub_dir = path + "/" + file_path;
				if (flags & 2)list.Add(sub_dir);
				if (op == SearchOption::AllDirectories)
				{
					GetDirFiles(list, sub_dir, op, flags);
				}
			}
			else if (find_data.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
			{
				if (flags & 1)list.Add(path +"/" + file_path);
			}
		}
	} while (::FindNextFileW(find_handle, &find_data));

	::FindClose(find_handle);
}
#elif defined(DC_PLATFORM_LINUX) || defined(DC_PLATFORM_ANDROID) || defined(DC_PLATFORM_MAC) || defined(DC_PLATFORM_IOS)
static void GetDirFiles(VecString& list, const String& path, SearchOption op, int flags = 1)
{
	DIR *dir = opendir(path.c_str());
	if (dir)
	{
		dirent* p;
		while ((p = readdir(dir)) != nullptr)
		{
			String name = p->d_name;
			if (p->d_type & DT_DIR)
			{
				if (name != "." && name != ".." && name != ".svn")
				{
					String sub_dir = path + "/" + name;
					if (flags & 2)list.Add(sub_dir);
					if (op == SearchOption::AllDirectories)
					{
						GetDirFiles(list, sub_dir, op, flags);
					}
				}
			}
			else if (p->d_type & DT_REG)
			{
				if (flags & 1)list.Add(path + "/" + name);
			}
		}
		closedir(dir);
	}
	else
	{
		Debuger::Error("Cannot open dir:%s", path.c_str());
	}
}
#endif

bool Directory::Create(const String& path)
{
	if (Exist(path))
	{
		return true;
	}
	auto splits = path.Split("/");
	if (splits.Size() == 0)
	{
		return true;
	}
	String folder = splits[0];
	if (path.StartsWith("/"))
	{
		folder = "/" + folder;
	}

	bool result = true;
	for (int i = 0; i < (int)splits.Size(); ++i)
	{
		if(i != 0)folder += "/" + splits[i];
		if (Exist(folder))continue;
#if defined(DC_PLATFORM_WIN32)
		std::wstring w_folder = Encoding::Utf8ToWChar(folder.c_str(), folder.Size());
		if (::CreateDirectoryW(w_folder.c_str(), nullptr) == FALSE)
		{
			result = false;
			break;
		}
#elif defined(DC_PLATFORM_LINUX) || defined(DC_PLATFORM_ANDROID) || defined(DC_PLATFORM_MAC) || defined(DC_PLATFORM_IOS)
		if (mkdir(folder.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0)
		{
			result = false;
			break;
		}
#endif
	}
	if (!result)
	{
		Debuger::Error("create directory error:%s", path.c_str());
	}
	return true;
}
#if defined(DC_PLATFORM_WIN32)
bool Directory::Delete(const String& path)
{
	std::wstring root_dir = Encoding::Utf8ToWChar(path.c_str(), path.Size());
	std::wstring curr_path = root_dir + L"/*.*";	//匹配格式为*.*,即该目录下的所有文件  
	WIN32_FIND_DATAW find_file_data = { 0 };
	HANDLE handle = ::FindFirstFileW(curr_path.c_str(), &find_file_data);
	bool is_finded = true;
	while (is_finded)
	{
		is_finded = ::FindNextFileW(handle, &find_file_data);			//递归搜索其他的文件  
		if (is_finded && ::wcscmp(find_file_data.cFileName, L".") && ::wcscmp(find_file_data.cFileName, L"..")) //如果不是"." ".."目录  
		{
			std::wstring w_file_path = root_dir + L"/" + find_file_data.cFileName;
			std::string file_path = Encoding::WCharToUtf8(w_file_path);
			if (IsDirectory(file_path))								//如果是目录，则递归地调用  
			{
				Delete(file_path);
			}
			else
			{
				::DeleteFileW(w_file_path.c_str());
			}
		}
	}
	::FindClose(handle);

	bool bRet = RemoveDirectoryW(root_dir.c_str());
	if (bRet == 0) //删除目录  
	{
		Debuger::Error("delete directory error:%s", path.c_str());
		return false;
	}
	return true;
}
#elif defined(DC_PLATFORM_LINUX) || defined(DC_PLATFORM_ANDROID) || defined(DC_PLATFORM_MAC) || defined(DC_PLATFORM_IOS)
bool Directory::Delete(const String& path)
{
	const char *dir = path.c_str();
	// 参数传递进来的目录不存在，直接返回
	if (0 != access(dir, F_OK))
	{
		return true;
	}
	struct stat dir_stat;
	// 获取目录属性失败，返回错误
	if (0 > stat(dir, &dir_stat)) 
	{
		Debuger::Error("delete directory error1:%s", path.c_str());
		return false;
	}
	bool result = false;
	if (S_ISREG(dir_stat.st_mode)) 
	{// 普通文件直接删除
		result = remove(dir) == 0;
	}
	else if (S_ISDIR(dir_stat.st_mode)) 
	{// 目录文件，递归删除目录中内容
		char cur_dir[] = ".";
		char up_dir[] = "..";
		struct dirent *dp;
		DIR *dirp = opendir(dir);
		while ((dp = readdir(dirp)) != NULL) 
		{
			if ((0 == strcmp(cur_dir, dp->d_name)) || (0 == strcmp(up_dir, dp->d_name))) 
			{// 忽略 . 和 ..
				continue;
			}
			Delete(path + "/" + dp->d_name);   // 递归调用
		}
		closedir(dirp);

		result = rmdir(dir) == 0;		// 删除空目录
	}
	if (!result) //删除目录  
	{
		Debuger::Error("delete directory error:%s", path.c_str());
	}
	return result;
}
#endif
bool Directory::Exist(const String& path)
{
	if (path.IsEmpty())return false;
	return ::access(path.c_str(), 0) == 0;
}
bool Directory::IsDirectory(const String& path)
{
	if (!File::Exist(path))return false;
#if defined(DC_PLATFORM_WIN32)
	std::wstring w_path = Encoding::Utf8ToWChar(path.c_str(), path.Size());
	DWORD flags = ::GetFileAttributesW(w_path.c_str());
	bool result = false;
	if (flags & FILE_ATTRIBUTE_DIRECTORY)
	{
		result = true;
	}
	return result;
#elif defined(DC_PLATFORM_LINUX) || defined(DC_PLATFORM_ANDROID) || defined(DC_PLATFORM_MAC) || defined(DC_PLATFORM_IOS)
	const char *dir = path.c_str();
	struct stat st;
	stat(dir, &st);
	{// 获取目录属性
		if (S_ISDIR(st.st_mode))
		{// 目录
			return true;
		}
	}

	return false;
#endif
}
bool Directory::Move(const String& src_path, const String& dst_path)
{
	String path = Path::GetDirectoryName(dst_path);
	if (!Directory::Exist(path))
		Directory::Create(path);

#if defined(DC_PLATFORM_WIN32)
	std::wstring w_src_path = Encoding::Utf8ToWChar(src_path.c_str(), src_path.Size());
	std::wstring w_dst_path = Encoding::Utf8ToWChar(dst_path.c_str(), dst_path.Size());
	return MoveFileW(w_src_path.c_str(), w_dst_path.c_str()) == 1;
#elif defined(DC_PLATFORM_LINUX) || defined(DC_PLATFORM_ANDROID) || defined(DC_PLATFORM_MAC) || defined(DC_PLATFORM_IOS)
	return ::rename(src_path.c_str(), dst_path.c_str()) == 0;
#endif
}

void Directory::GetDirectorys(const String& path, SearchOption op, VecString& list)
{
	list.Clear();
	GetDirFiles(list, path, op, 2);
}

void Directory::GetFiles(const String& path, SearchOption op, VecString& list)
{
	list.Clear();
	GetDirFiles(list, path, op, 1);
}
void Directory::GetFiles(const String& path, SearchOption op, Vector<FileInfo>& list)
{
	list.Clear();
	VecString list_files;
	GetDirFiles(list_files, path, SearchOption::TopDirectoryOnly, 3);
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