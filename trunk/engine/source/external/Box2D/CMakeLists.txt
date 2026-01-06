message("当前CMakeLists目录: ${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB_RECURSE ENGINE_EXTERNAL_BOX2D_SRC "source/external/Box2D/*.cpp")

set(external_box2d "source/external/Box2D/*")
source_group("external/Box2D" REGULAR_EXPRESSION "${external_box2d}" )