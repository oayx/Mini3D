 
/*****************************************************************************
* Author： hannibal
* Date：2020/3/18
* Description：文件操作
*****************************************************************************/
#pragma once

#include "Directory.h"
#include "core/Encoding.h"

DC_BEGIN_NAMESPACE
// 文件类型
enum class FileInfoType
{
	File = 0,
	Dir,
};
DECLARE_ENUM_OPERATORS(FileInfoType);

class MemoryDataStream;
/********************************************************************/
// 文件信息
class ENGINE_DLL FileInfo final : public object
{
	BEGIN_REFECTION_TYPE(FileInfo)
	END_FINAL_REFECTION_TYPE;
	
public:
	explicit FileInfo(const String& fileName);
	FileInfo(const String& fileName, SearchOption op);
	FileInfo(const FileInfo& other);
	FileInfo(FileInfo&& other)noexcept;
	FileInfo& operator =(const FileInfo& other);
	FileInfo& operator =(FileInfo&& other)noexcept;

	bool Exist()const;
	constexpr bool IsFolder()const { return FileType == FileInfoType::Dir; }

	int   GetChildrenCount()const { return _childrens.Size(); }
	const FileInfo& GetChildren(int i)const { AssertEx(i < _childrens.Size(), ""); return _childrens[i]; }
	const Vector<FileInfo>& Children()const { return _childrens; }
	bool  HasChildren(const String& name);

	void  DeleteChild(int i) { _childrens.RemoveAt(i); }

public:
	//获取文件名
	String FileName = "";
	// 获取表示文件扩展名部分的字符串
	String Extension = "";
	//获取目录或文件的完整目录
	String FullName = "";
	//类型
	FileInfoType FileType = FileInfoType::File;

private:
	String _filePath = "";
	bool   _hasChildrenFolder = false;
	Vector<FileInfo> _childrens;
};//File

/********************************************************************/
// 文件
class ENGINE_DLL File final : public object
{
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(File);
	BEGIN_REFECTION_TYPE(File)
	END_FINAL_REFECTION_TYPE;
	
public:
	//在指定路径中创建或覆盖文件。
	static bool Create(const String& fullPath);
	//删除指定的文件。如果指定的文件不存在，则不引发异常
	static bool Delete(const String& fullPath);
	//确定指定的文件是否存在。
	static bool Exist(const String& fullPath);

	//将现有文件复制到新文件。允许覆盖同名的文件
	static bool Copy(const String& srcPath, const String& dstPath);
	//将指定文件移到新位置，并提供指定新文件名的选项
	//如果将一个文件移动到一个已经存在的目标文件中，目标文件的内容将会被覆盖
	static bool Move(const String& srcPath, const String& dstPath);
	//重命名
	//new_name:新文件名，不用带目录
	static bool Rename(const String& srcPath, const String& new_name);

	//将指定的字符串追加到文件中，如果文件还不存在则创建该文件。(要追加到文件中的字符串，要求是utf8编码)
	static bool AppendAllText(const String& fullPath, const String& contents, EncodingType encoding);
	//打开一个文件，向其中追加指定的字符串，然后关闭该文件。如果文件不存在，此方法创建一个文件，将指定的字符串写入文件，然后关闭该文件。
	static bool AppendAllText(const String& fullPath, const String& contents);

	//打开一个文件，将文件的内容读入一个字符串，然后关闭该文件
	static bool ReadAllBytes(const String& fullPath, MemoryDataStream& stream);
	// 打开一个文件，使用指定的编码读取文件的所有行，然后关闭该文件
	static String ReadAllText(const String& fullPath, EncodingType encoding);
	//打开一个文本文件，读取文件的所有行，然后关闭该文件
	static String ReadAllText(const String& fullPath);

	//创建一个新文件，在其中写入指定的字节数组，然后关闭该文件。如果目标文件已存在，则覆盖该文件
	static bool WriteAllBytes(const String& fullPath, byte* bytes, int count);
	//创建一个新文件，在其中写入指定的字符串，然后关闭文件。如果目标文件已存在，则覆盖该文件(要写入文件的字符串，要求是utf8编码)
	static bool WriteAllText(const String& fullPath, const String& contents);
	//创建一个新文件，在其中写入指定的字符串，然后关闭文件。如果目标文件已存在，则覆盖该文件(要写入文件的字符串，要求是utf8编码)
	static bool WriteAllText(const String& fullPath, const String& contents, EncodingType encoding);

	//解压
	static bool Unzip(const String& zipFile, const String& destPath);
};//File
DC_END_NAMESPACE
