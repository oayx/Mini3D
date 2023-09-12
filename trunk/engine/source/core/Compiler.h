#pragma once

/********************************************************************/
#if defined(__clang__)
	#define DLL_EXPORT __attribute__ ((__visibility__ ("default")))
	#define DLL_IMPORT
	#define THREADLOCAL __thread
	//#define STDCALL __attribute__((stdcall))
	//#define CDECL __attribute__((cdecl))
	#define RESTRICT __restrict__
	#define INLINE inline
	#define FORCE_INLINE inline
	#define FORCE_NOINLINE __attribute__((noinline))
	#define NO_RETURN __attribute__((noreturn))
	#define PACK_BEGIN()
	#define PACK_END() __attribute__((__packed__))
	#define ALIGN_BEGIN(_align)
	#define ALIGN_END(_align) __attribute__( (aligned(_align) ) )
	#define OFFSET_OF(X, Y) __builtin_offsetof(X, Y)
	#define DEPRECATED

	#pragma clang diagnostic ignored "-Wswitch"
	#pragma clang diagnostic ignored "-Wmacro-redefined"
	#pragma clang diagnostic ignored "-Waddress-of-packed-member"
	#pragma clang diagnostic ignored "-Wnull-dereference"
	#pragma clang diagnostic ignored "-Winvalid-noreturn"
#elif defined(__GNUC__)
	#define DLL_EXPORT __attribute__ ((__visibility__ ("default")))
	#define DLL_IMPORT
	#define THREADLOCAL __thread
	//#define STDCALL __attribute__((stdcall))
	//#define CDECL __attribute__((cdecl))
	#define RESTRICT __restrict__
	#define INLINE inline
	#define FORCE_INLINE inline
	#define FORCE_NOINLINE __attribute__((noinline))
	#define NO_RETURN __attribute__((noreturn))
	#define PACK_BEGIN()
	#define PACK_END() __attribute__((__packed__))
	#define ALIGN_BEGIN(_align)
	#define ALIGN_END(_align) __attribute__( (aligned(_align) ) )
	#define OFFSET_OF(X, Y) __builtin_offsetof(X, Y)
	#define DEPRECATED __attribute__((deprecated))
#elif defined(_MSC_VER)
	#if _MSC_VER < 1800
		#error "Required Visual Studio 2013 or newer."
	#endif

	#define DLL_EXPORT __declspec(dllexport)
	#define DLL_IMPORT __declspec(dllimport)
	#define THREADLOCAL __declspec(thread)
	//#define STDCALL __stdcall								//win32 API采用的是_stdcall调用，函数参数按照从右到左的顺序入栈，被调用的函数在返回前清理传送参数的栈，函数参数个数固定
	//#define CDECL __cdecl									//C/C++的缺省调用方式，函数参数按照从右到左的顺序入栈，并且由调用函数者把参数弹出栈以清理堆栈
	#define RESTRICT __restrict
	#define INLINE __inline
	#define FORCE_INLINE __forceinline
	#define FORCE_NOINLINE __declspec(noinline)
	#define NO_RETURN __declspec(noreturn)					//一个函数被__declspec(noreturn)所修饰，那么它的含义是告诉编译器，这个函数不会返回，其结果是让编译器知道被修饰为__declspec(noreturn)的函数之后的代码不可到达
	#define PACK_BEGIN() __pragma(pack(push, 1))
	#define PACK_END() ; __pragma(pack(pop))
	#define ALIGN_BEGIN(_align) __declspec(align(_align))	//其中_align是对齐参数，其有效值是2的整数次幂（从1到8192字节），如2，4，8，16，32或64
	#define ALIGN_END(_align)
	#define OFFSET_OF(X, Y) offsetof(X, Y)					//自动返回结构对象中，某变量距离结构体对象首地址的偏移值
	#define DEPRECATED __declspec(deprecated)				//用于指定函数的某个重载形式是不推荐的。当在程序中调用了被deprecated修饰的函数时，编译器将给出C4996警告，并且可以指定具体的警告信息。该警告信息可以来源于定义的宏。例如：__declspec(deprecated) void func1(int) {}
#else
	#pragma error "Unknown compiler."
#endif

#define Final final

/********************************************************************/
//命名空间	
#define DC_BEGIN_NAMESPACE namespace dc{
#define DC_END_NAMESPACE }
#define USING_NAMESPACE_DC using namespace dc

//__declspec(dllexport)的作用，就是为了省掉在DEF文件中手工定义导出哪些函数的一个方法。
//当然，如果你的DLL里全是C++的类的话，你无法在DEF里指定导出的函数，只能用__declspec(dllexport)导出类)
#if defined(DC_EXPORT_STATIC)
	#define ENGINE_DLL 
	#define ENGINE_FUN_DLL
