message("当前CMakeLists目录: ${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB_RECURSE ENGINE_EXTERNAL_SPINE_SRC "source/external/spine/*.cpp")

set(external_spine "source/external/spine/*")
source_group("external/spine" REGULAR_EXPRESSION "${external_spine}" )