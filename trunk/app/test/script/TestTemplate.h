 #pragma once

#include "TestBase.h"

DC_BEGIN_NAMESPACE

class MyClass
{
public:
	MyClass()
	{
		Debuger::Log("MyClass::MyClass");
	}
	MyClass(int a)
	{
		Debuger::Log("MyClass::MyClass:%d", a);
	}
	virtual ~MyClass()
	{
		Debuger::Log("MyClass::~MyClass");
	}

private:
	int a = 0;
	int b = 0;
};

class DerivedMyClass : public MyClass
{
public:
	DerivedMyClass(int b)
	{
		Debuger::Log("DerivedMyClass::DerivedMyClass");
	}
	~DerivedMyClass()
	{
		Debuger::Log("DerivedMyClass::~DerivedMyClass");
	}

private:
	int c = 0;
};

class EmptyClass
{
};
class AbstractClass
{
	virtual void Print() = 0;
};
class DerivedAbstractClass final : public AbstractClass
{
	virtual void Print()override {}
};
struct MyStruct
{
	int s;
};

union MyUnion
{
	int a;
	char b;
};

enum class MyEnum
{
	One,
	Tow,
	Three,
};

#pragma region enable_if
template<typename T>
inline typename std::enable_if<!std::is_trivially_constructible<T>::value>::type ConstructItem(T* dst)
{
	new(dst) T();
}
template<typename T>
inline typename std::enable_if<std::is_trivially_constructible<T>::value>::type ConstructItem(T* dst)
{
}
void enable_if()
{
	std::cout << "--------enable_if--------" << std::endl;
	MyClass* ptr = (MyClass*)Memory::Alloc(sizeof(MyClass));
	ConstructItem(ptr);
	Memory::Free(ptr);

	int* ptr_i = (int*)Memory::Alloc(sizeof(int));
	ConstructItem(ptr_i);
	Memory::Free(ptr_i);
}
#pragma endregion

