 
#include "LuaManager.h"
#include "runtime/component/GameObject.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_REFECTION_TYPE(LuaManager);
void LuaManager::Initialize()
{
	_luaState = luaL_newstate();
	luaL_openlibs(_luaState);
}
void LuaManager::Destroy()
{
	if (_luaState != nullptr)
	{
		lua_close(_luaState);
		_luaState = nullptr;
	}
}	
bool LuaManager::DoFile(const String &file)
{
	if (file.IsEmpty())
	{
		return false;
	}
	if (luaL_dofile(_luaState, file.c_str()) != 0)
	{
		Debuger::Error("LuaManager::DoFile - file : %s", file.c_str());
		return false;
	}
	return true;
}
bool LuaManager::DoString(const String &str)
{
	if (str.IsEmpty())
	{
		return false;
	}
	if (luaL_dostring(_luaState, str.c_str()) != 0)
	{
		Debuger::Error("LuaManager::DoString - str : %s", str.c_str());
		return false;
	}
	return true;
}
bool LuaManager::DoFun(const String& fun, const char* paramFormat, ...)
{
	if (fun.IsEmpty())
	{
		Debuger::Error("LuaManager::DoFun - function name is empty");
		return false;
	}

	lua_getglobal(_luaState, fun.c_str());
	if (!lua_isfunction(_luaState, -1)) // 不是函数名
	{
		Debuger::Error("LuaManager::DoFun - function is error:%s", fun.c_str());
		return false;
	}

	bool result = true;
	bool hasParam = paramFormat ? true : false;
	bool hasRet = hasParam;

	char buf[256] = { 0 };
	va_list param;
	if (hasParam || hasRet)
	{
		va_start(param, paramFormat);
		vsprintf(buf, paramFormat, param);
	}

	do 
	{
		//处理输入参数
		int arg_num = 0;
		if (hasParam)
		{
			while (*paramFormat)
			{
				switch (*paramFormat++)
				{
				case 'i': // int argument
				{
					int val = va_arg(param, int);
					lua_pushnumber(_luaState, LUA_NUMBER(val));
				}
				break;
				case 'u': // unsigned int argument
				{
					unsigned int val = va_arg(param, unsigned int);
					lua_pushnumber(_luaState, LUA_NUMBER(val));
				}
				break;
				case 'f': // float argument
				{
					float val = (float)va_arg(param, double);
					lua_pushnumber(_luaState, LUA_NUMBER(val));
				}
				break;
				case 'p': // void* argument
				{
					void* val = va_arg(param, void*);
					if (val)
						lua_pushlightuserdata(_luaState, val);
					else
						lua_pushnil(_luaState);
				}
				break;
				case 's': // string argument
				{
					char* val = va_arg(param, char*);
					lua_pushstring(_luaState, val);
				}
				break;
				case 'b': // byte argument
				{
					char* szByte = va_arg(param, char*);
					unsigned int unLen = va_arg(param, unsigned int);
					lua_pushlstring(_luaState, szByte, unLen);
				}
				break;
				case '|': // input and output param split
					goto ret_proc;
				default:
					Debuger::Error("LuaManager::DoFun - invalid param");
					result = false;
					goto ret_proc;
				}
				arg_num++;
			}
		}

ret_proc:
		if (!result)break;

		//执行函数
		int retNum = 0;
		if (hasRet)retNum = (int)strlen(paramFormat);
		if (lua_pcall(_luaState, arg_num, retNum, -1 - arg_num - 1) != 0)
		{
			const char* szErrInfo = lua_tostring(_luaState, -1);
			Debuger::Error("LuaManager::DoFun - lua_pcall error : %s", szErrInfo);
			result = false;
			break;
		}

		//处理返回值
		if (hasRet)
		{
			retNum = -retNum;
			int ret_count = 0;
			while (*paramFormat && result)
			{
				ret_count++;
				switch (*paramFormat++)
				{
				case 'i':
				case 'u':
				{
					int t = lua_type(_luaState, retNum);
					if (t != LUA_TNUMBER)
					{
						Debuger::Error("LuaManager::DoFun - function(%s) param(%d) is invalid\n", fun.c_str(), ret_count);
					}
					*va_arg(param, int*) = (int)lua_tonumber(_luaState, retNum);
				}
				break;
				case 'f':
				{
					if (lua_type(_luaState, retNum) != LUA_TNUMBER)
					{
						Debuger::Error("LuaManager::DoFun - function(%s) param(%d) is invalid\n", fun.c_str(), ret_count);
					}
					*va_arg(param, float*) = (float)lua_tonumber(_luaState, retNum);
				}
				break;
				case 'p':
				{
					if (lua_type(_luaState, retNum) != LUA_TLIGHTUSERDATA)
					{
						Debuger::Error("LuaManager::DoFun - function(%s) param(%d) is invalid\n", fun.c_str(), ret_count);
					}
					*va_arg(param, int**) = (int*)lua_touserdata(_luaState, retNum);
				}
				break;
				case 's':
				{
					if (lua_type(_luaState, retNum) != LUA_TSTRING)
					{
						Debuger::Error("LuaManager::DoFun - function(%s) param(%d) is invalid\n", fun.c_str(), ret_count);
					}
					strcpy(va_arg(param, char*), lua_tostring(_luaState, retNum));
				}
				break;
				default:
					Debuger::Error("LuaManager::DoFun - function(%s) param(%d) is invalid\n", fun.c_str(), ret_count);
					result = false;
					break;
				}
				retNum++;
			}
		}
	} while (false);

	if (hasParam || hasRet)
		va_end(param);

	lua_settop(_luaState, 0);
	return result;
}
DC_END_NAMESPACE

