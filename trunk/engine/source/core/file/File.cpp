#include "File.h" 
#include "Path.h"
#include "Directory.h"
#include "core/Debuger.h"
#include "core/stream/DataStream.h"
#include "platform/PlatformDefine.h"
#include <zlib/unzip.h>

#if defined(DC_PLATFORM_WIN32)
#include <io.h>
#elif defined(DC_PLATFORM_LINUX) || defined(DC_PLATFORM_ANDROID) || defined(DC_PLATFORM_MAC) || defined(DC_PLATFORM_IOS)
#include <stdio.h>
#include <unistd.h>
#endif
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_REFECTION_TYPE(FileInfo);
FileInfo::FileInfo(const String& fileName)
: _filePath(fileName)
{
	FileType = Directory::IsDirectory(_filePath) ? FileInfoType::Dir : FileInfoType::File;
	FileName = Path::GetFileName(_filePath);
	if (FileType == FileInfoType::File)
	{
		Extension = Path::GetExtension(_filePath);
	}
	FullName = _filePath;
}
FileInfo::FileInfo(const String& fileName, SearchOption op)
	: FileInfo(fileName)
{
	if (FileType == FileInfoType::Dir)
	{
		Directory::GetFiles(_filePath, op, _childrens);
	}
}
bool FileInfo::Exist()const
{
	return File::Exist(_filePath);
}
bool FileInfo::HasChildren(const String& name)
{
	for (int i = 0; i < _childrens.Size(); ++i)
	{
		const FileInfo& file_info = _childrens[i];
		if (file_info.FileName.Equals(name, true))
			return true;
	}
	return false;
}
/********************************************************************/
IMPL_REFECTION_TYPE(File);
bool File::Create(const String& full_path)
{
	DC_PROFILE_FUNCTION();
	if (File::Exist(full_path))return false;

	String path = Path::GetDirectoryName(full_path);
	if (!path.IsEmpty() && !Directory::Exist(path))
		Directory::Create(path);

	FileDataStream file(full_path, "w");
	return file.IsOpen();
}
bool File::Delete(const String& full_path)
{
	DC_PROFILE_FUNCTION();
	if (full_path.IsEmpty())return false;
	if (!File::Exist(full_path))return true;

#if defined(DC_PLATFORM_WIN32)
	std::wstring w_path = Encoding::Utf8ToWChar(full_path.c_str(), full_path.Size());
	bool result = ::DeleteFileW(w_path.c_str()) == TRUE;
#elif defined(DC_PLATFORM_LINUX) || defined(DC_PLATFORM_ANDROID) || defined(DC_PLATFORM_MAC) || defined(DC_PLATFORM_IOS)
	bool result = ::remove(full_path.c_str()) == 0;
#else
	bool result = false;
#endif
#if DC_DEBUG
	if(!result)Debuger::Error("delete file error:%s", full_path.c_str());
#endif
	return result;
}
bool File::Exist(const String& full_path)
{
	DC_PROFILE_FUNCTION();
	if (full_path.IsEmpty())return false;

#if defined(DC_PLATFORM_WIN32)
	std::wstring w_path = Encoding::Utf8ToWChar(full_path.c_str(), full_path.Size());
	return ::GetFileAttributesW(w_path.c_str()) != INVALID_FILE_ATTRIBUTES;
#elif defined(DC_PLATFORM_LINUX) || defined(DC_PLATFORM_ANDROID) || defined(DC_PLATFORM_MAC) || defined(DC_PLATFORM_IOS)
	return ::access(full_path.c_str(), F_OK) == 0;
#else
	return false;
#endif
}
bool File::Copy(const String& src_path, const String& dst_path)
{
	DC_PROFILE_FUNCTION();
	if (src_path.IsEmpty() || dst_path.IsEmpty())return false;

	String path = Path::GetDirectoryName(dst_path);
	if (!Directory::Exist(path))
		Directory::Create(path);

	if (File::Exist(dst_path))
		File::Delete(dst_path);

#if defined(DC_PLATFORM_WIN32)
	std::wstring w_src_path = Encoding::Utf8ToWChar(src_path.c_str(), src_path.Size());
	std::wstring w_dst_path = Encoding::Utf8ToWChar(dst_path.c_str(), dst_path.Size());
	bool result = ::CopyFileW(w_src_path.c_str(), w_dst_path.c_str(), false) == TRUE;
#elif defined(DC_PLATFORM_LINUX) || defined(DC_PLATFORM_ANDROID) || defined(DC_PLATFORM_MAC) || defined(DC_PLATFORM_IOS)
	FILE *in = fopen(src_path.c_str(), "r+");
	FILE *out = fopen(dst_path.c_str(), "w+");
	bool result = false;
	if (in && out)
	{
		char buff[1024];
		int len = 0;
		while (len = fread(buff, 1, sizeof(buff), in))
		{
			fwrite(buff, 1, len, out);
		}
		fflush(out);
		result = true;
	}
	if(in)fclose(in);
	if(out)fclose(out);
#else
	bool result = false;
#endif

#if DC_DEBUG
	if (!result)Debuger::Error("copy file error:%s", src_path.c_str());
#endif
	return result;
}
bool File::Move(const String& src_path, const String& dst_path)
{
	DC_PROFILE_FUNCTION();
	if (src_path.IsEmpty() || dst_path.IsEmpty())return false;

	String path = Path::GetDirectoryName(dst_path);
	if (!Directory::Exist(path))
		Directory::Create(path);

	if (File::Exist(dst_path))
		File::Delete(dst_path);

#if defined(DC_PLATFORM_WIN32)
	std::wstring w_src_path = Encoding::Utf8ToWChar(src_path.c_str(), src_path.Size());
	std::wstring w_dst_path = Encoding::Utf8ToWChar(dst_path.c_str(), dst_path.Size());
	//如果源位置和目标位置都在同一卷上，则将执行原子重命名操作.如果它们位于不同的卷上，那么将执行复制/删除操作
	bool result = MoveFileW(w_src_path.c_str(), w_dst_path.c_str()) == TRUE;
#elif defined(DC_PLATFORM_LINUX) || defined(DC_PLATFORM_ANDROID) || defined(DC_PLATFORM_MAC) || defined(DC_PLATFORM_IOS)
	bool result = ::rename(src_path.c_str(), dst_path.c_str()) == 0;
#else
	bool result = false;
#endif

#if DC_DEBUG
	if (!result)Debuger::Error("move file error:%s", src_path.c_str());
#endif
	return result;
}
bool File::Rename(const String& src_path, const String& new_name)
{
	DC_PROFILE_FUNCTION();
	if (!File::Exist(src_path))
		return false;

	String path = Path::GetDirectoryName(src_path);
	String dst_path = Path::Combine(path, new_name);
	if (Move(src_path, dst_path))
	{
		File::Delete(src_path);
		return true;
	}
	return false;
}
bool File::AppendAllText(const String& full_path, const String& contents, EncodingType encoding)
{
	DC_PROFILE_FUNCTION();
	if (full_path.IsEmpty())return false;

	String path = Path::GetDirectoryName(full_path);
	if (!Directory::Exist(path))
		Directory::Create(path);

	FileDataStream file(full_path, "a+");
	if (!file.IsOpen())
	{
		return false;
	}
	String content = contents;
	if (file.Write(const_cast<char*>(content.c_str()), 0, content.Size()) != content.Size())
	{
#if DC_DEBUG
		Debuger::Error("append file error:%s", full_path.c_str());
#endif
		return false;
	}

#if DC_DEBUG
	Debuger::Log("append file:%s", full_path.c_str());
#endif

	return true;
}
bool File::AppendAllText(const String& full_path, const String& contents)
{
	return AppendAllText(full_path, contents, EncodingType::Default);
}
bool File::ReadAllBytes(const String& full_path, MemoryDataStream& stream)
{
	DC_PROFILE_FUNCTION();
	if (full_path.IsEmpty() || !File::Exist(full_path))
		return false;

	FileDataStream file(full_path, "rb+");
	if (!file.IsOpen())
	{
		return false;
	}

	int size = file.Size();
	byte* buf = NewArray<byte>(size);
	if (file.Read(buf, size) != size)
	{
#if DC_DEBUG
		Debuger::Error("read file error:%s", full_path.c_str());
#endif
		DeleteArray(buf);
		return false;
	}
	stream.Write(buf, 0, size);
	DeleteArray(buf);
	return true;
}
String File::ReadAllText(const String& full_path, EncodingType encoding)
{
	DC_PROFILE_FUNCTION();
	if (full_path.IsEmpty() || !File::Exist(full_path))
	{
#if DC_DEBUG
		Debuger::Error("file not exist:%s", full_path.c_str());
#endif
		return "";
	}

	FileDataStream file(full_path, "rb+");
	if (!file.IsOpen())
	{
		return "";
	}

	int size = file.Size();
	char* buf = NewArray<char>(size + 1);
	if (file.Read(buf, size) != size)
	{
#if DC_DEBUG
		Debuger::Error("read file error:%s", full_path.c_str());
#endif
		DeleteArray(buf);
		return "";
	}
	buf[size] = '\0';

#if DC_DEBUG
	//Debuger::Log("read file:%s", full_path.c_str());
#endif

	//String str = Encoding::Convert(encoding, EncodingType::Default, buf, size);
	String str = buf;
	DeleteArray(buf);
	return str;
}
String File::ReadAllText(const String& full_path)
{
	return ReadAllText(full_path, EncodingType::Default);
}
bool File::WriteAllBytes(const String& full_path, byte* bytes, int count)
{
	DC_PROFILE_FUNCTION();
	if (full_path.IsEmpty())return false;

	String path = Path::GetDirectoryName(full_path);
	if (!Directory::Exist(path))
		Directory::Create(path);

	if (count < 0)return false;
	FileDataStream file(full_path, "wb");
	if (!file.IsOpen())
	{
		return false;
	}
	if (file.Write(bytes, 0, count) != count)
	{
#if DC_DEBUG
		Debuger::Error("write file error:%s", full_path.c_str());
#endif
		return false;
	}
	return true;
}
bool File::WriteAllText(const String& full_path, const String& contents)
{
	return WriteAllText(full_path, contents, EncodingType::Default);
}
bool File::WriteAllText(const String& full_path, const String& contents, EncodingType encoding)
{
	DC_PROFILE_FUNCTION();
	if (full_path.IsEmpty())return false;

	String path = Path::GetDirectoryName(full_path);
	if (!Directory::Exist(path))
		Directory::Create(path);

	FileDataStream file(full_path, "w+");
	if (!file.IsOpen())
	{
		return false;
	}
	//String content = Encoding::Convert(EncodingType::Default, encoding, contents);
	String content = contents;
	if (file.Write(const_cast<char*>(content.c_str()), 0, content.Size()) != content.Size())
	{
#if DC_DEBUG
		Debuger::Error("write file error:%s", full_path.c_str());
#endif
		return false;
	}

#if DC_DEBUG
	Debuger::Log("write file:%s", full_path.c_str());
#endif

	return true;
}

