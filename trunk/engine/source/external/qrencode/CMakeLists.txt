message("当前CMakeLists目录: ${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB_RECURSE ENGINE_EXTERNAL_QRENCODE_SRC "source/external/qrencode/*.c")

set(external_qrencode "source/external/qrencode/*")
source_group("external/qrencode" REGULAR_EXPRESSION "${external_qrencode}" )