/*
（1）lua 和 C++之间的交互的基本知识：
	lua 和 C++ 之间的数据交互通过堆栈进行，栈中的数据通过索引值进行定位，（栈就像是一个容器一样，放进去的东西都要有标号）
	其中栈顶是-1，栈底是1，也就是第 1 个入栈的在栈底；也可以这么说：正数表示相对于栈底的位置（位移），负数表示相对于栈顶的位置（位移）；
（2）计算和清空栈中元素的操作：
	1、函数lua_gettop()
		用于返回栈中元素的个数，同时也是栈顶元素的索引，因为栈底是1，所以栈中有多少个元素，栈顶索引就是多少；
	2、函数lua_settop()
		函数原型：void lua_settop(lua_State *L , int index);
		用于把堆栈的栈顶索引设置为指定的数值，比如说，一个栈原来有8个元素，调用函数设置index为7，就是把堆栈的元素数设置为7，也就是删掉一个元素，而且是栈顶元素；这个是用的正数，也就是相对于栈底元素设置的；如果是相对于栈顶元素，则要求用负值；也就是说如果设置索引为-2（index = -2），也相当于删除掉栈顶元素；呵呵，画个图就很方便了；为了说明方便，干脆设置一个宏：
		#define lua_pop(L,n) lua_settop(L,-(n)-1)
		这里的n是相对于栈顶的第几个元素，主要是为了理解；后面的lua_settop(L,-(n)-1) 用的就是相对于栈顶位移的负数表示；
	3、lua_pushvalue()
		函数原型：void lua_pushvalue (lua_State *L, int index);
		英文原意：Pushes a copy of the element at the given valid index onto the stack.
		下面是一个例子，栈的初始状态为10 20 30 40 50 *（从栈底到栈顶，“*”标识为栈顶）
		lua_pushvalue(L, 3)    --> 10 20 30 40 50 30*
		lua_pushvalue(L,3)是取得原来栈中的第三个元素，压到栈顶；
	4、lua_remove()
		void lua_remove(lua_State *L, int index);
		lua_remove删除给定索引的元素，并将这一索引之上的元素来填补空缺；
		下面是一个例子，栈的初始状态为10 20 30 40 50 *（从栈底到栈顶，“*”标识为栈顶）
		lua_remove(L, -3)      --> 10 20 40 50*
		下面是一个例子，栈的初始状态为10 20 30 40 50 *（从栈底到栈顶，“*”标识为栈顶）
		lua_settop(L, -3)      --> 10 20 30 *
		lua_settop(L,  6)      --> 10 20 30 nil nil nil *
	5、lua_replace
		void    lua_replace    (lua_State* L, int index);
		Lua_replace将栈顶元素压入指定位置而不移动任何元素（因此指定位置的元素的值被替换）。
		下面是一个例子，栈的初始状态为10 20 30 40 50 *（从栈底到栈顶，“*”标识为栈顶）
		lua_replace(L, 2)      --> 10 50 30 40 *    //把50替换到索引的位置，同时去掉栈顶元素
	6、lua_insert
		void  lua_insert (lua_State *L, int idx);   //弹出栈顶元素，并插入索引idx位置
（3）其他操作方法
	lua_pushcclosure(L, func, 0)	// 创建并压入一个闭包
	lua_createtable(L, 0, 0)        // 新建并压入一个表
	lua_pushnumber(L, 343)			// 压入一个数字
	lua_pushstring(L, “mystr”)	// 压入一个字符串
（4）C中加载lua时一些函数的用法
	lua_getgobal ------void lua_getglobal (lua_State *L, const char *name);把全局的name的值压到栈顶。
	lua_is***(lua_State *L,int index) 检查变量是不是某个类型，index指示变量的顺序，栈顶为-1。
	lua_to***(lua_State *L,int index) 获取栈中的变量，然后转换为某个指定的类型，并返回。
	lua_close()   销毁所有在指定的Lua State上的所有对象，同时释放所有该State使用的动态分配的空间。
*/