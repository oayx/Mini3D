message("当前CMakeLists目录: ${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB_RECURSE ENGINE_EXTERNAL_FREETYPE_SRC "source/external/freetype/*.c")

set(external_freetype "source/external/freetype/*")
source_group("external/freetype" REGULAR_EXPRESSION "${external_freetype}" )