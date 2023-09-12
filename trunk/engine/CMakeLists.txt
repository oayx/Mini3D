# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.
cmake_minimum_required(VERSION 3.4.1)

project(engine)

# -std=c++17
set(CMAKE_CXX_STANDARD 17) 

# find cmake run platform
message(${CMAKE_SYSTEM_NAME})
if("!${CMAKE_SYSTEM_NAME}" STREQUAL "!Windows")
	set(WINDOWS TRUE)
	message("windows platform")
elseif("!${CMAKE_SYSTEM_NAME}" STREQUAL "!Linux")
    set(LINUX TRUE)
	message("linux platform")
elseif("!${CMAKE_SYSTEM_NAME}" STREQUAL "!Android")
    set(ANDROID TRUE)
	message("Android platform")
elseif("!${CMAKE_SYSTEM_NAME}" STREQUAL "!Mac")
    set(MACOSX TRUE)
	message("Mac platform")
elseif("!${CMAKE_SYSTEM_NAME}" STREQUAL "!iOS")
    set(APPLE TRUE)
	message("iOS platform")
elseif("!${CMAKE_SYSTEM_NAME}" STREQUAL "!Emscripten")
	set(WASM TRUE)
	message("WASM platform")
else()
    message(FATAL_ERROR "Unknown platform: ${CMAKE_SYSTEM_NAME}!")
endif()

# macro definitions
if(WINDOWS)
	add_definitions (-DDC_EXPORT_DLL)
	add_definitions (-DDC_OS_WIN10)
	add_definitions (-DDC_PLATFORM_WIN32)
	add_definitions (-DDC_GRAPHICS_API_OPENGL)
	add_definitions (-DIMGUI_DISABLE_WIN32_FUNCTIONS)
	add_definitions (-D_CRT_SECURE_NO_WARNINGS)
	add_definitions (-DWIN32_LEAN_AND_MEAN)
	add_definitions (-DWIN32)
	add_definitions (-D_WIN32)
	add_definitions (-D_WINDOWS)
	add_definitions (-D_DEBUG)
	set(PLATFORM_FOLDER win)
	add_compile_options(/utf-8)
elseif(MACOSX)
	add_definitions (-DDC_GRAPHICS_API_OPENGL)
	add_definitions (-DDC_PLATFORM_MAC)
	add_definitions (-D__APPLE__)
	set(PLATFORM_FOLDER mac)
elseif(APPLE)
	add_definitions (-DDC_GRAPHICS_API_OPENGLES3)
	add_definitions (-DDC_PLATFORM_IOS)
	add_definitions (-D__APPLE__)
	set(PLATFORM_FOLDER ios)
elseif(LINUX)
	add_definitions (-DDC_GRAPHICS_API_OPENGL)
	add_definitions (-DDC_PLATFORM_LINUX)
	add_definitions (-D__linux__)
	set(PLATFORM_FOLDER linux)
elseif(ANDROID)
	add_definitions (-DDC_GRAPHICS_API_OPENGLES3)
	add_definitions (-DDC_PLATFORM_ANDROID)
	add_definitions (-D__ANDROID__)
	set(PLATFORM_FOLDER android)
elseif(WASM)
	add_definitions (-DDC_GRAPHICS_API_OPENGLES3)
	add_definitions (-DDC_PLATFORM_WASM)
	add_definitions (-D__WASM__)
	set(PLATFORM_FOLDER wasm)
else()
	message( FATAL_ERROR "Unsupported platform, CMake will exit" )
endif()

# common macro definitions
set(CMAKE_C_FLAGS
	"${CMAKE_C_FLAGS} -fpermissive")
set(CMAKE_CXX_FLAGS
	"${CMAKE_CXX_FLAGS} -fpermissive")

add_definitions(-DUNICODE)					# Unicode
add_definitions(-D_UNICODE)
add_definitions (-DDC_DEBUG)
add_definitions(-DDC_EXPORT_STATIC)
add_definitions(-DDC_DEBUG_MALLOC)
add_definitions(-DFT2_BUILD_LIBRARY)		# freetype
add_definitions(-DAL_LIBTYPE_STATIC)		# openal
add_definitions(-DAL_ALEXT_PROTOTYPES)

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
set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../bin/${PLATFORM_FOLDER}/${ARCH_DIR})
set (LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../bin/${PLATFORM_FOLDER}/${ARCH_DIR})
if(WINDOWS)
	set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/../bin/${PLATFORM_FOLDER}/${ARCH_DIR})
	set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/../bin/${PLATFORM_FOLDER}/${ARCH_DIR})
	set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/../bin/${PLATFORM_FOLDER}/${ARCH_DIR})
	set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RLEASE ${PROJECT_SOURCE_DIR}/../bin/${PLATFORM_FOLDER}/${ARCH_DIR})