#pragma region test_simple_is
/*
主要类型类别
	is_void 测试类型是否为 void。
	is_null_pointer 测试类型是否为 std::nullptr_t。
	is_integral 测试类型是否为整型。
	is_floating_point 测试类型是否为浮点。
	is_array 测试类型是否为数组。
	is_pointer 测试类型是否为指针。
	is_lvalue_reference 测试类型是否是左值引用。
	is_rvalue_reference 测试类型是否是右值引用。
	is_member_object_pointer 测试类型是否为指向成员对象的指针。
	is_member_function_pointer 测试类型是否为指向成员函数的指针。
	is_enum 测试类型是否为枚举。
	is_union 测试类型是否为联合。
	is_class 测试类型是否为类。
	is_function 测试类型是否为函数类型。
复合类型类别
	is_reference 测试类型是否为引用。
	is_arithmetic 测试类型是否为算术型。
	is_fundamental 测试类型是否为 void 或算术型。
	is_object 测试类型是否为对象类型。
	is_scalar 测试类型是否为标量类型。
	is_compound 测试类型是否为非标量类型。
	is_member_pointer 测试类型是否为指向成员的指针。
类型属性
	is_const 测试类型是否为 const。
	is_volatile 测试类型是否为 volatile。
	is_trivial 测试类型是否为简单。
	is_trivially_copyable 测试类型是否为完全复制。
	is_standard_layout 测试类型是否为标准布局类型。
	is_pod 测试类型是否为 POD。
	is_literal_type 测试是否的类型可以是 constexpr 变量或用在 constexpr 函数。
	is_empty 测试类型是否为空类。
	is_polymorphic 测试类型是否为多态类。
	is_abstract 测试类型是否为抽象类。
	is_final 测试类型是否为标记的类类型 final。
	is_signed 测试类型是否为有符号的整数。
	is_unsigned 测试类型是否为无符号的整数。
	is_constructible 测试类型是否为可构造使用指定的参数类型。
	is_default_constructible 测试类型是否包含一个默认构造函数。
	is_copy_constructible 测试类型是否包含一个复制构造函数。
	is_move_constructible 测试类型是否包含移动构造函数。
	is_assignable 测试是否第一种类型都可以分配到第二个类型的值。
	is_copy_assignable 测试是否可以指定一个类型的类型的常量引用值。
	is_move_assignable 测试是否可以指定一个类型的右值引用的类型。
	is_destructible 测试该类型是否易损坏。
	is_trivially_constructible 测试是否不使用任何重要的操作时使用的指定的类型构造的类型。
	is_trivially_default_constructible 测试是否不使用任何重要的操作类型默认构造时。
	is_trivially_copy_constructible 测试是否复制构造时不使用任何重要的操作类型。
	is_trivially_move_constructible 测试是否移动构造时不使用任何重要的操作类型。
	is_trivially_assignable 测试是否可分配的类型赋值不使用任何重要的操作。
	is_trivially_copy_assignable 测试类型是否是可赋值的复制和赋值使用任何重要的操作。
	is_trivially_move_assignable 测试类型是否是可赋值的移动和分配使用任何重要的操作。
	is_trivially_destructible 测试类型易损坏以及析构函数使用任何重要的操作。
	is_nothrow_constructible 测试类型可构造，以及已知不在构造使用指定的类型时引发。
	is_nothrow_default_constructible 测试类型是否为默认构造和已知没有默认构造时引发。
	is_nothrow_copy_constructible 测试是否类型是复制构造的并且复制构造函数已知不引发。
	is_nothrow_move_constructible 测试类型移动构造，以及移动构造函数已知不引发。
	is_nothrow_assignable 测试类型可使用指定的类型赋值以及分配已知不引发。
	is_nothrow_copy_assignable 测试类型可分配的副本以及分配已知不引发。
	is_nothrow_move_assignable 测试类型移动赋值以及分配已知不引发。
	is_nothrow_destructible 测试类型易损坏以及析构函数已知不引发。
	has_virtual_destructor 测试类型是否包含虚拟的析构函数。

类型关系
	is_same 确定两个类型是否相同。
	is_base_of 测试是否是一种类型的另一个基。
	is_convertible 测试一种类型是否可转换为另一种类型。

Const 易失性的修改
	add_const 生成 const 从类型的类型。
	add_volatile 生成 volatile 从类型的类型。
	add_cv 生成 constvolatile 从类型的类型。
	remove_const 生成类型中的非常量类型。
	remove_volatile 生成类型的非易失性类型。
	remove_cv 生成类型的非 const 非易失性类型。
*/
void test_simple_is()
{
	std::cout << "--------test_simple_is--------" << std::endl;
	std::cout << "is_empty EmptyClass: "				<< std::is_empty<EmptyClass>::value << std::endl;
	std::cout << "is_polymorphic DerivedAbstractClass: "<< std::is_polymorphic<DerivedAbstractClass>::value << std::endl;
	std::cout << "is_abstract AbstractClass: "			<< std::is_abstract<AbstractClass>::value << std::endl;
	std::cout << "is_final DerivedAbstractClass："		<< std::is_final<DerivedAbstractClass>::value << std::endl;
	std::cout << "is_signed int:"						<< std::is_signed<int>::value << std::endl;	
	std::cout << "is_unsigned uint:"					<< std::is_unsigned<unsigned int>::value << std::endl;

	std::cout << "is_void void:"						<< std::is_void<void>::value << '\n';
	std::cout << "is_void int:"							<< std::is_void<int>::value << '\n';

	std::cout << "is_pointer MyClass:"					<< std::is_pointer<MyClass>::value << '\n';
	std::cout << "is_pointer MyClass*:"					<< std::is_pointer<MyClass *>::value << '\n';
	std::cout << "is_pointer MyClass&:"					<< std::is_pointer<MyClass &>::value << '\n';
	std::cout << "is_pointer int:"						<< std::is_pointer<int>::value << '\n';
	std::cout << "is_pointer int *:"					<< std::is_pointer<int *>::value << '\n';
	std::cout << "is_pointer int **:"					<< std::is_pointer<int **>::value << '\n';
	std::cout << "is_pointer int[10]:"					<< std::is_pointer<int[10]>::value << '\n';
	std::cout << "is_pointer std::nullptr_t:"			<< std::is_pointer<std::nullptr_t>::value << '\n';

	//is_object包括除函数、引用和void以外的所有类型。
	std::cout << "is_object int:"						<< std::is_object<int>::value << '\n';
	std::cout << "is_object int&:"						<< std::is_object<int&>::value << '\n';
	std::cout << "is_object MyClass:"					<< std::is_object<MyClass>::value << '\n';
	std::cout << "is_object MyClass&:"					<< std::is_object<MyClass&>::value << '\n';

	std::cout << "is_enum MyClass: "					<< std::is_enum<MyClass>::value << std::endl;
	std::cout << "is_enum MyStruct: "					<< std::is_enum<MyStruct>::value << std::endl;

	//class 和 struct是，enum 和 union不是
	std::cout << "MyClass is_class: "					<< std::is_class<MyClass>::value << std::endl;//true
	std::cout << "MyStruct is_class: "					<< std::is_class<MyStruct>::value << std::endl;//true
	std::cout << "MyUnion is_class: "					<< std::is_class<MyUnion>::value << std::endl;//false
	std::cout << "MyEnum is_class: "					<< std::is_class<MyEnum>::value << std::endl;//false

	//is_reference 是否引用
	std::cout << "is_reference int: "					<< std::is_reference<int>::value << std::endl;//false
	std::cout << "is_reference int&: "					<< std::is_reference<int&>::value << std::endl;//true
	std::cout << "is_reference int&&: "					<< std::is_reference<int&&>::value << std::endl;//true

	//bool
	//char
	//char16_t
	//char32_t
	//wchar_t
	//signed char
	//short int
	//int
	//long int
	//long long int
	//unsigned char
	//unsigned short int
	//unsigned int
	//unsigned long int
	//unsigned long long int
	std::cout << "is_integral char: "					<< std::is_integral<char>::value << std::endl;//true
	std::cout << "is_integral int: "					<< std::is_integral<int>::value << std::endl;//true
	std::cout << "is_integral float: "					<< std::is_integral<float>::value << std::endl;//false

	std::cout << "is_array int: "						<< std::is_array<int>::value << std::endl;//false
	std::cout << "is_array int[3]: "					<< std::is_array<int[3]>::value << std::endl;//true
	std::cout << "is_array array<int,3>: "				<< std::is_array<std::array<int, 3>>::value << std::endl;//false
	std::cout << "is_array string: "					<< std::is_array<std::string>::value << std::endl;//false
	std::cout << "is_array string[3]: "					<< std::is_array<std::string[3]>::value << std::endl;//true


}
#pragma endregion

