message("当前CMakeLists目录: ${CMAKE_CURRENT_SOURCE_DIR}")

if(WINDOWS)
file(GLOB_RECURSE ENGINE_EXTERNAL_TINYSTL_SRC "source/external/tinystl/*.cpp")
else()
file(GLOB_RECURSE ENGINE_EXTERNAL_TINYSTL_SRC "source/external/tinystl/stl/*.cpp")
endif()

if(WINDOWS)
set(external_tinystl "source/external/tinystl/*")
else()
set(external_tinystl "source/external/tinystl/stl/*")
endif()
source_group("external/tinystl" REGULAR_EXPRESSION "${external_tinystl}" )