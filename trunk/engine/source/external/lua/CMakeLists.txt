message("当前CMakeLists目录: ${CMAKE_CURRENT_SOURCE_DIR}")

set(ENGINE_EXTERNAL_LUA_SRC
	source/external/lua/lua/lapi.c
	source/external/lua/lua/lauxlib.c
	source/external/lua/lua/lbaselib.c
	source/external/lua/lua/lbitlib.c
	source/external/lua/lua/lcode.c
	source/external/lua/lua/lcorolib.c
	source/external/lua/lua/lctype.c
	source/external/lua/lua/ldblib.c
	source/external/lua/lua/ldebug.c
	source/external/lua/lua/ldo.c
	source/external/lua/lua/ldump.c
	source/external/lua/lua/lfunc.c
	source/external/lua/lua/lgc.c
	source/external/lua/lua/linit.c
	source/external/lua/lua/liolib.c
	source/external/lua/lua/llex.c
	source/external/lua/lua/lmathlib.c
	source/external/lua/lua/lmem.c
	source/external/lua/lua/loadlib.c
	source/external/lua/lua/lobject.c
	source/external/lua/lua/lopcodes.c
	source/external/lua/lua/loslib.c
	source/external/lua/lua/lparser.c
	source/external/lua/lua/lstate.c
	source/external/lua/lua/lstring.c
	source/external/lua/lua/lstrlib.c
	source/external/lua/lua/ltable.c
	source/external/lua/lua/ltablib.c
	source/external/lua/lua/ltm.c
	source/external/lua/lua/lundump.c
	source/external/lua/lua/lutf8lib.c
	source/external/lua/lua/lvm.c
	source/external/lua/lua/lzio.c
)

set(external_lua "source/external/lua/*")
source_group("external/lua" REGULAR_EXPRESSION "${external_lua}" )