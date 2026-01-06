message("当前CMakeLists目录: ${CMAKE_CURRENT_SOURCE_DIR}")

set(ENGINE_EXTERNAL_IMGUI_SRC
	source/external/imgui/imgui.cpp
	source/external/imgui/imgui_draw.cpp
	source/external/imgui/imgui_tables.cpp
	source/external/imgui/imgui_widgets.cpp
	source/external/imgui/guizmo/GraphEditor.cpp
	source/external/imgui/guizmo/ImCurveEdit.cpp
	source/external/imgui/guizmo/ImGradient.cpp
	source/external/imgui/guizmo/ImGuizmo.cpp
	source/external/imgui/guizmo/ImSequencer.cpp
)

set(external_imgui "source/external/imgui/*")
source_group("external/imgui" REGULAR_EXPRESSION "${external_imgui}" )