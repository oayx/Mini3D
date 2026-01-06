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
		float moveSpeed = _moveSpeed;
		if (Input::GetKey(KeyCode::W))
		{
			camera->MoveForwardBack(moveSpeed);
		}
		if (Input::GetKey(KeyCode::S))
		{
			camera->MoveForwardBack(-moveSpeed);
		}
		if (Input::GetKey(KeyCode::A))
		{
			camera->MoveLeftRight(-moveSpeed);
		}
		if (Input::GetKey(KeyCode::D))
		{
			camera->MoveLeftRight(moveSpeed);
		}
	}
	if (EditorConfig::Is3D && Input::IsMouseMove() && Input::GetMouseButton(MouseBtnID::Right))
	{//旋转镜头
		float offsetX = Input::mousePositionDelta.x;
		float offsetY = -Input::mousePositionDelta.y;
		if (Math::Abs(offsetX) > 0 || Math::Abs(offsetY) > 0)
		{
			camera->Rotate(offsetX, offsetY);
		}
	}
}
void SceneFreeCamera::OnDrawEditor()
{
	base::OnDrawEditor();

	ImGuiEx::Label("MoveSpeed");
	float minValue = 0.0f;
	ImGui::DragScalar("##MoveSpeed", ImGuiDataType_Float, &_moveSpeed, 0.01f, &minValue);
}
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(SceneFreeCamera);
template<class TransferFunction> inline
void SceneFreeCamera::Transfer(TransferFunction& transfer, void* ptr)
{
	base::Transfer(transfer, ptr);

	TRANSFER_SIMPLE(_moveSpeed);
}
DC_END_NAMESPACE
