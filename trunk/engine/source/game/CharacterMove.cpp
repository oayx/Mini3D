#include "CharacterMove.h"
#include "runtime/input/Input.h"
#include "runtime/physics/CharacterController.h"
#include "runtime/Application.h"
#include "editor/main/EMain_GameView.h"
#include "editor/EditorAppliction.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(CharacterMove, Component);
void CharacterMove::Update()
{
	base::Update();

	if (Application::IsEditor() && !EMain_GameView::IsFocus())
		return;

	if (Input::GetKey(KeyCode::W))
	{
		CharacterController* controller = GetGameObject()->GetComponent<CharacterController>();
		if (controller)
		{
			controller->Move(GetTransform()->GetForward() * _moveSpeed);
		}
	}
	if (Input::GetKey(KeyCode::S))
	{
		CharacterController* controller = GetGameObject()->GetComponent<CharacterController>();
		if (controller)
		{
			controller->Move(-GetTransform()->GetForward() * _moveSpeed);
		}
	}
	if (Input::GetKey(KeyCode::A))
	{
		CharacterController* controller = GetGameObject()->GetComponent<CharacterController>();
		if (controller)
		{
			controller->Move(-GetTransform()->GetRight() * _moveSpeed);
		}
	}
	if (Input::GetKey(KeyCode::D))
	{
		CharacterController* controller = GetGameObject()->GetComponent<CharacterController>();
		if (controller)
		{
			controller->Move(GetTransform()->GetRight() * _moveSpeed);
		}
	}
}
void CharacterMove::OnDrawEditor()
{
	base::OnDrawEditor();

	ImGuiEx::Label("MoveSpeed");
	float min_value = 0.0f;
	ImGui::DragScalar("##MoveSpeed", ImGuiDataType_Float, &_moveSpeed, 0.01f, &min_value);
}
/********************************************************************/
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(CharacterMove);
template<class TransferFunction> inline
void CharacterMove::Transfer(TransferFunction& transfer, void* ptr)
{
	base::Transfer(transfer, ptr);
	TRANSFER_SIMPLE(_moveSpeed);
}
DC_END_NAMESPACE
