 
/*****************************************************************************
* Date：2020/3/18
* Author： hannibal
* Description：路径操作
*****************************************************************************/
#pragma once

#include "core/String.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
// 路径操作
class ENGINE_DLL Path Final : public object
{
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(Path);
	BEGIN_REFECTION_TYPE(Path)
	END_FINAL_REFECTION_TYPE;
	
public:
	//当前工作目录
	static bool SetCurrentProcessDirectory(const String& path);
	//当前工作目录
	static String GetCurrentProcessDirectory();

	//合并两个路径字符串
	static String Combine(const String& path1, const String& path2);
	//返回指定路径字符串的目录信息
	static String GetDirectoryName(const String& path);
	//返回文件夹名称
	static String GetFolderName(const String& path);
	//返回指定的路径字符串的扩展名
	static String GetExtension(const String& path);
	//替换扩展名
	static String ReplaceExtension(const String& path, const String& new_ext);
	//返回指定路径字符串的文件名和扩展名
	static String GetFileName(const String& path);
	//返回不具有扩展名的指定路径字符串的文件名
	static String GetFileNameWithoutExtension(const String& path);
	//返回指定路径字符串的绝对路径
	static String GetFullDataPath(const String& path);
	//获取指定路径的根目录信息
	static String GetPathRoot(const String& path);
	//确定路径是否包括文件扩展名
	static bool HasExtension(const String& path);
	//是否目录
	static bool IsFolder(const String& path);
	// \\ 替换成 /
	static String ReplaceSplit(const String& path);
};//Path

inline void ConvertSeparatorsToGNUC(char* pathName)
{
	while (*pathName != '\0') 
	{
		if (*pathName == '\\')
			*pathName = '/';
		++pathName;
	}
}
inline void ConvertSeparatorsToWindows(wchar_t* pathName)
{
	while (*pathName != L'\0') 
	{
		if (*pathName == L'/')
			*pathName = L'\\';
		++pathName;
	}
}
inline void ConvertSeparatorsToWindows(std::wstring& pathName)
{
	for (int i = 0; i < pathName.length(); ++i)
	{
		if (pathName[i] == L'/')
			pathName[i] = L'\\';
	}
}
inline void ConvertSeparatorsToWindows(char* pathName)
{
	while (*pathName != '\0')
	{
		if (*pathName == '/')
			*pathName = '\\';
		++pathName;
	}
}
inline void ConvertSeparatorsToWindows(std::string& pathName)
{
	for(int i = 0; i < pathName.length(); ++i)
	{
		if (pathName[i] == '/')
			pathName[i] = '\\';
	}
}
DC_END_NAMESPACE

/*
/bin 二进制可执行命令
/dev 设备特殊文件
/etc 系统管理和配置文件
/etc/rc.d 启动的配置文件和脚本
/home 用户主目录的基点，比如用户user的主目录就是/home/user，可以用~user表示
/lib 标准程序设计库，又叫动态链接共享库，作用类似windows里的.dll文件
/sbin 超级管理命令，这里存放的是系统管理员使用的管理程序
/tmp 公共的临时文件存储点
/root 系统管理员的主目录
/mnt 系统提供这个目录是让用户临时挂载其他的文件系统
/lost+found这个目录平时是空的，系统非正常关机而留下“无家可归”的文件（windows下叫什么.chk）就在这里
/proc 虚拟的目录，是系统内存的映射。可直接访问这个目录来获取系统信息。
/var 某些大文件的溢出区，比方说各种服务的日志文件

/usr 最庞大的目录，要用到的应用程序和文件几乎都在这个目录，其中包含：
/usr/x11R6 存放x window的目录
/usr/bin 众多的应用程序
/usr/sbin 超级用户的一些管理程序
/usr/doc linux文档
/usr/include linux下开发和编译应用程序所需要的头文件
/usr/lib 常用的动态链接库和软件包的配置文件
/usr/man 帮助文档
/usr/src 源代码，linux内核的源代码就放在/usr/src/linux里
/usr/local/bin 本地增加的命令
/usr/local/lib 本地增加的库根文件系统
*/