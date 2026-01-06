message("当前CMakeLists目录: ${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB_RECURSE ENGINE_EXTERNAL_JSON_SRC "source/external/json/*.cpp")

set(external_json "source/external/json/*")
source_group("external/json" REGULAR_EXPRESSION "${external_json}" )