#pragma region is_const
void is_const()
{
	std::cout << "--------is_const--------" << std::endl;
	std::cout << "int: " << std::is_const<int>::value << std::endl;					// 0
	std::cout << "const int: " << std::is_const<const int>::value << std::endl;		// 1
	std::cout << "const int&: " << std::is_const<const int&>::value << std::endl;	// 0
	std::cout << "cont int* " << std::is_const<const int*>::value << std::endl;		// 0
	std::cout << "int * const:" << std::is_const<int* const>::value << std::endl;	// 1
}
#pragma endregion

#pragma region is_trivial
class TrivialA {
};
class TrivialB {
public:
	int a;
};
class TrivialC 
{
	TrivialC() {}								// 有默认构造函数，不平凡
};
class TrivialD 
{
	TrivialD(const TrivialD& a) {}				// 有赋值构造函数，不平凡
};
class TrivialE 
{
	TrivialE(TrivialE&& a) {}					// 有移动构造函数，不平凡
};
class TrivialF 
{
	TrivialF& operator=(const TrivialF& a) {}   // 有赋值构造函数，不平凡
};
class TrivialG 
{
	virtual void func() = 0;					// 有虚函数，不平凡
};
class TrivialH : virtual public TrivialA		// 有虚基类，不平凡
{               
};
/*
平凡的类或结构体必须满足以下的条件：
- 不能包含默认构造函数和析构函数
- 不能包含拷贝构造函数和移动构造函数
- 不能包含赋值构造函数
- 不能包含虚函数和虚基类
*/
void is_trivial()
{
	std::cout << "--------is_trivial(测试类型是否为平凡的)--------" << std::endl;
	std::cout << "有构造函数: " << std::is_trivial<MyClass>::value << std::endl;
	std::cout << "没有构造有变量: " << std::is_trivial<EmptyClass>::value << std::endl;
	std::cout << "结构体: " << std::is_trivial<MyStruct>::value << std::endl;
	std::cout << "int：" << std::is_trivial<int>::value << std::endl;
	std::cout << "std::string：" << std::is_trivial<std::string>::value << std::endl;
}
#pragma endregion

