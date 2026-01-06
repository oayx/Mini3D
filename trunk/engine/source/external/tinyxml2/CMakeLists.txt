message("当前CMakeLists目录: ${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB_RECURSE ENGINE_EXTERNAL_TINYXML2_SRC "source/external/tinyxml2/*.cpp")

set(external_tinyxml2 "source/external/tinyxml2/*")
source_group("external/tinyxml2" REGULAR_EXPRESSION "${externtinyxml2al_tinyxml2}" )