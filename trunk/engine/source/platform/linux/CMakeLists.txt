
file(GLOB ENGINE_PLATFORM_LINUX_SRC1 "source/platform/*.cpp")
file(GLOB_RECURSE ENGINE_PLATFORM_LINUX_SRC2 "source/platform/common/*.cpp")
file(GLOB_RECURSE ENGINE_PLATFORM_LINUX_SRC3 "source/platform/linux/*.cpp")
set(ENGINE_PLATFORM_LINUX_SRC 
	${ENGINE_PLATFORM_LINUX_SRC1} 
	${ENGINE_PLATFORM_LINUX_SRC2} 
	${ENGINE_PLATFORM_LINUX_SRC3}
)

set(platform "source/platform/")
source_group("platform" REGULAR_EXPRESSION "${platform}" )
set(platform_common "source/platform/common/*")
source_group("platform/common" REGULAR_EXPRESSION "${platform_common}" )
set(platform_linux "source/platform/linux/")
source_group("platform/linux" REGULAR_EXPRESSION "${platform_linux}" )
set(platform_linux_window "source/platform/linux/window/")
source_group("platform/linux/window" REGULAR_EXPRESSION "${platform_linux_window}" )