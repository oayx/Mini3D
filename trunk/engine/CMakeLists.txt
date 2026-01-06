# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.
cmake_minimum_required(VERSION 3.5)

project(engine)

# c++版本
set(CMAKE_CXX_STANDARD 17)			# 支持17,20,23,26等
set(CMAKE_CXX_STANDARD_REQUIRED ON)	# 如果编译器不支持C++20会报错
set(CMAKE_CXX_EXTENSIONS OFF)		# 不使用编译器的特有扩展（比如 gnu++20）

# find cmake run platform
message("platform = ${CMAKE_SYSTEM_NAME}")
if("!${CMAKE_SYSTEM_NAME}" STREQUAL "!Windows")
	set(WINDOWS TRUE)
	message("windows platform")
elseif("!${CMAKE_SYSTEM_NAME}" STREQUAL "!Linux")
    set(LINUX TRUE)
	message("linux platform")
elseif("!${CMAKE_SYSTEM_NAME}" STREQUAL "!Android")
    set(ANDROID TRUE)
	message("Android platform")
elseif("!${CMAKE_SYSTEM_NAME}" STREQUAL "!iOS")
    set(iOS TRUE)
	set(APPLE TRUE)
	message("iOS platform")
elseif("!${CMAKE_SYSTEM_NAME}" STREQUAL "!Darwin")
    set(MACOSX TRUE)
	set(APPLE TRUE)
	message("Mac platform")
elseif("!${CMAKE_SYSTEM_NAME}" STREQUAL "!Emscripten")
	set(WASM TRUE)
	message("WASM platform")
else()
    message(FATAL_ERROR "Unknown platform: ${CMAKE_SYSTEM_NAME}!")
endif()

# macro definitions
if(WINDOWS)
	add_definitions (-DDC_EXPORT_DLL)
	add_definitions (-D_DEBUG)
	add_definitions (-DVK_USE_PLATFORM_WIN32_KHR)
	add_definitions (-DIMGUI_DISABLE_WIN32_FUNCTIONS)
	add_definitions (-DDC_GRAPHICS_API_DX11)
	add_definitions (-DDC_COLORSPACE_LINEAR)
	add_definitions (-DDC_PLATFORM_WIN32)

	add_definitions (-DWIN32)
	add_definitions (-D_WIN32)
	add_definitions (-D_WINDOWS)
	add_definitions (-DWIN32_LEAN_AND_MEAN)
	add_definitions (-D_CRT_SECURE_NO_WARNINGS)
elseif(MACOSX)
	add_definitions (-DDC_EXPORT_STATIC)
	add_definitions (-DDC_GRAPHICS_API_OPENGL)
	add_definitions (-DDC_PLATFORM_MAC)

	add_definitions (-D__APPLE__)
elseif(iOS)
	add_definitions (-DDC_EXPORT_STATIC)
	add_definitions (-DDC_GRAPHICS_API_OPENGLES3)
	add_definitions (-DDC_PLATFORM_IOS)

	add_definitions (-D__APPLE__)
	#add_compile_options(-Wno-deprecated-declarations)  # 全局禁用警告
elseif(LINUX)
	add_definitions (-DDC_EXPORT_STATIC)
	add_definitions (-DDC_GRAPHICS_API_OPENGL)
	add_definitions (-DDC_PLATFORM_LINUX)

	add_definitions (-D__linux__)
elseif(ANDROID)
	add_definitions (-DDC_EXPORT_STATIC)
	add_definitions (-DDC_GRAPHICS_API_OPENGLES3)
	add_definitions (-DDC_PLATFORM_ANDROID)

	add_definitions (-D__ANDROID__)
elseif(WASM)
	add_definitions (-DDC_EXPORT_STATIC)
	add_definitions (-DDC_GRAPHICS_API_OPENGLES3)
	add_definitions (-DDC_PLATFORM_WASM)

	add_definitions (-D__WASM__)
else()
	message( FATAL_ERROR "Unsupported platform, CMake will exit" )
endif()

# 通用宏
add_definitions(-DUNICODE)							# Unicode
add_definitions(-D_UNICODE)
add_definitions (-DDC_DEBUG)
#add_definitions(-DDC_DEBUG_MALLOC)
add_definitions(-DFT2_BUILD_LIBRARY)				# freetype
add_definitions(-DAL_LIBTYPE_STATIC)				# openal
add_definitions(-DAL_ALEXT_PROTOTYPES)

# 编译器
message("compiler = ${CMAKE_CXX_COMPILER_ID}")
if(MSVC) # MSVC 原生或 clang-cl（MSVC 模拟器）都会进这里
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        message("MSVC compiler")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        message("Clang-cl compiler(MSVC emulation)")
    else()
        message(FATAL_ERROR "Unknown MSVC-like compiler")
    endif()
    add_compile_options(/utf-8)						# /utf-8 是 Microsoft Visual Studio (MSVC) 编译器的选项，它告诉 MSVC 按 UTF-8 编码 读取源文件（.cpp/.h）
