set(GLFW_COMMON_SRC 
	source/external/glfw/context.c
	source/external/glfw/init.c
	source/external/glfw/input.c
	source/external/glfw/monitor.c
	source/external/glfw/vulkan.c
	source/external/glfw/window.c
)
if(WINDOWS)
set(ENGINE_EXTERNAL_GLFW_SRC
	${GLFW_COMMON_SRC}
	source/external/glfw/win32_init.c
	source/external/glfw/win32_joystick.c
	source/external/glfw/win32_monitor.c
	source/external/glfw/win32_thread.c
	source/external/glfw/win32_time.c
	source/external/glfw/win32_window.c
	source/external/glfw/wgl_context.c
	source/external/glfw/egl_context.c
	source/external/glfw/osmesa_context.c
)
elseif(LINUX)
set(ENGINE_EXTERNAL_GLFW_SRC
	${GLFW_COMMON_SRC}
	source/external/glfw/x11_init.c
	source/external/glfw/x11_monitor.c
	source/external/glfw/x11_window.c
	source/external/glfw/xkb_unicode.c
	source/external/glfw/posix_time.c
	source/external/glfw/posix_thread.c
	source/external/glfw/glx_context.c
	source/external/glfw/egl_context.c
	source/external/glfw/osmesa_context.c
	source/external/glfw/linux_joystick.c
)
elseif(MACOSX OR APPLE)
set(ENGINE_EXTERNAL_GLFW_SRC
	${GLFW_COMMON_SRC}
	source/external/glfw/cocoa_init.m
	source/external/glfw/cocoa_joystick.m
	source/external/glfw/cocoa_monitor.m
	source/external/glfw/cocoa_window.m
	source/external/glfw/cocoa_time.c
	source/external/glfw/posix_thread.c
	source/external/glfw/nsgl_context.m
	source/external/glfw/egl_context.c
	source/external/glfw/osmesa_context.c
)
else()
	message( FATAL_ERROR "Unsupported architecture, glfw" )
endif()

set(external_glfw "source/external/glfw/*")
source_group("external/glfw" REGULAR_EXPRESSION "${external_glfw}" )