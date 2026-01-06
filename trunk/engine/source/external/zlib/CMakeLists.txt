message("当前CMakeLists目录: ${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB_RECURSE ENGINE_EXTERNAL_ZLIB_SRC "source/external/zlib/*.c")

set(external_zlib "source/external/zlib/*")
source_group("external/zlib" REGULAR_EXPRESSION "${external_zlib}" )