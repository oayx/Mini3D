#include "File.h" 
#include "Path.h"
#include "Directory.h"
#include "core/Debuger.h"
#include "core/stream/DataStream.h"
#include "platform/PlatformDefine.h"
#include <external/zlib/unzip.h>
#include <filesystem>
#if defined(DC_PLATFORM_WIN32)
#include <io.h>
#else
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
		Directory::GetFileInfos(_filePath, op, _childrens);
	}
}
FileInfo::FileInfo(const FileInfo& other)
	: FileName(other.FileName),
	Extension(other.Extension),
	FullName(other.FullName),
	FileType(other.FileType),
	_filePath(other._filePath),
	_hasChildrenFolder(other._hasChildrenFolder),
	_childrens(other._childrens)
{
}
FileInfo::FileInfo(FileInfo&& other)noexcept
	: FileName(std::move(other.FileName)),
	Extension(std::move(other.Extension)),
	FullName(std::move(other.FullName)),
	FileType(other.FileType),
	_filePath(std::move(other._filePath)),
	_hasChildrenFolder(other._hasChildrenFolder),
	_childrens(std::move(other._childrens))
{
	// 清空被移动对象的数据
	other._hasChildrenFolder = false;
	other._childrens.Clear();
}
FileInfo& FileInfo::operator =(const FileInfo& other)
{
	if (this != &other)
	{
		FileName = other.FileName;
		Extension = other.Extension;
		FullName = other.FullName;
		FileType = other.FileType;
		_filePath = other._filePath;
		_hasChildrenFolder = other._hasChildrenFolder;
		_childrens = other._childrens;
	}
	return *this;
}
FileInfo& FileInfo::operator =(FileInfo&& other)noexcept
{
	if (this != &other)
	{
		FileName = std::move(other.FileName);
		Extension = std::move(other.Extension);
		FullName = std::move(other.FullName);
		FileType = other.FileType;
		_filePath = std::move(other._filePath);
		_hasChildrenFolder = other._hasChildrenFolder;
		_childrens = std::move(other._childrens);

		// 清空被移动对象的数据
		other._hasChildrenFolder = false;
		other._childrens.Clear();
	}
	return *this;
}
bool FileInfo::Exist()const
{
	return File::Exist(_filePath);
}
bool FileInfo::HasChildren(const String& name)
{
	for (int i = 0; i < _childrens.Size(); ++i)
	{
		const FileInfo& fileInfo = _childrens[i];
		if (fileInfo.FileName.Equals(name, true))
			return true;
	}
	return false;
}
/********************************************************************/
IMPL_REFECTION_TYPE(File);
bool File::Create(const String& fullPath)
{
	DC_PROFILE_FUNCTION;
	if (File::Exist(fullPath))return false;

	String path = Path::GetDirectoryName(fullPath);
	if (!path.IsEmpty() && !Directory::Exist(path))
		Directory::Create(path);

	FileDataStream file(fullPath, "w");
	return file.IsOpen();
}
bool File::Delete(const String& fullPath)
{
	std::error_code ec;
	std::filesystem::remove(fullPath.c_str());
	if (ec)
	{
#if DC_DEBUG
		Debuger::Error("delete file:%s -> %s error:%s", fullPath.c_str(), ec.message().c_str());
#endif
		return false;
	}
	return true;
}
bool File::Exist(const String& fullPath)
{
	DC_PROFILE_FUNCTION;
	if (fullPath.IsEmpty())return false;
	return std::filesystem::exists(fullPath.c_str());
}
bool File::Copy(const String& srcPath, const String& dstPath)
{
	DC_PROFILE_FUNCTION;
	if (!Exist(srcPath))
		return false;

	std::error_code ec;
	std::filesystem::copy_file(srcPath.c_str(), dstPath.c_str(), std::filesystem::copy_options::overwrite_existing, ec);
	if (ec)
	{
#if DC_DEBUG
		Debuger::Error("copy file:%s -> %s error:%s", srcPath.c_str(), dstPath.c_str(), ec.message().c_str());
#endif
		return false;
	}
	return true;
}
bool File::Move(const String& srcPath, const String& dstPath)
{
	DC_PROFILE_FUNCTION;
	if (!Exist(srcPath))
		return false;

	std::error_code ec;
	std::filesystem::rename(srcPath.c_str(), dstPath.c_str(), ec);
	if (ec)
	{
#if DC_DEBUG
		Debuger::Error("move file:%s -> %s error:%s", srcPath.c_str(), dstPath.c_str(), ec.message().c_str());
#endif
		return false;
	}
	return true;
}
bool File::Rename(const String& srcPath, const String& new_name)
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
		Debuger::Error("rename file:%s -> %s error:%s", srcPath.c_str(), dstPath.c_str(), ec.message().c_str());