endif()
if(ANDROID)
	#设置生成的so动态库最后输出的路径
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../bin/${PLATFORM_FOLDER}/${ARCH_DIR})
endif()
#------------------------------------源文件编译------------------------------------
# external
include_directories(
	${PROJECT_SOURCE_DIR}/source
	${PROJECT_SOURCE_DIR}/source/external
)

# include makelist
include(${PROJECT_SOURCE_DIR}/source/core/CMakeLists.txt)
include(${PROJECT_SOURCE_DIR}/source/runtime/CMakeLists.txt)
include(${PROJECT_SOURCE_DIR}/source/external/CMakeLists.txt)
include(${PROJECT_SOURCE_DIR}/source/editor/CMakeLists.txt)
include(${PROJECT_SOURCE_DIR}/source/game/CMakeLists.txt)
if(WINDOWS)
	include(${PROJECT_SOURCE_DIR}/source/platform/windows/CMakeLists.txt)
	set(Engine
		${ENGINE_CORE_SRC}
		${ENGINE_RUNTIME_SRC}
		${ENGINE_EXTERNAL_SRC}
		${ENGINE_EDITOR_SRC}
		${ENGINE_PLATFORM_WINDOWS_SRC}
		${ENGINE_GAME_SRC}
    )
elseif(LINUX)
	include(${PROJECT_SOURCE_DIR}/source/platform/linux/CMakeLists.txt)
	set(Engine
		${ENGINE_CORE_SRC}
		${ENGINE_RUNTIME_SRC}
		${ENGINE_EXTERNAL_SRC}
		${ENGINE_EDITOR_SRC}
		${ENGINE_PLATFORM_LINUX_SRC}
		${ENGINE_GAME_SRC}
    )
elseif(ANDROID)
	include(${PROJECT_SOURCE_DIR}/source/platform/android/CMakeLists.txt)
	set(Engine
		${ENGINE_CORE_SRC}
		${ENGINE_RUNTIME_SRC}
		${ENGINE_EXTERNAL_SRC}
		${ENGINE_EDITOR_SRC}
		${ENGINE_PLATFORM_ANDROID_SRC}
		${ENGINE_GAME_SRC}
    )
elseif(WASM)
	include(${PROJECT_SOURCE_DIR}/source/platform/wasm/CMakeLists.txt)
	set(Engine
		${ENGINE_CORE_SRC}
		${ENGINE_RUNTIME_SRC}
		${ENGINE_EXTERNAL_SRC}
		${ENGINE_EDITOR_SRC}
		${ENGINE_PLATFORM_WASM_SRC}
		${ENGINE_GAME_SRC}
    )
endif()

#------------------------------------库文件引用------------------------------------
# include opengles library
include_directories(${PROJECT_SOURCE_DIR}/libs/opengles/include)
file(COPY ${PROJECT_SOURCE_DIR}/libs/opengles/prebuilt/${PLATFORM_FOLDER}/${ARCH_DIR}/ DESTINATION ${EXECUTABLE_OUTPUT_PATH})

# include directx library
if(WINDOWS)
	include_directories(${PROJECT_SOURCE_DIR}/libs/directx/include)
	file(COPY ${PROJECT_SOURCE_DIR}/libs/directx/prebuilt/${PLATFORM_FOLDER}/${ARCH_DIR}/ DESTINATION ${EXECUTABLE_OUTPUT_PATH})
	
	include_directories(${PROJECT_SOURCE_DIR}/libs/dxcompiler/include)
	file(COPY ${PROJECT_SOURCE_DIR}/libs/dxcompiler/prebuilt/${PLATFORM_FOLDER}/${ARCH_DIR}/ DESTINATION ${EXECUTABLE_OUTPUT_PATH})
endif()

# include ffmpeg library
include_directories(${PROJECT_SOURCE_DIR}/libs/ffmpeg/include)
file(COPY ${PROJECT_SOURCE_DIR}/libs/ffmpeg/prebuilt/${PLATFORM_FOLDER}/${ARCH_DIR}/ DESTINATION ${EXECUTABLE_OUTPUT_PATH})

link_directories(${EXECUTABLE_OUTPUT_PATH})

#------------------------------------生成------------------------------------
if(WINDOWS)
	add_library(engine SHARED ${Engine} )
	target_link_libraries(engine
		ShaderConductor.lib
		)
elseif(LINUX)

	#add_library(engine SHARED ${Engine} )

	include_directories(
		${PROJECT_SOURCE_DIR}/../app/test/script
	)
	find_package(Threads)	#pthread
	add_library(engine SHARED ${Engine} )
	target_link_libraries(engine
		${CMAKE_THREAD_LIBS_INIT}
		-ldl
		-lX11				#glfw x11
		)
elseif(ANDROID)
	add_library(engine SHARED ${Engine} )
	target_link_libraries(engine
		android
		log
		OpenSLES			#openal
		EGL					#opengles
		GLESv3				#opengles
		)
elseif(WASM)
	add_library(engine SHARED ${Engine} )
endif()