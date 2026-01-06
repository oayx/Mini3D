message("当前CMakeLists目录: ${CMAKE_CURRENT_SOURCE_DIR}")

include_directories(
	source/external/openal/include
	source/external/openal/OpenAL32/Include
)
if(WINDOWS)
	include_directories(source/external/openal/win)
elseif(LINUX)
	include_directories(source/external/openal/linux)
elseif(ANDROID)
	include_directories(source/external/openal/android)
endif()

set(OPENAL_COMMON_SRC 
    source/external/openal/Alc/backends/loopback.c
    source/external/openal/Alc/backends/null.c
    source/external/openal/Alc/backends/thread_msg_queue_cpp11.cpp
    source/external/openal/Alc/ALc.c
    source/external/openal/Alc/ALc_cpp11.cpp
    source/external/openal/Alc/alcConfig.c
    source/external/openal/Alc/alcDedicated.c
    source/external/openal/Alc/alcEcho.c
    source/external/openal/Alc/alcModulator.c
    source/external/openal/Alc/alcReverb.c
    source/external/openal/Alc/alcRing.c
    source/external/openal/Alc/ALu.c
    source/external/openal/Alc/bs2b.c
    source/external/openal/Alc/helpers.c
    source/external/openal/Alc/hrtf.c
    source/external/openal/Alc/mixer.c
    source/external/openal/Alc/panning.c
	source/external/openal/Alc/alcThread.c
    source/external/openal/OpenAL32/alError.c
    source/external/openal/OpenAL32/alExtension.c
    source/external/openal/OpenAL32/alListener.c
    source/external/openal/OpenAL32/alSource.c
    source/external/openal/OpenAL32/alThunk.c
)
if(WINDOWS)
    set(ENGINE_EXTERNAL_OPENAL_SRC
	${OPENAL_COMMON_SRC}
    source/external/openal/Alc/backends/xaudio2.cpp
    )
elseif(LINUX)
    set(ENGINE_EXTERNAL_OPENAL_SRC
	${OPENAL_COMMON_SRC}

    )
elseif(ANDROID)
    set(ENGINE_EXTERNAL_OPENAL_SRC
	${OPENAL_COMMON_SRC}
    source/external/openal/Alc/backends/opensl.c
    )
elseif(APPLE)
    set(ENGINE_EXTERNAL_OPENAL_SRC
	${OPENAL_COMMON_SRC}

    )
elseif(WASM)
    set(ENGINE_EXTERNAL_OPENAL_SRC
	${OPENAL_COMMON_SRC}

    )
else()
	message( FATAL_ERROR "Unsupported architecture, openal" )
endif()

set(external_openal "source/external/openal/*")
source_group("external/openal" REGULAR_EXPRESSION "${external_openal}" )