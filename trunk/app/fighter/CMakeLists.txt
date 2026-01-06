# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.
cmake_minimum_required(VERSION 3.5)

project(fighter)

# c++版本
set(CMAKE_CXX_STANDARD 17)

# find cmake run platform
message(${CMAKE_SYSTEM_NAME})
if("!${CMAKE_SYSTEM_NAME}" STREQUAL "!Windows")
    set(WINDOWS TRUE)
elseif("!${CMAKE_SYSTEM_NAME}" STREQUAL "!Linux")
    set(LINUX TRUE)
elseif("!${CMAKE_SYSTEM_NAME}" STREQUAL "!Android")
    set(ANDROID TRUE)
elseif("!${CMAKE_SYSTEM_NAME}" STREQUAL "!iOS")
    set(iOS TRUE)
elseif("!${CMAKE_SYSTEM_NAME}" STREQUAL "!Darwin")
    set(MACOSX TRUE)
elseif("!${CMAKE_SYSTEM_NAME}" STREQUAL "!Emscripten")
	set(WASM TRUE)
else()
    message(FATAL_ERROR "Unknown platform: ${CMAKE_SYSTEM_NAME}!")
endif()

# macro definitions
if(WINDOWS)
	add_definitions (-DDC_GRAPHICS_API_DX11)
	add_definitions (-DDC_PLATFORM_WIN32)
elseif(MACOSX)
	add_definitions (-DDC_GRAPHICS_API_OPENGL)
	add_definitions (-DDC_PLATFORM_MAC)
elseif(iOS)
	add_definitions (-DDC_GRAPHICS_API_OPENGLES3)
	add_definitions (-DDC_PLATFORM_IOS)
elseif(LINUX)
	add_definitions (-DDC_GRAPHICS_API_OPENGL)
	add_definitions (-DDC_PLATFORM_LINUX)
elseif(ANDROID)
	add_definitions (-DDC_GRAPHICS_API_OPENGLES3)
	add_definitions (-DDC_PLATFORM_ANDROID)
elseif(WASM)
	add_definitions (-DDC_GRAPHICS_API_OPENGLES3)
	add_definitions (-DDC_PLATFORM_WASM)
else()
	message( FATAL_ERROR "Unsupported platform, CMake will exit" )
endif()

# common macro definitions
add_definitions(-DUNICODE)					# Unicode
add_definitions(-D_UNICODE)
add_definitions(-DDC_DEBUG)
# add_definitions(-DDC_DEBUG_MALLOC)

# 编译器
if(MSVC) # MSVC 原生或 clang-cl（MSVC 模拟器）都会进这里
	message("MSVC or Clang-cl compiler")
    add_compile_options(/utf-8)
else() # 非 MSVC 环境（GCC/Clang）
endif()

# architecture
if ( CMAKE_SIZEOF_VOID_P EQUAL 8 )
	message("64-bit")
	if(WINDOWS)
		set(ARCH_DIR "x64")
		add_definitions (-D_WIN64)
	elseif(LINUX)
		set(ARCH_DIR "x64")
		add_definitions (-D_LINUX64)
	elseif(WASM)
		set(ARCH_DIR "x64")
		add_definitions (-D_WASM64)
	elseif(ANDROID)
		set(ARCH_DIR "arm64-v8a")
	elseif(APPLE)
        if(MACOSX)
            # 检测Apple Silicon（即M1、M2等基于ARM架构的芯片）
            if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64")
                set(ARCH_DIR "arm64")
                add_definitions (-D_ARM64)
                message("Apple Silicon (arm64)")
            else()
                set(ARCH_DIR "x64")
                add_definitions (-D_X64)
                message("Intel x64")
            endif()
        elseif(iOS)
            set(ARCH_DIR "arm64")
            add_definitions (-D_ARM64)
            message("iOS arm64")
        endif()
        # 通用Apple架构定义
        add_definitions (-D__APPLE64__)
	endif()
