
file(GLOB ENGINE_PLATFORM_WINDOWS_SRC1 "source/platform/*.cpp")
file(GLOB_RECURSE ENGINE_PLATFORM_WINDOWS_SRC2 "source/platform/common/*.cpp")
file(GLOB_RECURSE ENGINE_PLATFORM_WINDOWS_SRC3 "source/platform/windows/*.cpp")
set(ENGINE_PLATFORM_WINDOWS_SRC 
	${ENGINE_PLATFORM_WINDOWS_SRC1} 
	${ENGINE_PLATFORM_WINDOWS_SRC2} 
	${ENGINE_PLATFORM_WINDOWS_SRC3}
)

set(platform "source/platform/")
source_group("platform" REGULAR_EXPRESSION "${platform}" )
set(platform_common "source/platform/common/*")
source_group("platform/common" REGULAR_EXPRESSION "${platform_common}" )
set(platform_windows "source/platform/windows/")
source_group("platform/windows" REGULAR_EXPRESSION "${platform_windows}" )
set(platform_windows_window "source/platform/windows/window/")
source_group("platform/windows/window" REGULAR_EXPRESSION "${platform_windows_window}" )