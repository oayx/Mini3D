# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.
cmake_minimum_required(VERSION 3.4.1)

project(fighter)

# -std=c++17
set(CMAKE_CXX_STANDARD 17)

# find cmake run platform
message(${CMAKE_SYSTEM_NAME})
if("!${CMAKE_SYSTEM_NAME}" STREQUAL "!Windows")
    set(WINDOWS TRUE)
elseif("!${CMAKE_SYSTEM_NAME}" STREQUAL "!Linux")
    set(LINUX TRUE)
elseif("!${CMAKE_SYSTEM_NAME}" STREQUAL "!Android")
    set(ANDROID TRUE)
elseif("!${CMAKE_SYSTEM_NAME}" STREQUAL "!Mac")
    set(MACOSX TRUE)
elseif("!${CMAKE_SYSTEM_NAME}" STREQUAL "!iOS")
    set(APPLE TRUE)
else()
    message(FATAL_ERROR "Unknown platform: ${CMAKE_SYSTEM_NAME}!")
endif()

# macro definitions
if(WINDOWS)
	add_definitions (-DDC_GRAPHICS_API_DX11)
	add_definitions (-DDC_PLATFORM_WIN32)
	set(PLATFORM_FOLDER win)
elseif(MACOSX)
	add_definitions (-DDC_GRAPHICS_API_OPENGL)
	add_definitions (-DDC_PLATFORM_MAC)
	set(PLATFORM_FOLDER mac)
elseif(APPLE)
	add_definitions (-DDC_GRAPHICS_API_OPENGLES3)
	add_definitions (-DDC_PLATFORM_IOS)
	set(PLATFORM_FOLDER ios)
elseif(LINUX)
	add_definitions (-DDC_GRAPHICS_API_OPENGL)
	add_definitions (-DDC_PLATFORM_LINUX)
	set(PLATFORM_FOLDER linux)
elseif(ANDROID)
	add_definitions (-DDC_GRAPHICS_API_OPENGLES3)
	add_definitions (-DDC_PLATFORM_ANDROID)
	set(PLATFORM_FOLDER android)
else()
	message( FATAL_ERROR "Unsupported platform, CMake will exit" )
endif()

# common macro definitions
set(CMAKE_C_FLAGS
    "${CMAKE_C_FLAGS} ")
set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} ")
	
add_definitions(-DUNICODE)					# Unicode
add_definitions(-D_UNICODE)
add_definitions(-DDC_DEBUG)
add_definitions(-DDC_EXPORT_STATIC)
add_definitions(-DDC_DEBUG_MALLOC)

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
	endif()
elseif ( CMAKE_SIZEOF_VOID_P EQUAL 4 )
	message("32-bit")
	if(WINDOWS OR LINUX OR WASM)
		set(ARCH_DIR "x86")
	elseif(ANDROID)
		set(ARCH_DIR "armeabi-v7a")
	endif()
else()
	message( FATAL_ERROR "Unsupported architecture, CMake will exit" )
endif()

# exe output path
set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../../bin/${PLATFORM_FOLDER}/${ARCH_DIR})
set (LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../../bin/${PLATFORM_FOLDER}/${ARCH_DIR})
if(WINDOWS)
	set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/../../bin/${PLATFORM_FOLDER}/${ARCH_DIR})
	set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/../../bin/${PLATFORM_FOLDER}/${ARCH_DIR})
endif()

#------------------------------------源文件编译------------------------------------
# engine
include_directories(
	${PROJECT_SOURCE_DIR}/../../engine/source
	${PROJECT_SOURCE_DIR}/../../engine/source/external
	)
if(LINUX)
	link_directories(${EXECUTABLE_OUTPUT_PATH})
elseif(ANDROID)
	link_directories(${PROJECT_SOURCE_DIR}/../../bin/${PLATFORM_FOLDER}/${ARCH_DIR})
endif()

# application
include_directories(
	${PROJECT_SOURCE_DIR}/script
	)

file(GLOB_RECURSE Application ${PROJECT_SOURCE_DIR}/script/*.cpp)

if(LINUX)
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
endif()