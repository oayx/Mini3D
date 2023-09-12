# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.
cmake_minimum_required(VERSION 3.4.1)

project(AllProject)

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
	ADD_DEFINITIONS (-DDC_PLATFORM_WIN)
	set(PLATFORM_FOLDER win)
elseif(MACOSX)
	ADD_DEFINITIONS (-DDC_GRAPHICS_OPENGL)
	ADD_DEFINITIONS (-DDC_PLATFORM_MAC)
	ADD_DEFINITIONS (-D__APPLE__)
	set(PLATFORM_FOLDER mac)
elseif(APPLE)
	ADD_DEFINITIONS (-DDC_GRAPHICS_OPENGLES)
	ADD_DEFINITIONS (-DDC_PLATFORM_IOS)
	set(PLATFORM_FOLDER ios)
elseif(LINUX)
	ADD_DEFINITIONS (-DDC_GRAPHICS_OPENGL)
	ADD_DEFINITIONS (-DDC_PLATFORM_LINUX)
	set(PLATFORM_FOLDER linux)
elseif(ANDROID)
	ADD_DEFINITIONS (-DDC_GRAPHICS_OPENGLES)
	ADD_DEFINITIONS (-DDC_PLATFORM_ANDROID)
	set(PLATFORM_FOLDER android)
else()
	message( FATAL_ERROR "Unsupported platform, CMake will exit" )
endif()
set(CMAKE_C_FLAGS
    "${CMAKE_C_FLAGS} -fdeclspec -DDC_EXPORT_STATIC")
set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -fdeclspec -std=c++17 -DDC_EXPORT_STATIC")

# architecture
if ( CMAKE_SIZEOF_VOID_P EQUAL 8 )
	message("64-bit")
	if(WINDOWS OR LINUX)
		set(ARCH_DIR "x64")
	elseif(ANDROID)
		set(ARCH_DIR "arm64-v8a")
	endif()
elseif ( CMAKE_SIZEOF_VOID_P EQUAL 4 )
	message("32-bit")
	if(WINDOWS OR LINUX)
		set(ARCH_DIR "x86")
	elseif(ANDROID)
		set(ARCH_DIR "armeabi-v7a")
	endif()
else()
	message( FATAL_ERROR "Unsupported architecture, CMake will exit" )
endif()

# exe output path
set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../../bin/${PLATFORM_FOLDER})

#engine
include_directories(
	${PROJECT_SOURCE_DIR}/../engine/source
	${PROJECT_SOURCE_DIR}/../engine/source/external
	)

# build application lib
include_directories(
	${PROJECT_SOURCE_DIR}/script
	)

#engine
add_library(engine SHARED IMPORTED)
if(LINUX)
	set_target_properties(engine PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/../../bin/${PLATFORM_FOLDER}/${ARCH_DIR}/libengine.so)
elseif(ANDROID)
	set_target_properties(engine PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/project.android/app/libs/${ARCH_DIR}/libengine.so)
endif()

# include makelist
include(${PROJECT_SOURCE_DIR}/script/CMakeLists.txt)
set(Application
	${APP_FIGHTER_SRC}
	)

add_library(application SHARED ${Application} )
target_link_libraries(application
	android
	engine
	)