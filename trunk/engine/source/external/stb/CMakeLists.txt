message("当前CMakeLists目录: ${CMAKE_CURRENT_SOURCE_DIR}")

set(external_stb "source/external/stb/*")
source_group("external/stb" REGULAR_EXPRESSION "${external_stb}" )