#pragma region is_standard_layout
class StdLayoutA {
};
class StdLayoutB {
public:
	int a;
	int b;
};
class StdLayoutC : public StdLayoutA {
public:
	int a;
	int b;
	void fun() {}
};
class StdLayoutD : public StdLayoutA {
public:
	int a;
	StdLayoutA sla;
};
class StdLayoutE : public StdLayoutA, public StdLayoutC {
};
class StdLayoutF {
public:
	static int a;
};
class StdLayoutG : public StdLayoutF {
public:
	int a;
};
class StdLayoutH : public StdLayoutA {		// 第一个非静态成员是基类，所以不标准，如果sla的位置和b交换，那么是标准的
public:
	StdLayoutA sla;
	int b;
};
class StdLayoutI : public StdLayoutB {		// 基类和派生类都有非静态变量，所以不标准，如果基类或者派生类中只有静态变量，那么是标准的
public:
	int a;
};
class StdLayoutJ : public StdLayoutI {		// 基类不标准，所以不标准，如果StdLayoutI标准，那么是标准的
};
class StdLayoutK {							// 非静态成员权限不同，所以不标准，如果a或b有一个是静态，那么是标准的
public:
	int a;
private:
	int b;
};
/*
1.标准布局的类或结构体必须满足以下的条件：
	所有非静态成员有相同的访问权限。
	在类或结构体继承时满足以下两个条件之一：
	1、派生类中有非静态成员，且只有仅包含静态成员的基类。
	2、基类有非静态成员，而派生类没有非静态成员。
	其实就是派生类和基类中不允许同时出现非静态成员，因为同时有非静态成员就无法进行memcpy
2.类中第一个非静态成员的类型与基类不同。
	C++标准允许，在基类没有成员时，派生类第一个成员与基类共享地址。但是当派生类中第一个数据成员类型为基类类型时，有趣的问题就来了。
	首先，这时派生类的内存布局包括基类部分的内存布局，同时自己又添加了另外一个基类类型的变量，如果编译器优化实现第一个成员和基类部分共享地址，
	那么就违背了C++标准的另一个要求，同类型的不同对象地址必须不同。
3.没有虚函数和虚基类。
4.所有非静态成员均符合标准布局，其基类也符合标准布局。
*/
void is_standard_layout()
{
	std::cout << "--------is_standard_layout--------" << std::endl;
	std::cout << std::is_standard_layout<StdLayoutA>::value << std::endl;
	std::cout << std::is_standard_layout<StdLayoutB>::value << std::endl;
	std::cout << std::is_standard_layout<StdLayoutC>::value << std::endl;
	std::cout << std::is_standard_layout<StdLayoutD>::value << std::endl;
	std::cout << std::is_standard_layout<StdLayoutE>::value << std::endl;
	std::cout << std::is_standard_layout<StdLayoutF>::value << std::endl;
	std::cout << std::is_standard_layout<StdLayoutG>::value << std::endl;
	std::cout << std::is_standard_layout<StdLayoutH>::value << std::endl;   // 以上都是1，从这里开始都是0
	std::cout << std::is_standard_layout<StdLayoutI>::value << std::endl;
	std::cout << std::is_standard_layout<StdLayoutJ>::value << std::endl;
	std::cout << std::is_standard_layout<StdLayoutK>::value << std::endl;
}
#pragma endregion

#pragma region is_pod
class PODClass
{
public:
	int x;
	double y;
};
/*
POD类型，plain old data。Plain代表数据是普通类型，old代表能与C兼容支持memcpy、memset等函数。 POD分为两个部分，trivial(平凡的)和(s’tan’dard layout)标准布局的，必须同时满足才是POD类型
POD类型的好处：
1.字节赋值，可以使用memset和memcpy。
2.兼容C语言的内存布局
3.保证静态初始化安全有效
*/
void is_pod()
{
	std::cout << "--------is_pod--------" << std::endl;

	if (std::is_pod<PODClass>::value)
	{//测试数据迁移
		std::cout << "before" << std::endl;
		PODClass a;
		a.x = 8;
		a.y = 10.5;
		std::cout << a.x << std::endl;
		std::cout << a.y << std::endl;

		size_t size = sizeof(a);
		char *p = new char[size];
		::memcpy(p, &a, size);
		PODClass *pA = (PODClass*)p;

		std::cout << "after" << std::endl;
		std::cout << pA->x << std::endl;
		std::cout << pA->y << std::endl;

		delete [] p;
	}
}
#pragma endregion

