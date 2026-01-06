message("当前CMakeLists目录: ${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB_RECURSE ENGINE_EXTERNAL_ASTC_SRC "source/external/astc/*.cpp")

set(external_astc "source/external/astc/*")
source_group("external/astc" REGULAR_EXPRESSION "${external_astc}" )