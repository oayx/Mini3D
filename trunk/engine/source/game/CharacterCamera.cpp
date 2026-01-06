#include "CharacterCamera.h"
#include "runtime/input/Input.h"
#include "runtime/mesh/Animation.h"
#include "runtime/physics/CharacterController.h"
#include "runtime/scene/Camera.h"
#include "runtime/Application.h"
#include "editor/main/EMain_GameView.h"
#include "editor/EditorAppliction.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(ThirdFollowCamera, Component);
void ThirdFollowCamera::Update()
{
	base::Update();

	if (Application::IsEditor() && !EMain_GameView::IsFocus())
		return;

	Camera* camera = SceneManager::GetMainCamera();
	if (!camera)return;

	Transform* transform = GetTransform();
	camera->SetPosition(transform->GetPosition() + _cameraOffset);
	camera->GetTransform()->LookAt(transform->GetPosition() + _cameraLookatOffset, Vector3::up);
}
void ThirdFollowCamera::OnDrawEditor()
{
	base::OnDrawEditor();

	ImGuiEx::Label("Camera Offset");
	ImGui::DragFloat3("##CameraOffset", _cameraOffset.ptr(), 0.1f);

	ImGuiEx::Label("Camera Lookat Offset");
	ImGui::DragFloat3("##CameraLookatOffset", _cameraLookatOffset.ptr(), 0.1f);
}
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(ThirdFollowCamera);
template<class TransferFunction> inline
void ThirdFollowCamera::Transfer(TransferFunction& transfer, void* ptr)
{
	base::Transfer(transfer, ptr);

	TRANSFER_SIMPLE(_cameraOffset);
	TRANSFER_SIMPLE(_cameraLookatOffset);
}

/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(ThirdFreeCamera, Component);
void ThirdFreeCamera::Start()
{
	base::Start();

	Camera* camera = SceneManager::GetMainCamera();
	if (!camera)return;

	Transform* transform = GetTransform();
	camera->SetPosition(transform->GetPosition() + _cameraOffset);
	camera->GetTransform()->LookAt(transform->GetPosition() + _cameraLookatOffset, Vector3::up);
}
void ThirdFreeCamera::Update()
{
	base::Update();

	if (Application::IsEditor() && !EMain_GameView::IsFocus())
		return;

	Camera* camera = SceneManager::GetMainCamera();
	if (!camera)return;

	Transform* transform = GetTransform();
	Transform* cameraTransform = camera->GetTransform();

	bool isWalking = false;
	if (Input::GetKey(KeyCode::W))
	{
		CharacterController* controller = GetGameObject()->GetComponent<CharacterController>();
		if (controller)
		{
			Vector3 forward = Vector3(cameraTransform->GetForward().x, 0.0f, cameraTransform->GetForward().z);
			controller->Move(forward.Normalize() * _moveSpeed);
			isWalking = true;
		}
	}
	if (Input::GetKey(KeyCode::S))
	{
		CharacterController* controller = GetGameObject()->GetComponent<CharacterController>();
		if (controller)
		{
			Vector3 forward = Vector3(cameraTransform->GetForward().x, 0.0f, cameraTransform->GetForward().z);
			controller->Move(-forward.Normalize() * _moveSpeed);
			isWalking = true;
		}
	}

	Animation* animation = GetGameObject()->GetComponent<Animation>();
	if (animation)
	{
		animation->Play(isWalking ? "run" : "idle", true);
	}

	Vector3 lookat = transform->GetPosition() + _cameraLookatOffset;
	if (Input::IsMouseMove() && Input::GetMouseButton(MouseBtnID::Right))
	{//旋转镜头
		float offsetX = Input::mousePositionDelta.x;
		float offsetY = -Input::mousePositionDelta.y;			
		if (Math::Abs(offsetX) > 0 || Math::Abs(offsetY) > 0)
		{
			Vector3 oldTarget = transform->GetPosition() + _cameraLookatOffset;
			Vector3 oldForward = oldTarget - cameraTransform->GetPosition();
			camera->Rotate(offsetX, offsetY);
			Vector3 nowTarget = cameraTransform->GetPosition() + cameraTransform->GetForward() * _cameraOffset.Lenght();
			Vector3 offset = oldTarget - nowTarget;
			Vector3 pos = transform->GetPosition() + _cameraLookatOffset + offset - oldForward;
			cameraTransform->SetPosition(pos);
			cameraTransform->LookAt(transform->GetPosition() + _cameraLookatOffset, Vector3::up);
		}
		if (Math::Abs(offsetX) > 0)
		{
			const Vector3& forward = cameraTransform->GetForward();
			transform->LookTo(Vector3(forward.x, 0.0f, forward.z), Vector3::up);
		}
	}
}
void ThirdFreeCamera::OnDrawEditor()
{
	base::OnDrawEditor();

	ImGuiEx::Label("MoveSpeed");
	float minValue = 0.0f;
	ImGui::DragScalar("##MoveSpeed", ImGuiDataType_Float, &_moveSpeed, 0.01f, &minValue);

	ImGuiEx::Label("Camera Offset");
	ImGui::DragFloat3("##CameraOffset", _cameraOffset.ptr(), 0.1f);

	ImGuiEx::Label("Camera Lookat Offset");
	ImGui::DragFloat3("##CameraLookatOffset", _cameraLookatOffset.ptr(), 0.1f);
}
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(ThirdFreeCamera);
template<class TransferFunction> inline
void ThirdFreeCamera::Transfer(TransferFunction& transfer, void* ptr)
{
	base::Transfer(transfer, ptr);

	TRANSFER_SIMPLE(_moveSpeed);
	TRANSFER_SIMPLE(_cameraOffset);
	TRANSFER_SIMPLE(_cameraLookatOffset);
}
DC_END_NAMESPACE
