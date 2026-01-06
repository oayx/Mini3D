message("当前CMakeLists目录: ${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB_RECURSE ENGINE_EXTERNAL_YAMLCPP_SRC "source/external/yamlcpp/*.c")

set(external_yamlcpp "source/external/yamlcpp/*")
source_group("external/yamlcpp" REGULAR_EXPRESSION "${external_yamlcpp}" )