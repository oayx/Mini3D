#pragma once
 
#include "TestBase.h"
#include "TestTemplate.h"

DC_BEGIN_NAMESPACE

#pragma region 反射
class TestReflectClass : public Object
{
	BEGIN_DERIVED_REFECTION_TYPE(TestReflectClass, Object)
	END_REFECTION_TYPE;
public:
protected:
private:
	int cc = 10;
};
IMPL_DERIVED_REFECTION_TYPE(TestReflectClass, Object);
#pragma endregion

#pragma region 对齐
struct AlignTest1
{
	char c;
	int i;
	double d;
};
struct AlignTest2
{
	int i;
	double d;
};
struct AlignTest3
{
	int i;
	double d;
	char c;
};
#pragma endregion

class TestAutoPointerClass : public Object
{
public:
	TestAutoPointerClass()
	{
		Debuger::Log("TestAutoPointerClass::TestAutoPointerClass");
	}
	~TestAutoPointerClass()
	{
		Debuger::Log("TestAutoPointerClass::~TestAutoPointerClass");
	}
};

struct SetComp
{
	bool operator ()(int a, int b)
	{
		return a > b;
	}
};

class TestMetaObj : public Object
{
	BEGIN_DERIVED_REFECTION_TYPE(TestMetaObj, Object)
	END_REFECTION_TYPE;

public:
	UFUNCTION(void, Print, ())
	void Print()
	{
		Debuger::Log("Print id:%d, name:%s", _id, _name.c_str());
	}
	UFUNCTION(void, PrintInt, (int), EditAnywhere, RuntimeOnly=true)
	void PrintInt(int a)
	{
		Debuger::Log("PrintInt:%d", a);
	}

public:
	UFUNCTION(void, SetId, (int))
	void SetId(int id) { _id = id; }
	UFUNCTION(void, SetName, (std::string))
	void SetName(std::string name) { _name = name; }

	UFUNCTION(int, GetId, ())
	int GetId() { return _id; }
	UFUNCTION(std::string, GetName, ())
	std::string GetName() { return _name; }

private:
	UPROPERTY(int, _id, EditAnywhere, EditOnly, RuntimeAnywhere = false, RuntimeOnly)
	int _id;
	UPROPERTY(std::string, _name)
	std::string _name;
};