static bool unzipCurrentFile(unzFile uf, const String& dest_path)
{
	char szFilePath[512] = {0};
	unz_file_info64 file_info;
	if (unzGetCurrentFileInfo64(uf, &file_info, szFilePath, sizeof(szFilePath), NULL, 0, NULL, 0) != UNZ_OK)
		return false;

	size_t len = strlen(szFilePath);
	if (len <= 0)
	{
		return false;
	}

	String file_path = dest_path + "/" + Path::ReplaceSplit(szFilePath);
	if (szFilePath[len - 1] == '\\' || szFilePath[len - 1] == '/')
	{
		Directory::Create(file_path);
		return true;
	}

	auto file = fopen(file_path.c_str(), "wb");
	if (file == nullptr)
	{
		return false;
	}

	const int BUFFER_SIZE = 4096;
	unsigned char byBuffer[BUFFER_SIZE];
	if (unzOpenCurrentFile(uf) != UNZ_OK)
	{
		fclose(file);
		return false;
	}

	while (true)
	{
		int nSize = unzReadCurrentFile(uf, byBuffer, BUFFER_SIZE);
		if (nSize < 0)
		{
			unzCloseCurrentFile(uf);
			fclose(file);
			return false;
		}
		else if (nSize == 0)
		{
			break;
		}
		else
		{
			size_t wSize = fwrite(byBuffer, 1, nSize, file);
			if (wSize != nSize)
			{
				unzCloseCurrentFile(uf);
				fclose(file);
				return false;
			}
		}
	}

	unzCloseCurrentFile(uf);
	fclose(file);
	return true;
}
bool File::Unzip(const String& zip_file, const String& dest_path)
{
	unzFile uf = unzOpen64(zip_file.c_str());
	if (uf == NULL)
	{
		Debuger::Error("open zip file error:%s", zip_file.c_str());
		return false;
	}

	unz_global_info64 gi;
	if (unzGetGlobalInfo64(uf, &gi) != UNZ_OK)
	{
		Debuger::Error("init zip file error:%s", zip_file.c_str());
		unzClose(uf);
		return false;
	}

	bool result = true;
	for (int i = 0; i < gi.number_entry; ++i)
	{
		if (!unzipCurrentFile(uf, dest_path.c_str()))
		{
			unzClose(uf);
			result = false;
			break;
		}
		if (i < gi.number_entry - 1)
		{
			if (unzGoToNextFile(uf) != UNZ_OK)
			{
				unzClose(uf);
				result = false;
				break;
			}
		}
	}
	unzClose(uf);

	if (!result)
		Debuger::Error("unzip file error:%s", zip_file.c_str());
	return result;
}
DC_END_NAMESPACE
