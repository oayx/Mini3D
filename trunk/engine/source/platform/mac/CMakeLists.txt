
file(GLOB ENGINE_PLATFORM_MAC_SRC1 "source/platform/*.cpp")
file(GLOB_RECURSE ENGINE_PLATFORM_MAC_SRC2 "source/platform/common/glfw/*.cpp")
file(GLOB_RECURSE ENGINE_PLATFORM_MAC_SRC3 "source/platform/mac/*.cpp")
set(ENGINE_PLATFORM_MAC_SRC 
	${ENGINE_PLATFORM_MAC_SRC1} 
	${ENGINE_PLATFORM_MAC_SRC2} 
	${ENGINE_PLATFORM_MAC_SRC3}
)

set(platform "source/platform/")
source_group("platform" REGULAR_EXPRESSION "${platform}" )
set(platform_common "source/platform/common/*")
source_group("platform/common" REGULAR_EXPRESSION "${platform_common}" )
set(platform_mac "source/platform/mac/")
source_group("platform/mac" REGULAR_EXPRESSION "${platform_mac}" )
set(platform_mac_window "source/platform/mac/window/")
source_group("platform/mac/window" REGULAR_EXPRESSION "${platform_mac_window}" )