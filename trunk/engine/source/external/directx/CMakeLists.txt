message("当前CMakeLists目录: ${CMAKE_CURRENT_SOURCE_DIR}")

set(ENGINE_EXTERNAL_DIRECTX_SRC
	source/external/directx/dds/DDSTextureLoader9.cpp
	source/external/directx/dds/DDSTextureLoader11.cpp
	source/external/directx/dds/DDSTextureLoader12.cpp
	source/external/directx/grab/ScreenGrab9.cpp
	source/external/directx/grab/ScreenGrab11.cpp
	source/external/directx/grab/ScreenGrab12.cpp
	source/external/directx/wic/WICTextureLoader9.cpp
	source/external/directx/wic/WICTextureLoader11.cpp
	source/external/directx/wic/WICTextureLoader12.cpp
)

set(external_directx "source/external/directx/*")
source_group("external/directx" REGULAR_EXPRESSION "${external_directx}" )