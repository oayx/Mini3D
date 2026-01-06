#pragma once

#include "TestBase.h"

DC_BEGIN_NAMESPACE
class TestFile : public TestBase
{
	typedef TestBase base;

#if defined(DC_PLATFORM_WIN32)
	String root_path = "c:/1/";
#else
	String root_path = "sdcard/backup/tmp/";
#endif

public:
	TestFile()
	{
		std::function<void()> fun = CALLBACK_0(TestFile::TestFiles, this);
		_menus.Add(std::make_pair("File", fun));

		fun = CALLBACK_0(TestFile::TestDirectory, this);
		_menus.Add(std::make_pair("Directory", fun));

		fun = CALLBACK_0(TestFile::TestPath, this);
		_menus.Add(std::make_pair("Path", fun));
	}
	virtual void OnEnter(int child_unit_index)override
	{
		base::OnEnter(child_unit_index);
	}
	virtual void OnLeave()override
	{
		base::OnLeave();
	}
	virtual String GetTitle()override { return "File"; }

public:
	void TestFiles()
	{
		if (Directory::Exist(root_path))
			Directory::Delete(root_path);

		bool b = File::Exist(root_path + "aa.txt");
		MyAssert(!b);
		b = File::Create(root_path + "aa.txt");
		MyAssert(b);
		b = File::Exist(root_path + "aa.txt");
		MyAssert(b);
		b = File::Delete(root_path + "aa.txt");
		MyAssert(b);
		b = File::Exist(root_path + "aa.txt");
		MyAssert(!b);

		b = File::WriteAllText(root_path + "bb.txt", "asdf");
		MyAssert(b);
		String str = File::ReadAllText(root_path + "bb.txt");
		MyAssert(str == "asdf");

		byte cc[] = "ddffdd";
		b = File::WriteAllBytes(root_path + "cc.txt", cc, sizeof(cc) - 1);
		MyAssert(b);
		Debuger::Log("File::WriteAllBytes cc.txt -> ddffdd");

		Debuger::Log("File::ReadAllBytes cc.txt");
		byte* buf = nullptr;
		MemoryDataStream stream;
		if(File::ReadAllBytes(root_path + "cc.txt", stream))
		{
			for (long i = 0; i < stream.Size(); ++i)
			{
				byte b = stream[i];
				printf("%c", b);
			}
		}
		Debuger::Log("\nFile::ReadAllBytes cc.txt end");

		b = File::Copy(root_path + "cc.txt", root_path + "copy.txt");
		MyAssert(b);
		b = File::Move(root_path + "cc.txt", root_path + "move.txt");
		MyAssert(b);

		//中文
		b = File::Create(root_path + "中文.txt");
		MyAssert(b);
		b = File::Exist(root_path + "中文.txt");
		MyAssert(b);
		b = File::Delete(root_path + "中文.txt");
		MyAssert(b);
		b = File::Exist(root_path + "中文.txt");
		MyAssert(!b);

		//重命名
		b = File::Create(root_path + "中文.txt");
		MyAssert(b);
		b = File::Rename(root_path + "中文.txt", "新中文.txt");
		MyAssert(b);
	}

	void TestDirectory()
	{
		if (Directory::Exist(root_path))
			Directory::Delete(root_path);

		//创建+删除目录
		bool b = Directory::Exist(root_path + "1/2");
		MyAssert(!b);
		b = Directory::Create(root_path + "1/2");
		MyAssert(b);
		b = Directory::Exist(root_path + "1/2");
		MyAssert(b);
		b = Directory::IsDirectory(root_path + "1/2");
		MyAssert(b);

		b = File::Create(root_path + "1/2/3.txt");
		MyAssert(b);
		b = File::WriteAllText(root_path + "1/2/3.txt", "1234");
		MyAssert(b);
		b = File::Create(root_path + "1/2/3.txt");
		MyAssert(!b);
		b = File::Create(root_path + "1/2/3/4/5/6/7.txt");
		MyAssert(b);

		//获得目录和文件列表
		VecString vec;
		Directory::GetDirectorys(root_path + "1", SearchOption::AllDirectories, vec);
		for (auto& p : vec)
		{
			Debuger::Log("Directory::GetDirectorys - %s", p.c_str());
		}
		Directory::GetFiles(root_path + "1", SearchOption::AllDirectories, vec);
		for (auto& p : vec)
		{
			Debuger::Log("Directory::GetFiles - %s", p.c_str());
		}

		//删除
		b = Directory::Delete(root_path + "1/2");
		MyAssert(b);
		b = Directory::Exist(root_path + "1/2");
		MyAssert(!b);
	}