#endif
		return false;
	}
	return true;
}
bool File::AppendAllText(const String& fullPath, const String& contents, EncodingType encoding)
{
	DC_PROFILE_FUNCTION;
	if (fullPath.IsEmpty())return false;

	String path = Path::GetDirectoryName(fullPath);
	if (!Directory::Exist(path))
		Directory::Create(path);

	FileDataStream file(fullPath, "a+");
	if (!file.IsOpen())
	{
		return false;
	}
	String content = contents;
	if (file.Write(const_cast<char*>(content.c_str()), 0, content.Size()) != content.Size())
	{
#if DC_DEBUG
		Debuger::Error("append file error:%s", fullPath.c_str());
#endif
		return false;
	}

#if DC_DEBUG
	Debuger::Log("append file:%s", fullPath.c_str());
#endif

	return true;
}
bool File::AppendAllText(const String& fullPath, const String& contents)
{
	return AppendAllText(fullPath, contents, EncodingType::Default);
}
bool File::ReadAllBytes(const String& fullPath, MemoryDataStream& stream)
{
	DC_PROFILE_FUNCTION;
	if (fullPath.IsEmpty() || !File::Exist(fullPath))
		return false;

	FileDataStream file(fullPath, "rb+");
	if (!file.IsOpen())
	{
		return false;
	}

	int size = file.Size();
	byte* buf = Memory::NewArray<byte>(size);
	if (file.Read(buf, size) != size)
	{
#if DC_DEBUG
		Debuger::Error("read file error:%s", fullPath.c_str());
#endif
		Memory::DeleteArray(buf);
		return false;
	}

	// 检查并移除UTF-8 BOM
	bool hasBom = false;
	if (size >= 3 &&
		static_cast<unsigned char>(buf[0]) == 0xEF &&
		static_cast<unsigned char>(buf[1]) == 0xBB &&
		static_cast<unsigned char>(buf[2]) == 0xBF) {
		hasBom = true;
	}

	stream.Write(buf, hasBom ? 3 : 0, hasBom ? size - 3 : size);
	Memory::DeleteArray(buf);
	return true;
}
String File::ReadAllText(const String& fullPath, EncodingType encoding)
{
	DC_PROFILE_FUNCTION;
	if (fullPath.IsEmpty() || !File::Exist(fullPath))
	{
#if DC_DEBUG
		Debuger::Error("file not exist:%s", fullPath.c_str());
#endif
		return "";
	}

	FileDataStream file(fullPath, "rb+");
	if (!file.IsOpen())
	{
		return "";
	}

	int size = file.Size();
	char* buf = Memory::NewArray<char>(size + 1);
	if (file.Read(buf, size) != size)
	{
#if DC_DEBUG
		Debuger::Error("read file error:%s", fullPath.c_str());
#endif
		Memory::DeleteArray(buf);
		return "";
	}

	buf[size] = '\0';

#if DC_DEBUG
	//Debuger::Log("read file:%s", fullPath.c_str());
#endif

	//String str = Encoding::Convert(encoding, EncodingType::Default, buf, size);
	String str = buf;
	Memory::DeleteArray(buf);

	// 检查并移除UTF-8 BOM
	if (str.Size() >= 3 &&
		static_cast<unsigned char>(str[0]) == 0xEF &&
		static_cast<unsigned char>(str[1]) == 0xBB &&
		static_cast<unsigned char>(str[2]) == 0xBF) {
		str.Remove(0, 3);
	}

	return str;
}
String File::ReadAllText(const String& fullPath)
{
	return ReadAllText(fullPath, EncodingType::Default);
}
bool File::WriteAllBytes(const String& fullPath, byte* bytes, int count)
{
	DC_PROFILE_FUNCTION;
	if (fullPath.IsEmpty())return false;

	String path = Path::GetDirectoryName(fullPath);
	if (!Directory::Exist(path))
		Directory::Create(path);

	if (count < 0)return false;
	FileDataStream file(fullPath, "wb");
	if (!file.IsOpen())
	{
		return false;
	}
	if (file.Write(bytes, 0, count) != count)
	{
#if DC_DEBUG
		Debuger::Error("write file error:%s", fullPath.c_str());
#endif
		return false;
	}
	return true;
}
bool File::WriteAllText(const String& fullPath, const String& contents)
{
	return WriteAllText(fullPath, contents, EncodingType::Default);
}
bool File::WriteAllText(const String& fullPath, const String& contents, EncodingType encoding)
{
	DC_PROFILE_FUNCTION;
	if (fullPath.IsEmpty())return false;

	String path = Path::GetDirectoryName(fullPath);
	if (!Directory::Exist(path))
		Directory::Create(path);

	FileDataStream file(fullPath, "w+");
	if (!file.IsOpen())
	{
		return false;
	}
	//String content = Encoding::Convert(EncodingType::Default, encoding, contents);
	String content = contents;
	if (file.Write(const_cast<char*>(content.c_str()), 0, content.Size()) != content.Size())
	{
#if DC_DEBUG
		Debuger::Error("write file error:%s", fullPath.c_str());
#endif
		return false;
	}

	return true;
}

static bool unzipCurrentFile(unzFile uf, const String& destPath)
{
	char szFilePath[512] = {0};
	unz_file_info64 fileInfo;
	if (unzGetCurrentFileInfo64(uf, &fileInfo, szFilePath, sizeof(szFilePath), NULL, 0, NULL, 0) != UNZ_OK)
		return false;

	size_t len = strlen(szFilePath);
	if (len <= 0)
	{
		return false;
	}

	String filePath = destPath + "/" + Path::ReplaceSplit(szFilePath);
	if (szFilePath[len - 1] == '\\' || szFilePath[len - 1] == '/')
	{
		Directory::Create(filePath);
		return true;
	}

	auto file = fopen(filePath.c_str(), "wb");
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
bool File::Unzip(const String& zipFile, const String& destPath)
{
	unzFile uf = unzOpen64(zipFile.c_str());
	if (uf == NULL)
	{
		Debuger::Error("open zip file error:%s", zipFile.c_str());
		return false;
	}

	unz_global_info64 gi;
	if (unzGetGlobalInfo64(uf, &gi) != UNZ_OK)
	{
		Debuger::Error("init zip file error:%s", zipFile.c_str());
		unzClose(uf);
		return false;
	}

	bool result = true;
	for (int i = 0; i < gi.number_entry; ++i)
	{
		if (!unzipCurrentFile(uf, destPath.c_str()))
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
		Debuger::Error("unzip file error:%s", zipFile.c_str());
	return result;
}
DC_END_NAMESPACE
