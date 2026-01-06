
file(GLOB ENGINE_PLATFORM_WASM_SRC1 "source/platform/*.cpp")
file(GLOB_RECURSE ENGINE_PLATFORM_WASM_SRC2 "source/platform/common/glfw/*.cpp")
file(GLOB_RECURSE ENGINE_PLATFORM_WASM_SRC3 "source/platform/wasm/*.cpp")
set(ENGINE_PLATFORM_WASM_SRC 
	${ENGINE_PLATFORM_WASM_SRC1} 
	${ENGINE_PLATFORM_WASM_SRC2} 
	${ENGINE_PLATFORM_WASM_SRC3}
)

set(platform "source/platform/")
source_group("platform" REGULAR_EXPRESSION "${platform}" )
set(platform_common "source/platform/common/*")
source_group("platform/common" REGULAR_EXPRESSION "${platform_common}" )
set(platform_WASM "source/platform/WASM/")
source_group("platform/WASM" REGULAR_EXPRESSION "${platform_WASM}" )
set(platform_WASM_window "source/platform/WASM/window/")
source_group("platform/WASM/window" REGULAR_EXPRESSION "${platform_WASM_window}" )