	void TestPath()
	{
		if (Directory::Exist(root_path))
			Directory::Delete(root_path);

		//当前目录
		String cur_path = Path::GetCurrentProcessDirectory();
		Debuger::Log("Path::GetCurrentDirectory - %s", cur_path.c_str());

		bool b = Directory::Create(root_path + "1/2");
		MyAssert(b);
		b = Path::SetCurrentProcessDirectory(root_path + "1/2");
		MyAssert(b);

		String new_path = Path::GetCurrentProcessDirectory();
		MyAssert(new_path.Replace("\\", "/") == root_path + "1/2");

		//还原
		b = Path::SetCurrentProcessDirectory(cur_path);
		MyAssert(b);

		//取完整路径
		String fullpath = Path::GetFullDataPath("App.config");  //c:\\users\\qqq\\source\\repos\\StudyDesignMode\\StudyDesignMode\\bin\\Debug\\App.config
		Debuger::Log("Path::GetFullDataPath - %s", fullpath.c_str());
		//参数为空字符串
		String fullpath2 = Path::GetFullDataPath("");  //Exception
		Debuger::Log("Path::GetFullDataPath - %s", fullpath2.c_str());

		//取所在磁盘
		String root = Path::GetPathRoot("D:\\1\\2\\App.config"); //D:/
		Debuger::Log("Path::GetPathRoot - %s", root.c_str());

		//取文件夹
		String folder = Path::GetFolderName("D:\\1\\2\\App.config"); //D:/
		MyAssert(folder == "2")
			
		//取文件名
		String name = Path::GetFileName("D:\\1\\2\\App.config"); //App.config
		MyAssert(name == "App.config")
		//无扩展名
		name = Path::GetFileName("D:\\1\\2\\App"); //App 
		MyAssert(name == "App")
		//只有扩展名
		name = Path::GetFileName("D:\\1\\2\\.config"); //.config
		MyAssert(name == ".config")
		//无文件名
		name = Path::GetFileName("D:\\1\\2\\");
		MyAssert(name == "")
		//只有盘符
		name = Path::GetFileName("D:"); //  ""
		MyAssert(name == "")
		//参数为""
		name = Path::GetFileName(""); // ""
		MyAssert(name == "")

		//取文件名 不带扩展名
		String nameWithoutExtension = Path::GetFileNameWithoutExtension("D:\\1\\2\\App.config"); //App
		MyAssert(nameWithoutExtension == "App")

		//取扩展名
		String extension = Path::GetExtension("D:\\1\\2\\App.config"); //.config
		MyAssert(extension == ".config")
		//无扩展名
		extension = Path::GetExtension("D:\\1\\2\\App"); //""
		MyAssert(extension == "")
		//只有扩展名
		extension = Path::GetExtension("D:\\1\\2\\.config"); //.config
		MyAssert(extension == "")
		//无文件名
		extension = Path::GetExtension("D:\\1\\2\\");
		MyAssert(extension == "")
		//只有盘符
		extension = Path::GetExtension("D:"); //  ""
		MyAssert(extension == "")
		//参数为""
		extension = Path::GetExtension(""); // ""
		MyAssert(extension == "")

		//连接路径
		String combine = Path::Combine("1", "2"); // 1\\2
		Debuger::Log("Path::Combine - %s", combine.c_str());
		//参数为""
		combine = Path::Combine("", ""); // ""
		Debuger::Log("Path::Combine - %s", combine.c_str());

		//取所在文件夹
		String dir = Path::GetDirectoryName("D:/1/2/App.config"); //D:/1/2
		MyAssert(dir == "D:/1/2");
		dir = Path::GetDirectoryName("D:/1/2/"); //D:/1/2
		MyAssert(dir == "D:/1/2");
		dir = Path::GetDirectoryName(Path::Combine("D:/1", "Temp"));  //  D:/1
		MyAssert(dir == "D:/1");
		dir = Path::GetDirectoryName(Path::Combine("D:/1/", "Temp")); //  D:/1
		MyAssert(dir == "D:/1");

		//相对路径
		dir = Path::GetRelativePath("/a/b/c/d/e.txt","/a/b");
		MyAssert(dir.Replace("\\", "/") == "c/d/e.txt");

		//绝对路径
		dir = Path::GetAbsolutePath("/a/b/c/d/e.txt");
		Debuger::Log("Path::GetAbsolutePath - %s", dir.c_str());
	}
};
DC_END_NAMESPACE
