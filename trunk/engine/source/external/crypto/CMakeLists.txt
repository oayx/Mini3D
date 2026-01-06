message("当前CMakeLists目录: ${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB_RECURSE ENGINE_EXTERNAL_CRYPTO_SRC "source/external/crypto/*.cpp" "source/external/crypto/*.c")

set(external_crypto "source/external/crypto/*")
source_group("external/crypto" REGULAR_EXPRESSION "${external_crypto}" )