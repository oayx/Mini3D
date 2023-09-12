
file(GLOB ENGINE_PLATFORM_ANDROID_SRC1 "source/platform/*.cpp")
file(GLOB ENGINE_PLATFORM_ANDROID_SRC2 "source/platform/common/egl/*.cpp")
file(GLOB_RECURSE ENGINE_PLATFORM_ANDROID_SRC3 "source/platform/android/*.cpp")
set(ENGINE_PLATFORM_ANDROID_SRC 
	${ENGINE_PLATFORM_ANDROID_SRC1} 
	${ENGINE_PLATFORM_ANDROID_SRC2} 
	${ENGINE_PLATFORM_ANDROID_SRC3}
)
#foreach(file IN LISTS ENGINE_PLATFORM_ANDROID_SRC)
#message(STATUS ${file})
#endforeach(file)

set(platform "source/platform/")
source_group("platform" REGULAR_EXPRESSION "${platform}" )
set(platform_common "source/platform/common/*")
source_group("platform/common" REGULAR_EXPRESSION "${platform_common}" )
set(platform_android "source/platform/android/")
source_group("platform/android" REGULAR_EXPRESSION "${platform_android}" )
set(platform_android_window "source/platform/android/window/")
source_group("platform/android/window" REGULAR_EXPRESSION "${platform_android_window}" )