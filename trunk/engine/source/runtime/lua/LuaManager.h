 
/*****************************************************************************************************/
// @author hannibal
// @date   2020/12/04
// @brief  lua
/*****************************************************************************************************/
#pragma once

#include "core/Object.h"

extern "C"
{
	#include "external/lua/lua/lua.h"
	#include "external/lua/lua/lualib.h"
	#include "external/lua/lua/lauxlib.h"
};

DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL LuaManager Final : public object
{
	friend class Application;
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(LuaManager);
	BEGIN_REFECTION_TYPE(LuaManager)
	END_FINAL_REFECTION_TYPE;

private:
	static void Initialize();
	static void Destroy();

public:	
	static lua_State* GetState() { return _luaState; }

	static bool DoFile(const String &file);
	static bool DoString(const String &str);
	static bool DoFun(const String& fun, const char* param_format, ...);

private:
	static lua_State* _luaState;
};
DC_END_NAMESPACE