else() # 非 MSVC 环境（GCC/Clang）
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		message("Gun compiler")
        add_compile_options(-fpermissive)			# fpermissive 是GCC/Clang特有的编译选项，用于放宽代码规范检查，但 MSVC编译器不支持此选项
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		message("Clang compiler")
        add_compile_options(-fpermissive)
    endif()
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
message("platform folder = ${PLATFORM_FOLDER}")

# architecture
message("sizeof point = ${CMAKE_SIZEOF_VOID_P}")
if ( CMAKE_SIZEOF_VOID_P EQUAL 8 )
	message("architecture = 64-bit")
	if(WINDOWS)
		set(ARCH_DIR "x64")
		add_definitions (-D_WIN64)
		message("x64")
	elseif(LINUX)
		set(ARCH_DIR "x64")
		add_definitions (-D_LINUX64)
		message("x64")
	elseif(WASM)
		set(ARCH_DIR "x64")
		add_definitions (-D_WASM64)
		message("x64")
	elseif(ANDROID)
		if(ANDROID_ABI STREQUAL "arm64-v8a")
			set(ARCH_DIR "arm64-v8a")
			add_definitions (-D_ARM64)
			message("arm64-v8a")
		elseif(ANDROID_ABI STREQUAL "x86_64")
			set(ARCH_DIR "x86_64")
			add_definitions (-D_X86_64)
			message("android x86_64")
		else()
			message( FATAL_ERROR "Unsupported android architecture, CMake will exit" )
		endif()
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
	message("architecture = 32-bit")
	if(WINDOWS OR LINUX OR WASM)
		set(ARCH_DIR "x86")
		message("x86")
	elseif(ANDROID)
		if(ANDROID_ABI STREQUAL "armeabi-v7a")
			set(ARCH_DIR "armeabi-v7a")
			message("armeabi-v7a")
		elseif(ANDROID_ABI STREQUAL "x86")
			set(ARCH_DIR "x86")
			message("android x86")
		else()
			message( FATAL_ERROR "Unsupported android architecture, CMake will exit" )
		endif()
	elseif(APPLE)
        message(FATAL_ERROR "32-bit builds are deprecated on Apple platforms, CMake will exit")
	endif()
else()
	message( FATAL_ERROR "Unsupported architecture, CMake will exit" )
endif()

#------------------------------------输出目录------------------------------------
# exe output path
set (BIN_OUTPUT_ROOT ${PROJECT_SOURCE_DIR}/../bin)
set (EXECUTABLE_OUTPUT_PATH ${BIN_OUTPUT_ROOT}/${PLATFORM_FOLDER}/${ARCH_DIR})
set (LIBRARY_OUTPUT_PATH ${BIN_OUTPUT_ROOT}/${PLATFORM_FOLDER}/${ARCH_DIR})
if(WINDOWS)
	set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BIN_OUTPUT_ROOT}/${PLATFORM_FOLDER}/${ARCH_DIR})
	set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BIN_OUTPUT_ROOT}/${PLATFORM_FOLDER}/${ARCH_DIR})
	set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${BIN_OUTPUT_ROOT}/${PLATFORM_FOLDER}/${ARCH_DIR})
	set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${BIN_OUTPUT_ROOT}/${PLATFORM_FOLDER}/${ARCH_DIR})
endif()
if(ANDROID)
	#设置生成的so动态库最后输出的路径
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BIN_OUTPUT_ROOT}/${PLATFORM_FOLDER}/${ARCH_DIR})
endif()

#------------------------------------头文件搜索路径------------------------------------
include_directories(
	${PROJECT_SOURCE_DIR}/source
	${PROJECT_SOURCE_DIR}/source/external
)

#------------------------------------源文件编译------------------------------------
# include makelist
include(${PROJECT_SOURCE_DIR}/source/core/CMakeLists.txt)
include(${PROJECT_SOURCE_DIR}/source/runtime/CMakeLists.txt)
include(${PROJECT_SOURCE_DIR}/source/external/CMakeLists.txt)
include(${PROJECT_SOURCE_DIR}/source/editor/CMakeLists.txt)
include(${PROJECT_SOURCE_DIR}/source/game/CMakeLists.txt)
set(ENGINE_COMMON_SRC
	${ENGINE_CORE_SRC}
	${ENGINE_RUNTIME_SRC}
	${ENGINE_EXTERNAL_SRC}
	${ENGINE_EDITOR_SRC}
	${ENGINE_GAME_SRC}
)
if(WINDOWS)
	include(${PROJECT_SOURCE_DIR}/source/platform/windows/CMakeLists.txt)
	set(Engine
		${ENGINE_COMMON_SRC}
		${ENGINE_PLATFORM_WINDOWS_SRC}
    )
