message("当前CMakeLists目录: ${CMAKE_CURRENT_SOURCE_DIR}")

set(GLFW_COMMON_SRC 
	source/external/glfw3/context.c
	source/external/glfw3/init.c
	source/external/glfw3/input.c
	source/external/glfw3/monitor.c
	source/external/glfw3/vulkan.c
	source/external/glfw3/window.c
)
if(WINDOWS)
set(ENGINE_EXTERNAL_GLFW_SRC
	${GLFW_COMMON_SRC}
	source/external/glfw3/win32_init.c
	source/external/glfw3/win32_joystick.c
	source/external/glfw3/win32_monitor.c
	source/external/glfw3/win32_thread.c
	source/external/glfw3/win32_time.c
	source/external/glfw3/win32_window.c
	source/external/glfw3/wgl_context.c
	source/external/glfw3/egl_context.c
	source/external/glfw3/osmesa_context.c
)
elseif(LINUX)
set(ENGINE_EXTERNAL_GLFW_SRC
	${GLFW_COMMON_SRC}
	source/external/glfw3/x11_init.c
	source/external/glfw3/x11_monitor.c
	source/external/glfw3/x11_window.c
	source/external/glfw3/xkb_unicode.c
	source/external/glfw3/posix_time.c
	source/external/glfw3/posix_thread.c
	source/external/glfw3/glx_context.c
	source/external/glfw3/egl_context.c
	source/external/glfw3/osmesa_context.c
	source/external/glfw3/linux_joystick.c
)
elseif(MACOSX OR iOS)
set(ENGINE_EXTERNAL_GLFW_SRC
	${GLFW_COMMON_SRC}
	source/external/glfw3/cocoa_init.m
	source/external/glfw3/cocoa_joystick.m
	source/external/glfw3/cocoa_monitor.m
	source/external/glfw3/cocoa_window.m
	source/external/glfw3/cocoa_time.c
	source/external/glfw3/posix_thread.c
	source/external/glfw3/nsgl_context.m
	source/external/glfw3/egl_context.c
	source/external/glfw3/osmesa_context.c
)
elseif(WASM)

else()
	message( FATAL_ERROR "Unsupported architecture, glfw" )
endif()

set(external_glfw "source/external/glfw3/*")
source_group("external/glfw3" REGULAR_EXPRESSION "${external_glfw}" )