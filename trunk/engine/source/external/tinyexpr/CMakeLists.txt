message("当前CMakeLists目录: ${CMAKE_CURRENT_SOURCE_DIR}")

set(ENGINE_EXTERNAL_TINYEXPR_SRC
	source/external/tinyexpr/tinyexpr.c
)

set(external_tinyexpr "source/external/tinyexpr/*")
source_group("external/tinyexpr" REGULAR_EXPRESSION "${external_tinyexpr}" )