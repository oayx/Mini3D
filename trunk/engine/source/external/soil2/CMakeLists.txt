message("当前CMakeLists目录: ${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB_RECURSE ENGINE_EXTERNAL_SOIL2_SRC "source/external/soil2/*.c")

set(external_soil2 "source/external/soil2/*")
source_group("external/soil2" REGULAR_EXPRESSION "${external_soil2}" )