#else
	#if defined(DC_EXPORT_DLL)
		#define ENGINE_DLL DLL_EXPORT
		#define ENGINE_FUN_DLL extern "C" DLL_EXPORT
	#else
			#define ENGINE_DLL DLL_IMPORT
			#define ENGINE_FUN_DLL extern "C"
	#endif
#endif

/*
inline，__inline，__forceinline的详细说明
__inline 关键字建议编译器在合理的情况下内联编译 C 或 C++ 函数。
__inline 语义与 C++ inline 关键字的语义完全相同。
__inline 是一个存储类限定符。 它不影响函数的类型。

格式：
inline _declarator;
__inline _declarator; // Microsoft相关
__forceinline _declarator; // Microsoft相关
__inline与inline等同。inline和__inline通知编译器将该函数的内容拷贝一份放在调用函数的地方，这称之为内联。内联减少了函数调用的开销，但却增加了代码量。__forceinline关键字则是不基于编译器的性能和优化分析而依赖于程序员的判断进行内联，但它也不保证一定内敛，有些情况函数是肯定不能内敛的。要慎用__forceinline，不加考虑的使用它将会造成代码量的膨胀而只得到很小的性能回报，甚至造成性能下降。
1.当使用/clr编译器选项时，如果函数使用了安全属性，则不会进行内联。
2.inline关键字仅用于C++；__inline和__forceinline在C和C++中都可用。
3.类的成员函数如果是在类定义的同时定义的，那么它默认是inline的，否则也可以使用inline指定为内联的。
4.如果函数使用了__forceinline关键字，但实际没有进行内联，将给出level 1警告。

在以下情况中，编译器不进行函数内联，即使是使用__forceinline：
1.函数或其调用者使用/Ob0编译器选项进行编译(Debug模式下的默认选项)。也就是说在Debug模式下，是不会发生函数内联的。
2.函数和其调用者使用不同类型的异常处理。
3.函数具有可变数目的参数。
4.函数使用了在线汇编(即直接在你C/C++代码里加入汇编语言代码)。但使用了编译器关于优化的选项/Og，/Ox，/O1，或/O2的情况除外。
5.是虚函数并且是虚调用。但对虚函数的直接调用可以inline。
6.通过指向该函数的函数指针进行调用。
7.函数被关键字__declspec(naked)修饰。
*/


/*
对齐规则
	一般来说，结构体的对齐规则是先按数据类型自身进行对齐，然后再按整个结构体进行对齐，对齐值必须是2的幂，比如1，2， 4， 8， 16。
如果一个类型按n字节对齐，那么该类型的变量起始地址必须是n的倍数。比如int按四字节对齐，那么int类型的变量起始地址一定是4的倍数，比如0x0012ff60，0x0012ff48等
1 数据自身的对齐
	数据自身的对齐值通常就是数据类型所占的空间大小，比如int类型占四个字节，那么它的对齐值就是4
2 整个结构体的对齐
	整个结构体的对齐值一般是结构体中最大数据类型所占的空间，比如下面这个结构体的对齐值就是8，因为double类型占8个字节。
struct Test2
{
	int i ;
	double d ;
};
3 例
struct Test
{
	char c ;
	int i ;
};
c是char类型，按1个字节对齐
i是int类型，按四个字节对齐，所以在c和i之间实际上空了三个字节。
整个结构体一共是1 + 3（补齐）+ 4 = 8字节

struct Test1
{
	int i ;
	double d ;
	char c ;
};
i是int类型，按4字节对齐
d是double类型，按8字节对齐，所以i和d之间空了4字节
c是char类型，按1字节对齐。
所以整个结构体是 4(i) + 4（补齐）+ 8(d) + 1(c) =  17字节，注意！还没完，整个结构体还没有对齐，因为结构体中空间最大的类型是double，所以整个结构体按8字节对齐，那么最终结果就是17 + 7（补齐） = 24字节。

4 如何查看结构体的对齐值
	使用预处理命令#pragma pack(show)

5 如何修改结构体的对齐值
使用预处理指令
#pragma pack(num)		num是结构体的对齐值，比如下面的例子按四个字节对齐。
#pragma pack(4)
还可以是：
#pragma pack(push,4)	这是将当前对齐方式压栈，然后设置对齐方式为4字节
#pragma pack(pop)		最后pop是恢复保存的对齐值。
再就是设置的对齐值和数据本身的对齐值是取最小值最终对齐的，结构体的整体大小也是取结构中的最大大小的成员和设置对齐值中的较小的一个生效！
*/