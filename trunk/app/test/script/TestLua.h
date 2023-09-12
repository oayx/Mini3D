 #pragma once

#include "TestBase.h"

DC_BEGIN_NAMESPACE
static int lua_Add(lua_State *L)
{
	int a = lua_tonumber(L, 1);//获得传入的参数
	int b = lua_tonumber(L, 2);
	int sum = a + b;
	lua_pushnumber(L, sum);//把结果压入堆栈
	return 1;//返回值个数
}
class TestLua : public TestBase
{
	typedef TestBase base;

public:
	virtual void OnEnter(int child_unit_index)override
	{
		base::OnEnter(child_unit_index);
		this->Test();
	}
	virtual void OnLeave()override 
	{
		base::OnLeave();
	}
	virtual String GetTitle()override { return "Lua"; }

private:
	void Test()
	{
		//字符串
		bool ret = LuaManager::DoString("print('ddd')");

		//文件
		ret = LuaManager::DoFile(Resource::GetFullDataPath("script/Main.lua"));

		//lua调用c
		lua_register(LuaManager::GetState(), "lua_Add", lua_Add);

		//函数
		int i;
		char ch[] = "123";
		ret = LuaManager::DoFun("InitUI", "s|is", ch, &i, ch);
		Debuger::Log("%d,%s", i, ch);

		//全局变量
		lua_getglobal(LuaManager::GetState(), "g_var");
		const char* ret_str = lua_tostring(LuaManager::GetState(), -1);
		Debuger::Log("global:%s", ret_str);
	}
};
DC_END_NAMESPACE