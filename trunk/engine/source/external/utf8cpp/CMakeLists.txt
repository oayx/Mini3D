message("当前CMakeLists目录: ${CMAKE_CURRENT_SOURCE_DIR}")

set(external_utf8cpp "source/external/utf8cpp/*")
source_group("external/utf8cpp" REGULAR_EXPRESSION "${external_utf8cpp}" )