class TestCore : public TestBase
{
	typedef TestBase base;
public:
	TestCore()
	{
		//1.定时器
		std::function<void()> fun = CALLBACK_0(TestCore::TestTimer, this);
		_menus.Add(std::make_pair("Timer", fun));

		//2.事件
		fun = CALLBACK_0(TestCore::TestEvent, this);
		_menus.Add(std::make_pair("Event", fun));

		//3.线程
		fun = CALLBACK_0(TestCore::TestThread, this);
		_menus.Add(std::make_pair("Thread", fun));

		//4.日志
		fun = CALLBACK_0(TestCore::TestLog, this);
		_menus.Add(std::make_pair("Log", fun));

		//5.编码
		fun = CALLBACK_0(TestCore::TestEncoding, this);
		_menus.Add(std::make_pair("Encoding", fun));

		//6.时间
		fun = CALLBACK_0(TestCore::TestTime, this);
		_menus.Add(std::make_pair("Time", fun));

		//7.crypto
		fun = CALLBACK_0(TestCore::TestCrypto, this);
		_menus.Add(std::make_pair("Crypto", fun));

		//8.智能指针
		fun = CALLBACK_0(TestCore::TestAutoPointer, this);
		_menus.Add(std::make_pair("AutoPointer", fun));

		//9.可变类型
		fun = CALLBACK_0(TestCore::TestVariant, this);
		_menus.Add(std::make_pair("Variant", fun));

		//10.json
		fun = CALLBACK_0(TestCore::TestJson, this);
		_menus.Add(std::make_pair("Json", fun));

		//11.内存池
		fun = CALLBACK_0(TestCore::TestAllocPool, this);
		_menus.Add(std::make_pair("AllocPool", fun));

		//12.压缩
		fun = CALLBACK_0(TestCore::TestZip, this);
		_menus.Add(std::make_pair("Zip", fun));

		//13.反射
		fun = CALLBACK_0(TestCore::TestReflection, this);
		_menus.Add(std::make_pair("Reflection", fun));

		//14.结构体对齐
		fun = CALLBACK_0(TestCore::TestStructAlign, this);
		_menus.Add(std::make_pair("StructAlign", fun));

		//15.矩阵
		fun = CALLBACK_0(TestCore::TestMatrix4, this);
		_menus.Add(std::make_pair("Matrix4", fun));

		//16.stl
		fun = CALLBACK_0(TestCore::TestSTL, this);
		_menus.Add(std::make_pair("STL", fun));

		//17.算法
		fun = CALLBACK_0(TestCore::TestAlgorithm, this);
		_menus.Add(std::make_pair("Algorithm", fun));

		//18.zml
		fun = CALLBACK_0(TestCore::TestXML, this);
		_menus.Add(std::make_pair("XML", fun));

		//19.QRCode
		fun = CALLBACK_0(TestCore::TestQRCode, this);
		_menus.Add(std::make_pair("QRCode", fun));

		//20.delegate
		fun = CALLBACK_0(TestCore::TestDelegate, this);
		_menus.Add(std::make_pair("Delegate", fun));

		//21.template
		fun = CALLBACK_0(TestCore::TestTemplates, this);
		_menus.Add(std::make_pair("Template", fun));

		//22.数据结构
		fun = CALLBACK_0(TestCore::TestStruct, this);
		_menus.Add(std::make_pair("Struct", fun));

		//23.元编程
		fun = CALLBACK_0(TestCore::TestMeta, this);
		_menus.Add(std::make_pair("Meta", fun));
	}
	virtual void OnEnter(int child_unit_index)override
	{
		base::OnEnter(child_unit_index);
	}
	virtual void OnLeave()override
	{
		base::OnLeave();
	}
	virtual String GetTitle()override { return "Core"; }

private:
#pragma region 定时器
	void TestTimer()
	{
		Debuger::Log("----------------------TestTimer----------------------");

		Debuger::Log("Begin Once Timer:%f, curr time:%f", 5.0f, Time::GetRealTimeSinceStartup());
		uint timer_id = Timer::AddOnce(5.0f, CALLBACK_0(TestCore::OnTimerOnce, this));
		_timers.Add(timer_id);

		Debuger::Log("Begin Loop Timer:%f, curr time:%f", 1.0f, Time::GetRealTimeSinceStartup());
		timer_id = Timer::AddLoop(1.0f, CALLBACK_1(TestCore::OnTimerLoop, this), "123");
		_timers.Add(timer_id);
	}
	void OnTimerOnce()
	{
		Debuger::Log("OnceTimer, curr time:%f", Time::GetRealTimeSinceStartup());
	}
	void OnTimerLoop(String parm)
	{
		static int count = 0;
		Debuger::Log("LoopTimer:%d,, curr time:%f", ++count, Time::GetRealTimeSinceStartup());
	}
#pragma endregion

#pragma region 事件
	void TestEvent()
	{
		Debuger::Log("----------------------TestEvent----------------------");

		Debuger::Log("add event 1");
		EventDispatcher::AddEventListener("1", this, MakeEventHandler1(TestCore::OnHandleEvt));
		Debuger::Log("trigger event 1, args:%s", "1");
		EventDispatcher::TriggerEvent("1", "1");
		//测试派发不相干的消息
		Debuger::Log("trigger event 2, args:%s", "2");
		EventDispatcher::TriggerEvent("2", "2");
		//移除不相干的消息
		Debuger::Log("Remove event 2");
		EventDispatcher::RemoveEventListener("2", this, MakeEventHandler1(TestCore::OnHandleEvt));
		Debuger::Log("trigger event 1, args:%s", "3");
		EventDispatcher::TriggerEvent("1", "3");
		//移除消息
		Debuger::Log("Remove event 1");
		EventDispatcher::RemoveEventListener("1", this, MakeEventHandler1(TestCore::OnHandleEvt));
		Debuger::Log("trigger event 1, args:%s", "4");
		EventDispatcher::TriggerEvent("1", "4");
	}
	void OnHandleEvt(const String& arg1)
	{
		//测试派送事件时，删除同一个事件
		//EventDispatcher::RemoveEventListener(type, this, MakeEventHandler(TestCore::OnHandleEvt));
		Debuger::Log("recv event, args:%s", arg1.c_str());
	}
#pragma endregion

#pragma region 线程
	int thread_count = 0;
	std::mutex lock_obj;
	void TestThread()
	{
		Debuger::Log("----------------------TestThread----------------------");

		Debuger::Log("main thread id:%llu", Thread::CurrentThreadId());
		this->OnThreadSleep();

		//测试lock
		Debuger::Log("test two thread add, result should be 2000000");
		Task task1;
		task1.job = CALLBACK_0(TestCore::OnThreadLock1, this);
		task1.complete = [this]() 
		{
			Debuger::Log("task1 complete");
		};
		auto thread1 = Thread::Start(task1);
		if (thread1 == nullptr)
			return;

		Task task2;
		task2.job = CALLBACK_0(TestCore::OnThreadLock2, this);
		task2.complete = [this]()
		{
			Debuger::Log("task2 complete");
		};
		auto thread2 = Thread::Start(task2);
		if (thread2 == nullptr)
			return;

		thread1->Wait();
		thread2->Wait();
		MyAssert(thread_count == 2000000);

		//task
		Debuger::Log("test lambda task");
		Thread::Sleep(1000);
		Thread::Start([]()
		{
				Debuger::Log("task start id:%llu", Thread::CurrentThreadId());
		});

		//批量
		Debuger::Log("test threads");
		Thread::Sleep(1000);
		thread_count = 0;
		for (int i = 0; i < 100; ++i)
		{
			Thread::Start(task1);
			Thread::Start(task2);
		}
	}
	int OnThreadSleep()
	{
		Debuger::Log("sleep thread:%llu 3s, curr time:%f", Thread::CurrentThreadId(), Time::GetRealTimeSinceStartup());
		Thread::Sleep(3000);
		Debuger::Log("sleep thread end, curr time:%f", Time::GetRealTimeSinceStartup());
		return 0;
	}
	void OnThreadLock1()
	{
		Debuger::Log("thread1 start id:%llu", Thread::CurrentThreadId());
		for (int i = 0; i < 1000000; ++i)
		{
			LOCK(lock_obj);
			thread_count++;
		}
		Debuger::Log("thread1 end:%d", thread_count);
	}
	void OnThreadLock2()
	{
		Debuger::Log("thread2 start id:%llu", Thread::CurrentThreadId());
		for (int i = 0; i < 1000000; ++i)
		{
			LOCK(lock_obj);
			thread_count++;
		}
		Debuger::Log("thread2 end:%d", thread_count);
	}
#pragma endregion

#pragma region 日志
	void TestLog()
	{
		Debuger::Log("----------------------TestLog----------------------");

		Function<void(LogMsgType, const char*)> f;
		f.Bind<TestCore, &TestCore::OnRecvLogMessage>(this);
		Debuger::RegisterLogMessageReceived(f);

		Debuger::Warning("test message callback1");
		Debuger::Warning("test message callback2");
	}
	void OnRecvLogMessage(LogMsgType type, const char* msg)
	{
		Debuger::Log("recv log msg:%d - %s\n", (int)type, msg);

		Function<void(LogMsgType, const char*)> f;
		f.Bind<TestCore, &TestCore::OnRecvLogMessage>(this);
		Debuger::UnregisterLogMessageReceived(f);
	}
#pragma endregion

#pragma region 编码
	void TestEncoding()
	{
		Debuger::Log("----------------------TestEncoding----------------------");

		char out[256];
		char out1[256];
		{//字符串
			const char *in = "我";
			//unicode码转为gbk码
			int rc = Encoding::Utf8ToGBK(in, strlen(in), out, 256);
			Debuger::Log("ret:%d, unicode-->gbk out=%s", rc, out);

			//gbk转unicode
			rc = Encoding::GBKToUtf8(out, strlen(out), out1, 256);
			Debuger::Log("ret:%d, gbk-->unicode out=%s\n", rc, out1);
		}
		{//宽字符转换
			Debuger::Log("wstring convert");
			std::wstring w_str = String("我").ToWString();
			Debuger::Log("string-->wstring out=%s", w_str.c_str());

			String c_str = String::FromWString(w_str.c_str());
			Debuger::Log("wstring-->string out=%s\n", c_str.c_str());
		}
		{//文件
			Debuger::Log("write utf8 txt to file:c:/bb.txt, txt:%s", "测试utf8编码");
			File::WriteAllText("c:/bb.txt", "测试utf8编码", EncodingType::UTF8);

			String str = File::ReadAllText("c:/bb.txt", EncodingType::UTF8);
			Debuger::Log("read from file:%s", str.c_str());

			String s = Encoding::Utf8ToGBK(str);
			Debuger::Log("unicode-->gbk out=%s", s.c_str());

			str = Encoding::GBKToUtf8(s);
			Debuger::Log("gbk-->unicode out=%s\n", str.c_str());
		}

		{//utf8cpp
			std::string utf8 = "男";
			std::basic_string<wchar_t> utf16;
			utf8::utf8to16(utf8.begin(), utf8.end(), back_inserter(utf16));
			Debuger::Log("utf8-->utf16 out:%s\n", utf16.c_str());

			std::string utf8_out;
			utf8::utf16to8(utf16.begin(), utf16.end(), back_inserter(utf8_out));
			Debuger::Log("utf16-->utf8 out:%s\n", utf8_out.c_str());
		}
	}
#pragma endregion

#pragma region 时间
	void TestTime()
	{
		Debuger::Log("----------------------TestTime----------------------");

		auto data = Time::GetDate();

		uint timer_id = Timer::AddLoop(0.001f, CALLBACK_0(TestCore::OnTimeLoop, this));
		_timers.Add(timer_id);
	}
	void OnTimeLoop()
	{
		Debuger::Log("Time::GetDeltaTime() - %f", Time::GetDeltaTime());
		Debuger::Log("Time::GetRealTimeSinceStartup() - %f", Time::GetRealTimeSinceStartup());
		Debuger::Log("Time::GetFPS() - %d", Time::GetFPS());
		Debuger::Log("Time::GetDate() - %s", Time::GetDate().ToString().c_str());
	}
#pragma endregion

#pragma region 加密
	void TestCrypto()
	{
		Debuger::Log("----------------------TestCrypto----------------------");

		String md5 = String::Md5("fupeng");
		Debuger::Log("md5:%s", md5.c_str());
		md5 = String::Md5("fupeng", 6);
		Debuger::Log("md5:%s\n",md5.c_str());

		String tea = String::TeaEncrypt("123467812345678");
		Debuger::Log("tea encrypt:%s", tea.c_str());
		tea = String::TeaDecrypt(tea);
		Debuger::Log("tea decrtot:%s\n", tea.c_str());

		uint hash = String::Hash("123467812345678");
		Debuger::Log("string 123467812345678 to uint hash:%u", hash);
		hash = String::Hash("1234678123456789");
		Debuger::Log("string 1234678123456789 to uint hash:%u\n", hash);
	}
#pragma endregion

#pragma region 智能指针
	AutoPointer<TestAutoPointerClass> TestAutoPointerFun()
	{
		auto p = AutoPointer<TestAutoPointerClass>(Memory::New<TestAutoPointerClass>());
		return p;
	}
	void TestAutoPointer()
	{
		Debuger::Log("----------------------TestAutoPointer----------------------");

		auto p = TestAutoPointerFun();
	}
#pragma endregion

#pragma region 可变类型
	void TestVariant()
	{
		//Debuger::Log("----------------------TestVariant----------------------");

		//var v = 12;
		//int value = v.Get<int>();

		//Object* a = Memory::New<Object>();
		//var obj = a;
		//Object* o = obj.GetPoint<Object>();

		//var str = String("123");
		//String s = str.Get<String>();

		//SAFE_DELETE(a);
	}
#pragma endregion

#pragma region Json
	void TestJson()
	{
		Debuger::Log("----------------------TestJson----------------------");

		nlohmann::json root;
		root["name"] = "aa";
		root["id"] = 1;

		nlohmann::json root2;
		root2["one"] = root;//可包含另一个json

		std::string str = root.dump();//将json转化为string类型：反序列化
		Debuger::Log("%s", str.c_str());
	}
#pragma endregion
	
#pragma region 内存池
	void TestAllocPool()
	{
		Debuger::Log("----------------------TestAllocPool----------------------");
		{
			{
				auto ptr = Memory::NewArray<TestAutoPointerClass>(10);
				Memory::DeleteArray(ptr, 10);
			}
			{//泛型
				//auto ptr1 = Memory::New<ConstructorImpl<byte>>();
				//auto ptr2 = 
			}
		}
	}
#pragma endregion

#pragma region Zip
	void TestZip()
	{
		Debuger::Log("----------------------TestZip----------------------");

		File::Unzip("1.zip", "jsoncpp");
	}
#pragma endregion

#pragma region 反射
	void TestReflection()
	{
		Debuger::Log("----------------------TestReflection----------------------");

		const Type* c1 = Type::GetType("Vector2");
		Debuger::Log("Vector2 name:", c1->GetName().c_str());
		const Type* c2 = Type::GetType("TestReflectClass");
		Debuger::Log("TestReflectClass name:", c2->GetName().c_str());


	}
#pragma endregion

#pragma region 对齐
	void TestStructAlign()
	{
		Debuger::Log("----------------------TestStructAlign----------------------");

		int t1 = sizeof(AlignTest1);
		Debuger::Log("AlignTest1 size:%d", t1);
		int of = offsetof(AlignTest1, c);
		Debuger::Log("AlignTest1 c offset:%d", of);
		of = offsetof(AlignTest1, i);
		Debuger::Log("AlignTest1 i offset:%d", of);
		of = offsetof(AlignTest1, d);
		Debuger::Log("AlignTest1 d offset:%d", of);

		int t2 = sizeof(AlignTest2);
		Debuger::Log("AlignTest2 size:%d", t2);

		int t3 = sizeof(AlignTest3);
		Debuger::Log("AlignTest3 size:%d", t3);

	}
#pragma endregion Test

#pragma region 矩阵
	void TestMatrix4()
	{
		Debuger::Log("----------------------TestMatrix4----------------------");

		Matrix4 perspective = Matrix4::Perspective(Math::PI / 4.0f, 1.0f, 0.0f, 100.0f);

		Vector3 vec3 = Vector3(0, 0, 2);
		Vector3 vec = perspective.TransformPoint(vec3);
		Debuger::Log("Vector3(0, 0, 90) -> %s", vec.ToString().c_str());

		vec3 = Vector3(0, 0, -90);
		vec = perspective.TransformPoint(vec3);
		Debuger::Log("Vector3(0, 0, 90) -> %s", vec.ToString().c_str());

	}
#pragma endregion

#pragma region STL
	static bool SortFunLess(int a, int b)
	{
		return a < b;
	}
	bool SortFunGreater(int a, int b)
	{
		return a > b;
	}
	static bool StableSortFun(std::pair<int, int> a, std::pair<int, int> b)
	{
		return a.second > b.second;
	}
	void TestSTL()
	{
		Debuger::Log("----------------------TestSTL----------------------");

		{//Vector
			Debuger::Log("----------vector----------");
			Vector<int> vec = {0};
			vec.Add(5); vec.Add(1); vec.Add(6); vec.Add(3); vec.Add(2); vec.Add(8); vec.Add(4); vec.Add(8);
			vec.Reverse();
			vec.Reverse(0, 4);
			//不稳定排序
			vec.Sort(0, 5, CALLBACK_2(TestCore::SortFunGreater, this));
			vec.Sort(std::less<int>());
			vec.Sort(std::greater<int>());
			vec.Sort();

			//稳定排序
			Debuger::Log("vector StableSort");
			Vector<std::pair<int, int>> vec_pair;
			vec_pair.Add(std::pair<int, int>(0, 5));
			vec_pair.Add(std::pair<int, int>(1, 2));
			vec_pair.Add(std::pair<int, int>(2, 2));
			vec_pair.Add(std::pair<int, int>(3, 2));
			vec_pair.Add(std::pair<int, int>(4, 6));
			vec_pair.StableSort(StableSortFun);
			vec_pair.ForEach([](std::pair<int, int> v) {Debuger::Log("key:%d, value:%d", v.first, v.second); });
		}

		{//List
			Debuger::Log("----------list----------");
			List<int> list;
			list.Add(5); list.Add(1); list.Add(6); list.Add(3); list.Add(2); list.Add(8); list.Add(4); list.Add(8);
			list.Reverse();
			list.Reverse(0, 4);
			list.Sort();
			list.Sort(CALLBACK_2(TestCore::SortFunGreater, this));
			list.ForEach([](int v) {Debuger::Log("%d,", v); });
		}

		{//Set
			//Debuger::Log("----------set----------");
			//Set<int, SetComp> set;
			//set.Add(2); set.Add(6); set.Add(4); set.Add(3); set.Add(1);
			//set.Remove(3);
			//set.ForEach([](int v) {Debuger::Log("%d,", v); });
		}

		{//Stack
			Debuger::Log("----------stack----------");
			Stack<int> stack;
			stack.Push(3); stack.Push(1); stack.Push(5); stack.Push(7);
			int v = 0;
			if (stack.Pop(v))
			{

			}
		}

		{//Queue
			Debuger::Log("----------queue----------");
			Queue<int> queue;
			queue.Enqueue(3); queue.Enqueue(1); queue.Enqueue(5); queue.Enqueue(7);
			int v = 0;
			if (queue.Dequeue(v))
			{

			}
		}
	}
#pragma endregion

#pragma region QRCode
	void TestQRCode()
	{
		Debuger::Log("----------------------TestQRCode----------------------");

		Debuger::Log("save qrcode to file yy.bmp");
		QRCode::SaveToPicture("tests123", 0, "yy.bmp");
	}
#pragma endregion

#pragma region Algorithm
	void TestAlgorithm()
	{
		Debuger::Log("----------------------TestAlgorithm----------------------");

		{
			Array<int> arr; arr.Add(4); arr.Add(3); arr.Add(8); arr.Add(1); arr.Add(0); arr.Add(5); arr.Add(2); arr.Add(6); arr.Add(9); arr.Add(7);
			arr.BubbleSort();
			Debuger::Log("BubbleSort:");
			for (int i = 0; i < arr.Size(); ++i)
			{
				printf("%d,", arr[i]);
			}
			printf("\n");
		}
		{
			Array<int> arr; arr.Add(4); arr.Add(3); arr.Add(8); arr.Add(1); arr.Add(0); arr.Add(5); arr.Add(2); arr.Add(6); arr.Add(9); arr.Add(7);
			arr.SelectionSort();
			Debuger::Log("SelectionSort:");
			for (int i = 0; i < arr.Size(); ++i)
			{
				printf("%d,", arr[i]);
			}
			printf("\n");
		}
		{
			Array<int> arr; arr.Add(4); arr.Add(3); arr.Add(8); arr.Add(1); arr.Add(0); arr.Add(5); arr.Add(2); arr.Add(6); arr.Add(9); arr.Add(7);
			arr.InsertionSort();
			Debuger::Log("InsertionSort:");
			for (int i = 0; i < arr.Size(); ++i)
			{
				printf("%d,", arr[i]);
			}
			printf("\n");
		}
		{
			Array<int> arr; arr.Add(4); arr.Add(3); arr.Add(8); arr.Add(1); arr.Add(0); arr.Add(5); arr.Add(2); arr.Add(6); arr.Add(9); arr.Add(7);
			arr.ShellSort();
			Debuger::Log("ShellSort:");
			for (int i = 0; i < arr.Size(); ++i)
			{
				printf("%d,", arr[i]);
			}
			printf("\n");
		}
		{
			Array<int> arr; arr.Add(4); arr.Add(3); arr.Add(8); arr.Add(1); arr.Add(0); arr.Add(5); arr.Add(2); arr.Add(6); arr.Add(9); arr.Add(7);
			arr.MergeSort();
			Debuger::Log("MergeSort:");
			for (int i = 0; i < arr.Size(); ++i)
			{
				printf("%d,", arr[i]);
			}
			printf("\n");
		}
		{
			Array<int> arr; arr.Add(4); arr.Add(3); arr.Add(8); arr.Add(1); arr.Add(0); arr.Add(5); arr.Add(2); arr.Add(6); arr.Add(9); arr.Add(7);
			arr.QuickSort();
			Debuger::Log("QuickSort:");
			for (int i = 0; i < arr.Size(); ++i)
			{
				printf("%d,", arr[i]);
			}
			printf("\n");
		}
		{
			Array<int> arr; arr.Add(4); arr.Add(3); arr.Add(8); arr.Add(1); arr.Add(0); arr.Add(5); arr.Add(2); arr.Add(6); arr.Add(9); arr.Add(7);
			arr.HeapSort();
			Debuger::Log("HeapSort:");
			for (int i = 0; i < arr.Size(); ++i)
			{
				printf("%d,", arr[i]);
			}
			printf("\n");
		}
		{
			Array<int> arr; arr.Add(4); arr.Add(3); arr.Add(8); arr.Add(1); arr.Add(0); arr.Add(5); arr.Add(2); arr.Add(6); arr.Add(9); arr.Add(7);
			arr.CountingSort();
			Debuger::Log("CountingSort:");
			for (int i = 0; i < arr.Size(); ++i)
			{
				printf("%d,", arr[i]);
			}
			printf("\n");
		}
		{
			Array<int> arr; arr.Add(4); arr.Add(3); arr.Add(8); arr.Add(1); arr.Add(0); arr.Add(5); arr.Add(2); arr.Add(6); arr.Add(9); arr.Add(7);
			arr.RadixSort();
			Debuger::Log("RadixSort:");
			for (int i = 0; i < arr.Size(); ++i)
			{
				printf("%d,", arr[i]);
			}
			printf("\n");
		}
		{
			Debuger::Log("rb tree----------------------");
			int a[] = { 10, 40, 30, 60, 90, 70, 20, 50, 80 };
			int check_insert = 0;    // "插入"动作的检测开关(0，关闭；1，打开)
			int check_remove = 0;    // "删除"动作的检测开关(0，关闭；1，打开)
			int i;
			int ilen = (sizeof(a)) / (sizeof(a[0]));
			RBTree<int>* tree = new RBTree<int>();

			std::cout << "== 原始数据: ";
			for (i = 0; i < ilen; i++)
				std::cout << a[i] << " ";
			std::cout << std::endl;

			for (i = 0; i < ilen; i++)
			{
				tree->Insert(a[i]);
				// 设置check_insert=1,测试"添加函数"
				if (check_insert)
				{
					std::cout << "== 添加节点: " << a[i] << std::endl;
					std::cout << "== 树的详细信息: " << std::endl;
					tree->Print();
					std::cout << std::endl;
				}

			}

			std::cout << "== 前序遍历: ";
			tree->PreOrder();

			std::cout << "\n== 中序遍历: ";
			tree->InOrder();

			std::cout << "\n== 后序遍历: ";
			tree->PostOrder();
			std::cout << std::endl;

			std::cout << "== 最小值: " << tree->Minimum() << std::endl;
			std::cout << "== 最大值: " << tree->Maximum() << std::endl;
			std::cout << "== 树的详细信息: " << std::endl;
			tree->Print();

			// 设置check_remove=1,测试"删除函数"
			if (check_remove)
			{
				for (i = 0; i < ilen; i++)
				{
					tree->Remove(a[i]);

					std::cout << "== 删除节点: " << a[i] << std::endl;
					std::cout << "== 树的详细信息: " << std::endl;
					tree->Print();
					std::cout << std::endl;
				}
			}
		}
		{
			Debuger::Log("avl tree----------------------");
			AVLTree<int> tree;
			tree.Add(10); tree.Add(40); 
			tree.Add(30); 
			tree.Add(60); tree.Add(90); tree.Add(70); tree.Add(20); tree.Add(50); tree.Add(80);
			std::cout << "中序遍历";
			tree.InOrder();
			std::cout << "\n前序遍历:";
			tree.PreOrder();
			if (tree.Contains(30))
				std::cout << "已查找到30" << std::endl;
			else
				std::cout << "值为30的结点不存在" << std::endl;
			tree.Remove(30);
			std::cout << "删除后的中序遍历：";
			tree.InOrder();
			std::cout << "\n删除后的前序遍历：";
			tree.PreOrder();

			std::cout << std::endl;
		}
		{
			Debuger::Log("hash table----------------------");
			HashTable<std::string, std::string, HashFunc_String, EqualKey_String> hashmap(3);

			hashmap.Insert("hello", "world");
			hashmap.Insert("why", "dream");
			hashmap.Insert("c++", "good");
			hashmap.Insert("welcome", "haha");


			std::cout << "after insert:" << std::endl;
			std::cout << hashmap.Find("welcome").c_str() << std::endl;
			std::cout << hashmap.Find("c++").c_str() << std::endl;
			std::cout << hashmap["why"].c_str() << std::endl;
			std::cout << hashmap["hello"].c_str() << std::endl;

			if (hashmap.Remove("hello"))
				std::cout << "remove is ok" << std::endl;
			std::cout << hashmap.Find("hello").c_str() << std::endl;

			hashmap["why"] = "love";
			std::cout << hashmap["why"].c_str() << std::endl;

			std::cout << std::endl;
		}
	}
#pragma endregion

#pragma region xml
	void TestXML()
	{
		Debuger::Log("----------------------TestXML----------------------");

		String path(Resource::GetFullDataPath("internal/material/Default.material"));

		tinyxml2::XMLDocument doc;
		tinyxml2::XMLError error = doc.LoadFile(path.c_str());
		if (error != tinyxml2::XML_SUCCESS)
			Debuger::Error("open xml error:%s", path.c_str());

		int result = 0;
		tinyxml2::XMLElement* root = doc.RootElement();
		tinyxml2::XMLElement* passNode = root->FirstChildElement("Pass");
		while (passNode != nullptr)
		{
			tinyxml2::XMLElement* shaderNode = passNode->FirstChildElement("Shader");
			if (shaderNode != nullptr)
			{
				tinyxml2::XMLElement* vs_node = shaderNode->FirstChildElement("VS");
				if (vs_node != nullptr)
				{
					const char* file = vs_node->Attribute("File");
					Debuger::Log("vs:%s", file);
				}
				tinyxml2::XMLElement* ps_node = shaderNode->FirstChildElement("PS");
				if (ps_node != nullptr)
				{
					const char* file = ps_node->Attribute("File");
					Debuger::Log("ps:%s", file);
				}
			}

			passNode = passNode->NextSiblingElement("Pass");
		}


	}
#pragma endregion

#pragma region Delegate
	int delegateCount = 0;
	int OnFunctionAdd(int aa, int bb)
	{
		return aa + bb;
	}
	void OnDelegateBind(int aa, int bb)
	{
		delegateCount++;
		Debuger::Log("%d", aa + bb);
	}
	void TestDelegate()
	{
		Debuger::Log("----------------------TestDelegate----------------------");

		Function<int(int, int)> fun_add;
		fun_add.Bind<TestCore, &TestCore::OnFunctionAdd>(this);
		int cc = fun_add(1,2);

		Delegate<int, int> delegate;
		delegate.Bind<TestCore, &TestCore::OnDelegateBind>(this);
		delegate(1,2);
		MyAssert(delegateCount == 1);
		delegate.Unbind<TestCore, &TestCore::OnDelegateBind>(this);
		delegate(1, 2);
		MyAssert(delegateCount == 1);

		delegate.Bind<TestCore, &TestCore::OnDelegateBind>(this);
		delegate(1, 2);
		MyAssert(delegateCount == 2);
		delegate.Unbind<TestCore, &TestCore::OnDelegateBind>(this);
		delegate(1, 2);
		MyAssert(delegateCount == 2);

		delegateCount = 0;
		for (int i = 0; i < 100; ++i)
		{
			delegate.Bind<TestCore, &TestCore::OnDelegateBind>(this);
			delegate.Unbind<TestCore, &TestCore::OnDelegateBind>(this);
		}
		delegate(1, 2);
		MyAssert(delegateCount == 0);

		delegateCount = 0;
		for (int i = 0; i < 100; ++i)
		{
			delegate.Bind<TestCore, &TestCore::OnDelegateBind>(this);
		}
		delegate(1, 2);
		MyAssert(delegateCount == 100);
		for (int i = 0; i < 100; ++i)
		{
			delegate.Unbind<TestCore, &TestCore::OnDelegateBind>(this);
		}
		delegate(1, 2);
		MyAssert(delegateCount == 100);
	}
#pragma endregion

#pragma region Template
	void TestTemplates()
	{
		Debuger::Log("----------------------TestTemplate----------------------");
		TestTemplate t;
		t.Run();
	}
#pragma endregion

#pragma region Struct
	void TestStruct()
	{
		{
			Debuger::Log("----------------------Array----------------------");
			Array<int> arr;
			for(int i = 0; i < 100; ++i)
				arr.Add(i);
			for (int i = 0; i < 100; ++i)
				MyAssert(arr[i] == i);

			Array<int> arr1 = arr;
			for (int i = 0; i < 100; ++i)
				MyAssert(arr1[i] == i);

			MyAssert(arr1 == arr);
		}
	}
#pragma endregion

#pragma region Meta
	//---------- 测试代码 ----------
	void PrintMetaInfo(const MetaPropertyInfo& prop) {
		std::cout << "  " << prop.name << " [" << prop.type << "]\n";

		std::cout << "  Metadata:\n";
		for (const auto& meta : prop.meta.data)
		{
			std::cout << "    - key:" << meta.first.c_str() << ", value:" << meta.second.c_str() << std::endl;
		}
	}

