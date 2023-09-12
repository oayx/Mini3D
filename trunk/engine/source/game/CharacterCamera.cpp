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
	Transform* camera_transform = camera->GetTransform();

	bool is_walking = false;
	if (Input::GetKey(KeyCode::W))
	{
		CharacterController* controller = GetGameObject()->GetComponent<CharacterController>();
		if (controller)
		{
			Vector3 forward = Vector3(camera_transform->GetForward().x, 0.0f, camera_transform->GetForward().z);
			controller->Move(forward.Normalize() * _moveSpeed);
			is_walking = true;
		}
	}
	if (Input::GetKey(KeyCode::S))
	{
		CharacterController* controller = GetGameObject()->GetComponent<CharacterController>();
		if (controller)
		{
			Vector3 forward = Vector3(camera_transform->GetForward().x, 0.0f, camera_transform->GetForward().z);
			controller->Move(-forward.Normalize() * _moveSpeed);
			is_walking = true;
		}
	}

	Animation* animation = GetGameObject()->GetComponent<Animation>();
	if (animation)
	{
		animation->Play(is_walking ? "run" : "idle", true);
	}

	Vector3 lookat = transform->GetPosition() + _cameraLookatOffset;
	if (Input::IsMouseMove() && Input::GetMouseButton(MouseBtnID::Right))
	{//旋转镜头
		float offset_x = Input::mousePositionDelta.x;
		float offset_y = -Input::mousePositionDelta.y;			
		if (Math::Abs(offset_x) > 0 || Math::Abs(offset_y) > 0)
		{
			Vector3 old_target = transform->GetPosition() + _cameraLookatOffset;
			Vector3 old_forward = old_target - camera_transform->GetPosition();
			camera->Rotate(offset_x, offset_y);
			Vector3 now_target = camera_transform->GetPosition() + camera_transform->GetForward() * _cameraOffset.Lenght();
			Vector3 offset = old_target - now_target;
			Vector3 pos = transform->GetPosition() + _cameraLookatOffset + offset - old_forward;
			camera_transform->SetPosition(pos);
			camera_transform->LookAt(transform->GetPosition() + _cameraLookatOffset, Vector3::up);
		}
		if (Math::Abs(offset_x) > 0)
		{
			const Vector3& forward = camera_transform->GetForward();
			transform->LookTo(Vector3(forward.x, 0.0f, forward.z), Vector3::up);
		}
	}
}
void ThirdFreeCamera::OnDrawEditor()
{
	base::OnDrawEditor();

	ImGuiEx::Label("MoveSpeed");
	float min_value = 0.0f;
	ImGui::DragScalar("##MoveSpeed", ImGuiDataType_Float, &_moveSpeed, 0.01f, &min_value);

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
