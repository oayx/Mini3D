message("当前CMakeLists目录: ${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB_RECURSE ENGINE_EXTERNAL_SHADERLAB_SRC "source/external/shaderlab/*.cpp")

set(external_shaderlab "source/external/shaderlab/*")
source_group("external/shaderlab" REGULAR_EXPRESSION "${external_shaderlab}" )