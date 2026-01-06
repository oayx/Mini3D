message("当前CMakeLists目录: ${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB_RECURSE ENGINE_EXTERNAL_ETCPACK_SRC "source/external/etcpack/*.cxx")

set(external_etcpack "source/external/etcpack/*")
source_group("external/etcpack" REGULAR_EXPRESSION "${external_etcpack}" )