elseif ( CMAKE_SIZEOF_VOID_P EQUAL 4 )
	message("32-bit")
	if(WINDOWS OR LINUX OR WASM)
		set(ARCH_DIR "x86")
	elseif(ANDROID)
		set(ARCH_DIR "armeabi-v7a")
	elseif(APPLE)
        message(FATAL_ERROR "32-bit builds are deprecated on Apple platforms, CMake will exit")
	endif()
else()
	message( FATAL_ERROR "Unsupported architecture, CMake will exit" )
endif()

# 平台相关目录
if(WINDOWS)
	set(PLATFORM_FOLDER win)
elseif(MACOSX)
	set(PLATFORM_FOLDER mac)
elseif(iOS)
	set(PLATFORM_FOLDER iOS)
elseif(LINUX)
	set(PLATFORM_FOLDER linux)
elseif(ANDROID)
	set(PLATFORM_FOLDER android)
elseif(WASM)
	set(PLATFORM_FOLDER wasm)
endif()

#------------------------------------输出目录------------------------------------
# exe output path
set (BIN_OUTPUT_ROOT ${PROJECT_SOURCE_DIR}/../../bin)
set (EXECUTABLE_OUTPUT_PATH ${BIN_OUTPUT_ROOT}/${PLATFORM_FOLDER}/${ARCH_DIR})
set (LIBRARY_OUTPUT_PATH ${BIN_OUTPUT_ROOT}/${PLATFORM_FOLDER}/${ARCH_DIR})
if(WINDOWS)
	set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BIN_OUTPUT_ROOT}/${PLATFORM_FOLDER}/${ARCH_DIR})
	set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BIN_OUTPUT_ROOT}/${PLATFORM_FOLDER}/${ARCH_DIR})
endif()

#------------------------------------头文件搜索路径------------------------------------
# application
include_directories(
	${PROJECT_SOURCE_DIR}/script
	)

#------------------------------------源文件编译------------------------------------
# engine
include_directories(
	${PROJECT_SOURCE_DIR}/../../engine/source
	${PROJECT_SOURCE_DIR}/../../engine/source/external
	)
link_directories(${EXECUTABLE_OUTPUT_PATH})

file(GLOB_RECURSE Application ${PROJECT_SOURCE_DIR}/script/*.cpp)

#------------------------------------生成------------------------------------
if(WINDOWS)
	add_executable(fighter ${Application})
	target_link_libraries(fighter
		engine.dll
	)
elseif(LINUX)
	add_executable(fighter ${Application})
	target_link_libraries(fighter
		libengine.so
	)
elseif(ANDROID)
	add_library(application SHARED ${Application} )
	target_link_libraries(application
		android
		libengine.so
		)
elseif(WASM)
	add_executable(fighter ${Application} )
	# 关键链接选项
	target_link_libraries(fighter PRIVATE
		libengine.a
		"-s USE_GLFW=3"
		"-s FULL_ES3=1"
		"-s USE_WEBGL2=1"
		"-s MIN_WEBGL_VERSION=2"
		"-s MAX_WEBGL_VERSION=2"
		"-s INITIAL_MEMORY=256MB"
		"-s MAXIMUM_MEMORY=2GB"
		"-s ALLOW_MEMORY_GROWTH=1"
		"--preload-file ${PROJECT_SOURCE_DIR}/../../template/data/assets@/"
		"--preload-file ${PROJECT_SOURCE_DIR}/../../bin/assets@/assets"
	)
	# 生成JS胶水代码
	set_target_properties(fighter PROPERTIES
		SUFFIX ".html"  # 生成 HTML 模板
	)
elseif(MACOSX)
	add_executable(fighter ${Application})

	find_package(OpenAL REQUIRED)
	find_package(Iconv REQUIRED)
	target_link_libraries(fighter
		libengine.a
		OpenAL::OpenAL
		Iconv::Iconv
		"-framework CoreFoundation"  # 直接传递框架
		"-framework IOKit"
		"-framework AppKit"
		"-framework Cocoa"
		)
endif()