#pragma region is_constructible
class TestConstructible {
	int v1;
	double v2;
public:
	TestConstructible(int n) : v1(n), v2() {}
	TestConstructible(int n, double f) noexcept : v1(n), v2(f) {}
};
void is_constructible()
{
	std::cout << "--------is_constructible--------" << std::endl;
	std::cout << "TestConstructible is ...\n" << std::boolalpha
		<< "\tTrivially-constructible from const Foo&? "
		<< std::is_trivially_constructible<TestConstructible, const TestConstructible&>::value << '\n'
		<< "\tTrivially-constructible from int? "
		<< std::is_trivially_constructible<TestConstructible, int>::value << '\n'
		<< "\tConstructible from int? "
		<< std::is_constructible<TestConstructible, int>::value << '\n'
		<< "\tConstructible from float? "
		<< std::is_constructible<TestConstructible, float>::value << '\n'
		<< "\tNothrow-constructible from int? "
		<< std::is_nothrow_constructible<TestConstructible, int>::value << '\n'
		<< "\tNothrow-constructible from int and double? "
		<< std::is_nothrow_constructible<TestConstructible, int, double>::value << '\n';
}
/*
is_copy_constructible:如果类型 T 是具有默认构造函数的类类型，则类型谓词的实例为 true，否则为 false
is_copy_constructible:如果类型 T 是具有复制构造函数的类，则类型谓词的实例为 true；否则为 false。
is_move_constructible:类型谓词的计算结果为 true 类型 T 可以通过使用移动操作构造。 此谓词等效于 is_constructible<T, T&&>。
*/
void is_default_constructible()
{
	std::cout << "--------is_default_constructible--------" << std::endl;
	std::cout << std::is_default_constructible<MyClass>::value << std::endl;
	std::cout << std::is_default_constructible<DerivedMyClass>::value << std::endl;
}
/*
is_trivially_constructible:测试是否使用指定的参数类型时，是完全无法构造类型。
is_trivially_default_constructible:测试类型是否具有普通默认构造函数。
is_trivially_copy_constructible:测试类型是否包含普通的复制构造函数。
is_trivially_move_constructible:测试类型是否具有普通移动构造函数。
*/
void is_trivially_constructible()
{
	std::cout << "--------is_default_constructible--------" << std::endl;
	std::cout << std::is_trivially_constructible<TestConstructible, int>::value << std::endl;		// 0
	std::cout << std::is_trivially_default_constructible<TestConstructible>::value << std::endl;	// 
	std::cout << std::is_trivially_copy_constructible<TestConstructible>::value << std::endl;		// 
	std::cout << std::is_trivially_move_constructible<TestConstructible>::value << std::endl;		// 
}
enum underlying_type1 {};
enum class underlying_type2 {};
enum class underlying_type3 : unsigned {};
enum class underlying_type4 : int {};
/*
获取枚举类型T的基础类型
*/
void underlying_type()
{
	constexpr bool e1_t = std::is_same< std::underlying_type<underlying_type1>::type, int >::value;
	constexpr bool e2_t = std::is_same< std::underlying_type<underlying_type2>::type, int >::value;
	constexpr bool e3_t = std::is_same< std::underlying_type<underlying_type3>::type, int >::value;
	constexpr bool e4_t = std::is_same< std::underlying_type<underlying_type4>::type, int >::value;

	std::cout
		<< "underlying type for 'e1' is " << (e1_t ? "int" : "non-int") << '\n'
		<< "underlying type for 'e2' is " << (e2_t ? "int" : "non-int") << '\n'
		<< "underlying type for 'e3' is " << (e3_t ? "int" : "non-int") << '\n'
		<< "underlying type for 'e4' is " << (e4_t ? "int" : "non-int") << '\n'
		;
}
#pragma endregion

class TestTemplate
{
public: 
	void Run()
	{
		enable_if();
		test_simple_is();
		is_const();
		is_trivial();
		is_standard_layout();
		is_pod();
		is_constructible();
		is_default_constructible();
		is_trivially_constructible();
		underlying_type();
	}
};
DC_END_NAMESPACE