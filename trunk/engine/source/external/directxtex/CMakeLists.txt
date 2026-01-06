message("当前CMakeLists目录: ${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB_RECURSE ENGINE_EXTERNAL_DIRECTXTEX_SRC "source/external/directxtex/*.cpp")

set(external_directxtex "source/external/directxtex/*")
source_group("external/directxtex" REGULAR_EXPRESSION "${external_directxtex}" )