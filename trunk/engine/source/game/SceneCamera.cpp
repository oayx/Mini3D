#include "SceneCamera.h"
#include "runtime/input/Input.h"
#include "runtime/scene/Camera.h"
#include "runtime/Application.h"
#include "editor/main/EMain_GameView.h"
#include "editor/EditorAppliction.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(SceneFreeCamera, Component);
void SceneFreeCamera::Update()
{
	base::Update();

	if (Application::IsEditor() && !EMain_GameView::IsFocus())
		return;

	Camera* camera = GetGameObject()->GetComponent<Camera>();
	if (!camera)return;

	if (!Input::GetKey(KeyCode::LeftControl) && !Input::GetKey(KeyCode::LeftShift) && !Input::GetKey(KeyCode::LeftAlt) &&
		!Input::GetKey(KeyCode::RightControl) && !Input::GetKey(KeyCode::RightShift) && !Input::GetKey(KeyCode::RightAlt))
	{//移动镜头
		float move_speed = _moveSpeed;
		if (Input::GetKey(KeyCode::W))
		{
			camera->MoveForwardBack(move_speed);
		}
		if (Input::GetKey(KeyCode::S))
		{
			camera->MoveForwardBack(-move_speed);
		}
		if (Input::GetKey(KeyCode::A))
		{
			camera->MoveLeftRight(-move_speed);
		}
		if (Input::GetKey(KeyCode::D))
		{
			camera->MoveLeftRight(move_speed);
		}
	}
	if (EditorConfig::Is3D && Input::IsMouseMove() && Input::GetMouseButton(MouseBtnID::Right))
	{//旋转镜头
		float offset_x = Input::mousePositionDelta.x;
		float offset_y = -Input::mousePositionDelta.y;
		if (Math::Abs(offset_x) > 0 || Math::Abs(offset_y) > 0)
		{
			camera->Rotate(offset_x, offset_y);
		}
	}
}
void SceneFreeCamera::OnDrawEditor()
{
	base::OnDrawEditor();

	ImGuiEx::Label("MoveSpeed");
	float min_value = 0.0f;
	ImGui::DragScalar("##MoveSpeed", ImGuiDataType_Float, &_moveSpeed, 0.01f, &min_value);
}
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(SceneFreeCamera);
template<class TransferFunction> inline
void SceneFreeCamera::Transfer(TransferFunction& transfer, void* ptr)
{
	base::Transfer(transfer, ptr);

	TRANSFER_SIMPLE(_moveSpeed);
}
DC_END_NAMESPACE