elseif(LINUX)
	include(${PROJECT_SOURCE_DIR}/source/platform/linux/CMakeLists.txt)
	set(Engine
		${ENGINE_COMMON_SRC}
		${ENGINE_PLATFORM_LINUX_SRC}
    )
elseif(ANDROID)
	include(${PROJECT_SOURCE_DIR}/source/platform/android/CMakeLists.txt)
	set(Engine
		${ENGINE_COMMON_SRC}
		${ENGINE_PLATFORM_ANDROID_SRC}
    )
elseif(WASM)
	include(${PROJECT_SOURCE_DIR}/source/platform/wasm/CMakeLists.txt)
	set(Engine
		${ENGINE_COMMON_SRC}
		${ENGINE_PLATFORM_WASM_SRC}
    )
elseif(MACOSX)
	include(${PROJECT_SOURCE_DIR}/source/platform/mac/CMakeLists.txt)
	set(Engine
		${ENGINE_COMMON_SRC}
		${ENGINE_PLATFORM_MAC_SRC}
    )
elseif(iOS)
	include(${PROJECT_SOURCE_DIR}/source/platform/ios/CMakeLists.txt)
	set(Engine
		${ENGINE_COMMON_SRC}
		${ENGINE_PLATFORM_IOS_SRC}
    )
else()
	message( FATAL_ERROR "Unsupported platform cmakelist, CMake will exit" )
endif()

#------------------------------------库文件引用------------------------------------
# include opengles library
if(NOT WASM)#wasm使用自带的
	include_directories(${PROJECT_SOURCE_DIR}/libs/opengles/include)
endif()
if(WINDOWS)
	file(COPY ${PROJECT_SOURCE_DIR}/libs/opengles/prebuilt/${PLATFORM_FOLDER}/${ARCH_DIR}/ DESTINATION ${EXECUTABLE_OUTPUT_PATH})
endif()

# include directx library
if(WINDOWS)
	include_directories(${PROJECT_SOURCE_DIR}/libs/directx/include)
	file(COPY ${PROJECT_SOURCE_DIR}/libs/directx/prebuilt/${PLATFORM_FOLDER}/${ARCH_DIR}/ DESTINATION ${EXECUTABLE_OUTPUT_PATH})
	
	if(ARCH_DIR STREQUAL "x64")
		include_directories(${PROJECT_SOURCE_DIR}/libs/dxcompiler/include)
		file(COPY ${PROJECT_SOURCE_DIR}/libs/dxcompiler/prebuilt/${PLATFORM_FOLDER}/${ARCH_DIR}/ DESTINATION ${EXECUTABLE_OUTPUT_PATH})
	endif()
endif()

# include ffmpeg library
include_directories(${PROJECT_SOURCE_DIR}/libs/ffmpeg/include)
if(NOT WASM AND NOT APPLE)
	file(COPY ${PROJECT_SOURCE_DIR}/libs/ffmpeg/prebuilt/${PLATFORM_FOLDER}/${ARCH_DIR}/ DESTINATION ${EXECUTABLE_OUTPUT_PATH})
endif()

link_directories(${EXECUTABLE_OUTPUT_PATH})

#------------------------------------打印路径------------------------------------
# 打印编译器隐式包含路径
message(STATUS "Implicit include directories:\n ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES}")

# 打印当前目录显式添加的包含路径
get_directory_property(dir_inc_dirs INCLUDE_DIRECTORIES)
message(STATUS "Current directory include directories:\n ${dir_inc_dirs}")

if(WASM)
	execute_process(
	  COMMAND ${CMAKE_CXX_COMPILER} --print-search-dirs
	  OUTPUT_VARIABLE EMCC_SEARCH_DIRS
	)
	message(STATUS "Emscripten include paths:\n${EMCC_SEARCH_DIRS}")
endif()

#------------------------------------生成------------------------------------
if(WINDOWS)
	add_library(engine SHARED ${Engine} )
	if(ARCH_DIR STREQUAL "x64")
		target_link_libraries(engine
			ShaderConductor.lib
			)
	endif()
elseif(LINUX)
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
	add_library(engine STATIC ${Engine} )
elseif(MACOSX)
	add_library(engine STATIC ${Engine} )
elseif(iOS)
	add_library(engine STATIC ${Engine} )
	target_compile_options(engine PRIVATE -Wno-deprecated-declarations)
else()
	message( FATAL_ERROR "Unsupported build target, CMake will exit" )
endif()