	void PrintFunctionInfo(const MetaFunctionInfo& func) {
		std::cout << "  " << func.name << "() -> " << func.returnType << "\n";
		std::cout << "  Parameters: ";
		for (const auto& param : func.paramTypes) {
			std::cout << "    - " << param << " ";
		}
		std::cout << "\n";

		std::cout << "  Metadata:\n";
		for (const auto& meta : func.meta.data)
		{
			std::cout << "    - key:" << meta.first.c_str() << ", value:" << meta.second.c_str() << std::endl;
		}
	}
	void TestMeta()
	{
		Debuger::Log("----------------------Meta----------------------");
		
		TestMetaObj meta;
		meta.SetId(1);
		meta.SetName("Hero");

		std::cout << "\nProperties:\n";
		for (const auto& prop : meta.GetClass()->properties) {
			PrintMetaInfo(prop);
			std::cout << "----------\n";
		}

		std::cout << "\nFunctions:\n";
		for (const auto& func : meta.GetClass()->functions) {
			PrintFunctionInfo(*func);
			std::cout << "----------\n";
		}

		// 直接调用函数
		std::cout << "\nDirect function calls:\n";
		//meta.Print();
		//meta.PrintInt(20);

		// 反射调用函数
		std::cout << "\nReflection function calls:\n";
		meta.Invoke("Print");
		meta.Invoke("PrintInt", 20);

		meta.Invoke("SetId", 2);
		meta.Invoke("SetName", std::string("2"));
		MyAssert(meta.Invoke<int>("GetId") == 2);
		MyAssert(meta.Invoke<std::string>("GetName") == "2");
	}
#pragma endregion
};
DC_END_NAMESPACE