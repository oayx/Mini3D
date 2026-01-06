message("当前CMakeLists目录: ${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB_RECURSE ENGINE_EXTERNAL_FORMAT_SRC "source/external/format/*.cc")

set(external_format "source/external/format/*")
source_group("external/format" REGULAR_EXPRESSION "${external_format}" )