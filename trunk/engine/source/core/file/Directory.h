 
/*****************************************************************************
* Author： hannibal
* Date：2009/11/20
* Description： 文件夹
*****************************************************************************/
#pragma once

#include "core/String.h"
#include "core/stl/List.h"

DC_BEGIN_NAMESPACE
class FileInfo;
/********************************************************************/
// Description：指定是搜索当前目录，还是搜索当前目录及其所有子目录。
enum class SearchOption
{
	TopDirectoryOnly = 0,//仅在搜索中包括当前目录。
	AllDirectories = 1//在搜索操作中包括当前目录和所有子目录。此选项在搜索中包括重解析点，比如安装的驱动器和符号链接。
};
DECLARE_ENUM_OPERATORS(SearchOption);

/********************************************************************/
// 文件夹
class ENGINE_DLL Directory Final : public object
{
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(Directory);
	BEGIN_REFECTION_TYPE(Directory)
	END_FINAL_REFECTION_TYPE;
	
public:
	static bool Create(const String& path);
	static bool Delete(const String& path);

	static bool Exist(const String& path);
	static bool IsDirectory(const String& path);

	//如果det_path已存在，则det_path必须是一个目录，而且该目录应当是空目录（空目录指的是该目录中只有.和..项）
	//如果det_path存在（而且是一个空目录），则先将其删除，然后将src_path更名为det_path
	static bool Move(const String& src_path, const String& det_path);

	static void GetDirectorys(const String& path, SearchOption op, VecString& list);
	static void GetFiles(const String& path, SearchOption op, VecString& list);
	//默认递归获得所有子目录信息
	static void GetFiles(const String& path, SearchOption op, Vector<FileInfo>& list);
};//Directory
DC_END_NAMESPACE
