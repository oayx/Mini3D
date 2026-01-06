
file(GLOB_RECURSE RUNTIME_COMMON_SRC
	source/runtime/audio/*.cpp
	source/runtime/component/*.cpp
	source/runtime/component/*.inl
	source/runtime/font/*.cpp
	source/runtime/graphics/Graphics.cpp
	source/runtime/graphics/Material.cpp
	source/runtime/graphics/Pass.cpp
	source/runtime/graphics/RenderDefine.cpp
	source/runtime/graphics/ReflectionProbe.cpp
	source/runtime/graphics/RenderDepthMap.cpp
	source/runtime/graphics/Shader.cpp
	source/runtime/graphics/ShadowMap.cpp
	source/runtime/graphics/TextureUnit.cpp
	source/runtime/graphics/null/*.cpp
	source/runtime/input/*.cpp
	source/runtime/lua/*.cpp
	source/runtime/mesh/*.cpp
	source/runtime/net/*.cpp
	source/runtime/particle/*.cpp
	source/runtime/physics/*.cpp
	source/runtime/physics2d/*.cpp
	source/runtime/postprocess/*.cpp
	source/runtime/project/*.cpp
	source/runtime/renderable/*.cpp
	source/runtime/resources/*.cpp
	source/runtime/scene/*.cpp
	source/runtime/skeleton2d/*.cpp
	source/runtime/terrain/*.cpp
	source/runtime/thread/*.cpp
	source/runtime/tools/*.cpp
	source/runtime/tween/*.cpp
	source/runtime/ui/*.cpp
	source/runtime/video/*.cpp
	source/runtime/Application.cpp
	source/runtime/CommandLine.cpp
	source/runtime/DllExport.cpp
	source/runtime/Screen.cpp
)

if(WINDOWS)
file(GLOB_RECURSE ENGINE_RUNTIME_SRC
	${RUNTIME_COMMON_SRC}
	source/runtime/graphics/dx/*.cpp
	source/runtime/graphics/opengl/*.cpp
	source/runtime/profiler/*.cpp
	)
elseif(LINUX OR ANDROID OR WASM OR APPLE)
file(GLOB_RECURSE ENGINE_RUNTIME_SRC
	${RUNTIME_COMMON_SRC}
	source/runtime/graphics/opengl/*.cpp
	)
else()
	message( FATAL_ERROR "Unsupported architecture, engine" )
endif()

set(runtime "source/runtime/")
source_group("runtime" REGULAR_EXPRESSION "${runtime}" )