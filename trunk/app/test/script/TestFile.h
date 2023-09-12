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
		bool b = File::Exist(root_path + "aa.txt");
		Debuger::Log("File::Exist aa.txt- %s", b == false ? "true" : "false");
		b = File::Create(root_path + "aa.txt");
		Debuger::Log("File::Create aa.txt- %s", b == true ? "true" : "false");
		b = File::Exist(root_path + "aa.txt");
		Debuger::Log("File::Exist aa.txt- %s", b == true ? "true" : "false");
		b = File::Delete(root_path + "aa.txt");
		Debuger::Log("File::Delete aa.txt- %s", b == true ? "true" : "false");
		b = File::Exist(root_path + "aa.txt");
		Debuger::Log("File::Exist aa.txt- %s", b == false ? "true" : "false");

		String str = File::ReadAllText(root_path + "bb.txt");
		b = File::WriteAllText(root_path + "bb.txt", "asdf");
		Debuger::Log("File::WriteAllText bb.txt- %s", "asdf");
		str = File::ReadAllText(root_path + "bb.txt");
		Debuger::Log("File::ReadAllText bb.txt- %s", str.c_str());

		byte cc[] = "ddffdd";
		File::WriteAllBytes(root_path + "cc.txt", cc, sizeof(cc) - 1);
		Debuger::Log("File::WriteAllBytes cc.txt- %s", cc);

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
		Debuger::Log("File::Copy - %s", b == true ? "true" : "false");
		b = File::Move(root_path + "cc.txt", root_path + "move.txt");
		Debuger::Log("File::Move - %s", b == true ? "true" : "false");

		//中文
		b = File::Create(root_path + "中文.txt");
		Debuger::Log("File::Create 中文.txt- %s", b == true ? "true" : "false");
		b = File::Exist(root_path + "中文.txt");
		Debuger::Log("File::Exist 中文.txt- %s", b == true ? "true" : "false");
		b = File::Delete(root_path + "中文.txt");
		Debuger::Log("File::Delete 中文.txt- %s", b == true ? "true" : "false");
		b = File::Exist(root_path + "中文.txt");
		Debuger::Log("File::Exist 中文.txt- %s", b == false ? "true" : "false");
	}

	void TestDirectory()
	{
		//创建+删除目录
		bool b = Directory::Exist(root_path + "1/2");
		Debuger::Log("Directory::Exist - %s", b == false ? "true" : "false");
		b = Directory::Create(root_path + "1/2");
		Debuger::Log("Directory::Create - %s", b == true ? "true" : "false");
		b = Directory::Exist(root_path + "1/2");
		Debuger::Log("Directory::Exist - %s", b == true ? "true" : "false");
		b = Directory::IsDirectory(root_path + "1/2");
		Debuger::Log("Directory::IsDirectory - %s", b == true ? "true" : "false");

		b = File::Create(root_path + "1/2/3.txt");
		Debuger::Log("Directory::Create - %s", b == true ? "true" : "false");
		b = File::WriteAllText(root_path + "1/2/3.txt", "1234");
		Debuger::Log("Directory::WriteAllText - %s", b == true ? "true" : "false");
		b = File::Create(root_path + "1/2/3.txt");
		Debuger::Log("Directory::Create - %s", b == false ? "true" : "false");
		b = File::Create(root_path + "1/2/3/4/5/6/7.txt");
		Debuger::Log("Directory::Create - %s", b == true ? "true" : "false");

		//获得目录和文件列表
		VecString vec;
		Directory::GetDirectorys(root_path + "1", SearchOption::AllDirectories, vec);
		for (auto p : vec)
		{
			Debuger::Log("Directory::GetDirectorys - %s", p.c_str());
		}
		Directory::GetFiles(root_path + "1", SearchOption::AllDirectories, vec);
		for (auto p : vec)
		{
			Debuger::Log("Directory::GetFiles - %s", p.c_str());
		}

		//删除
		b = Directory::Delete(root_path + "1/2");
		Debuger::Log("Directory::Delete - %s", b == true ? "true" : "false");
		b = Directory::Exist(root_path + "1/2");
		Debuger::Log("Directory::Exist - %s", b == false ? "true" : "false");
	}

	void TestPath()
	{
		//当前目录
		String cur_path = Path::GetCurrentProcessDirectory();
		Debuger::Log("Path::GetCurrentDirectory - %s", cur_path.c_str());

		bool b = Directory::Create(root_path + "1/2");
		Debuger::Log("Directory::Create - %s", b == true ? "true" : "false");
		b = Path::SetCurrentProcessDirectory(root_path + "1/2");
		Debuger::Log("Directory::Exist - %s", b == true ? "true" : "false");

		String new_path = Path::GetCurrentProcessDirectory();
		Debuger::Log("Path::GetCurrentDirectory - %s", new_path.c_str());

		//还原
		Path::SetCurrentProcessDirectory(cur_path);

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
		Debuger::Log("Path::GetFolderName - %s", folder.c_str());
			
		//取文件名
		String name = Path::GetFileName("D:\\1\\2\\App.config"); //App.config
		Debuger::Log("Path::GetFileName - %s", name.c_str());
		//无扩展名
		name = Path::GetFileName("D:\\1\\2\\App"); //App 
		Debuger::Log("Path::GetFileName - %s", name.c_str());
		//只有扩展名
		name = Path::GetFileName("D:\\1\\2\\.config"); //.config
		Debuger::Log("Path::GetFileName - %s", name.c_str());
		//无文件名
		name = Path::GetFileName("D:\\1\\2\\");
		Debuger::Log("Path::GetFileName - %s", name.c_str());
		//只有盘符
		name = Path::GetFileName("D:"); //  ""
		Debuger::Log("Path::GetFileName - %s", name.c_str());
		//参数为""
		name = Path::GetFileName(""); // ""
		Debuger::Log("Path::GetFileName - %s", name.c_str());

		//取文件名 不带扩展名
		String nameWithoutExtension = Path::GetFileNameWithoutExtension("D:\\1\\2\\App.config"); //App
		Debuger::Log("Path::GetFileNameWithoutExtension - %s", nameWithoutExtension.c_str());

		//取扩展名
		String extension = Path::GetExtension("D:\\1\\2\\App.config"); //.config
		Debuger::Log("Path::GetExtension - %s", extension.c_str());
		//无扩展名
		extension = Path::GetExtension("D:\\1\\2\\App"); //""
		Debuger::Log("Path::GetExtension - %s", extension.c_str());
		//只有扩展名
		extension = Path::GetExtension("D:\\1\\2\\.config"); //.config
		Debuger::Log("Path::GetExtension - %s", extension.c_str());
		//无文件名
		extension = Path::GetExtension("D:\\1\\2\\");
		Debuger::Log("Path::GetExtension - %s", extension.c_str());
		//只有盘符
		extension = Path::GetExtension("D:"); //  ""
		Debuger::Log("Path::GetExtension - %s", extension.c_str());
		//参数为""
		extension = Path::GetExtension(""); // ""
		Debuger::Log("Path::GetExtension - %s", extension.c_str());

		//连接路径
		String combine = Path::Combine("1", "2"); // 1\\2
		Debuger::Log("Path::Combine - %s", combine.c_str());
		//参数为""
		combine = Path::Combine("", ""); // ""
		Debuger::Log("Path::Combine - %s", combine.c_str());

		//取所在文件夹
		String dir = Path::GetDirectoryName("D:\\1\\2\\App.config"); //D:\\1\\2
		Debuger::Log("Path::GetDirectoryName - %s", dir.c_str());
		dir = Path::GetDirectoryName("D:\\1\\2\\"); //D:\\1\\2
		Debuger::Log("Path::GetDirectoryName - %s", dir.c_str());
		dir = Path::GetDirectoryName(Path::Combine("D:\\1", "Temp"));  //  D:\\1
		Debuger::Log("Path::GetDirectoryName - %s", dir.c_str());
		dir = Path::GetDirectoryName(Path::Combine("D:\\1\\", "Temp")); //  D:\\1
		Debuger::Log("Path::GetDirectoryName - %s", dir.c_str());
	}
};
DC_END_NAMESPACE
