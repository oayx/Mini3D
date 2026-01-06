
file(GLOB ENGINE_PLATFORM_IOS_SRC1 "source/platform/*.cpp")
file(GLOB_RECURSE ENGINE_PLATFORM_IOS_SRC2 "source/platform/common/glfw/*.cpp")
file(GLOB_RECURSE ENGINE_PLATFORM_IOS_SRC3 "source/platform/ios/*.cpp")
set(ENGINE_PLATFORM_IOS_SRC 
	${ENGINE_PLATFORM_IOS_SRC1} 
	${ENGINE_PLATFORM_IOS_SRC2} 
	${ENGINE_PLATFORM_IOS_SRC3}
)

set(platform "source/platform/")
source_group("platform" REGULAR_EXPRESSION "${platform}" )
set(platform_common "source/platform/common/*")
source_group("platform/common" REGULAR_EXPRESSION "${platform_common}" )
set(platform_ios "source/platform/ios/")
source_group("platform/ios" REGULAR_EXPRESSION "${platform_ios}" )
set(platform_ios_window "source/platform/ios/window/")
source_group("platform/ios/window" REGULAR_EXPRESSION "${platform_ios_window}" )