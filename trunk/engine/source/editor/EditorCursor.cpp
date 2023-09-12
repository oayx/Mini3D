#include "EditorCursor.h"
#include "runtime/input/Input.h"
#include "platform/WindowManager.h"
#if defined(DC_PLATFORM_WIN32) || defined(DC_PLATFORM_LINUX)
#include "platform/common/glfw/GLFWRenderWindow.h"
#endif
 
DC_BEGIN_NAMESPACE
/********************************************************************/
bool EditorCursor::_isDraging = false;
String EditorCursor::_dragFile = "";
ImGuiMouseCursor EditorCursor::_lastMouseCursor = ImGuiMouseCursor_COUNT;
Vector<std::wstring> EditorCursor::_addFiles;
bool EditorCursor::Update()
{
	DC_PROFILE_FUNCTION();
	ImGuiIO& io = ImGui::GetIO();

	ImGuiMouseCursor mouse_cursor = io.MouseDrawCursor ? ImGuiMouseCursor_None : ImGui::GetMouseCursor();

	if (_isDraging && mouse_cursor != ImGuiMouseCursor_None)
		mouse_cursor = ImGuiMouseCursor_Hand;

	if (_lastMouseCursor == mouse_cursor)
		return false;
	
	_lastMouseCursor = mouse_cursor;
#if defined(DC_PLATFORM_WIN32) || defined(DC_PLATFORM_LINUX)
	#if defined(DC_GRAPHICS_API_DX9) || defined(DC_GRAPHICS_API_DX11)
		static HCURSOR MouseCursors[ImGuiMouseCursor_COUNT] =
		{
			::LoadCursor(NULL, IDC_ARROW),
			::LoadCursor(NULL, IDC_IBEAM),
			::LoadCursor(NULL, IDC_SIZEALL),
			::LoadCursor(NULL, IDC_SIZENS),
			::LoadCursor(NULL, IDC_SIZEWE),
			::LoadCursor(NULL, IDC_SIZENESW),
			::LoadCursor(NULL, IDC_SIZENWSE),
			::LoadCursor(NULL, IDC_HAND),
			::LoadCursor(NULL, IDC_NO),
		};

		if (mouse_cursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
		{
			::SetCursor(NULL);
		}
		else
		{
			::SetCursor(MouseCursors[mouse_cursor]);
		}
	#elif defined(DC_GRAPHICS_API_OPENGL) || defined(DC_GRAPHICS_API_OPENGLES3)
		static GLFWcursor* MouseCursors[ImGuiMouseCursor_COUNT] = 
		{
			glfwCreateStandardCursor(GLFW_ARROW_CURSOR),
			glfwCreateStandardCursor(GLFW_IBEAM_CURSOR),
			glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR),
			glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR),
			glfwCreateStandardCursor(GLFW_ARROW_CURSOR),
			glfwCreateStandardCursor(GLFW_ARROW_CURSOR),
			glfwCreateStandardCursor(GLFW_ARROW_CURSOR),
			glfwCreateStandardCursor(GLFW_HAND_CURSOR),
			glfwCreateStandardCursor(GLFW_ARROW_CURSOR),
		};

		RenderWindow* render_window = WindowManager::GetWindow(TargetDisplay::Display1);
		if (render_window->Is<GLFWRenderWindow>())
		{
			GLFWwindow* window = render_window->As<GLFWRenderWindow>()->GetGLFWwindow();
			if (mouse_cursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
			{
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
			}
			else
			{
				glfwSetCursor(window, MouseCursors[mouse_cursor]);
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}
		}
	#endif
#endif
	return true;
}
void EditorCursor::EndRender() 
{
	_addFiles.Clear(); 
	if (_isDraging && Input::GetMouseButtonUp(MouseBtnID::Left))
	{
		_isDraging = false;
		ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
	}
}
void EditorCursor::BeginDrag(const String& file)
{
	_isDraging = true;
	_dragFile = file;
}
void EditorCursor::EndDrag()
{
	_isDraging = false;
}
DC_END_NAMESPACE