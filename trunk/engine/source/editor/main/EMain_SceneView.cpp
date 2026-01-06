#include "EMain_SceneView.h"
#include "EMain_Hierarchy.h"
#include "runtime/graphics/Material.h"
#include "runtime/graphics/null/PostProcess.h"
#include "runtime/input/Input.h"
#include "runtime/physics/Physics.h"
#include "runtime/postprocess/ShowDepthMapEffect.h"
#include "runtime/postprocess/ShowDepthNormalEffect.h"
#include "runtime/renderable/LineRender.h"
#include "runtime/scene/Camera.h"
#include "runtime/ui/GUI.h"
#include "runtime/ui/UICanvas.h"
#include "runtime/ui/UICanvasScaler.h"
#include "runtime/Application.h"
#include "runtime/Screen.h"
#include "inspector/EInspector_Hierarchy.h"
#include "inspector/EInspector_Project.h"
#include "inspector/EInspector_ProjectSettings_Layers.h"
#include "editor/EditorAppliction.h"
#include "editor/main/EMain_Tools.h"
#include "external/imgui/guizmo/ImGuizmo.h"
#include "external/imgui/imgui_internal.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(EMain_SceneView, EWindowBase);
void EMain_SceneView::Update()
{
	DC_PROFILE_FUNCTION;
	HandleInput();

	if (_oldViewPort != _viewPort)
	{
		Camera* camera = GetCamera();

		TextureDesc desc;
		desc.width = _viewPort.width; desc.height = _viewPort.height; desc.antiAlias = 4; desc.flags = TextureFlag::COLOR_AND_DEPTH;
		camera->CreateRenderTexture(desc);
		camera->SetAspect(_viewPort.width / _viewPort.height);

		_oldViewPort = _viewPort;
	}
}
void EMain_SceneView::Render()
{
	DC_PROFILE_FUNCTION;
	if (!IsShow)return;

	ImGuizmo::BeginFrame();
	//ImGuizmo::SetOrthographic(true);

	ImGui::SetNextWindowSizeConstraints(ImVec2(400.0f, 400.0f), ImVec2(FLT_MAX, FLT_MAX));
	ImGui::Begin(ICON_FA_PENCIL_ALT" Scene   ", &IsShow, ImGuiWindowFlags_MenuBar);
	{
		ShowToolbar();
		
		const ImVec2& pos = ImGui::GetCursorScreenPos();
		const ImVec2& size = ImGui::GetContentRegionAvail();
		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(pos.x, pos.y, size.x, size.y);
		_viewPort.Set(pos.x, pos.y, size.x, size.y);

		if (GetCamera()->GetRenderTexture())
		{
			ImGui::Image(GetCamera()->GetRenderTexture(), size);
		}

		ShowSelectObject();
		ShowLineGrid();

		_isMouseIn = ImGui::IsWindowHovered();

		//焦点
		if (ImGui::IsItemClicked(1) && ImGui::IsWindowHovered(1))
		{
			ImGui::SetWindowFocus();
		}
	}
	_isFocus = ImGui::IsWindowFocused();
	ImGui::End();
}
void EMain_SceneView::ShowToolbar()
{
	if (ImGui::BeginMenuBar())
	{
		//stats
		ImGui::Spacing();
		static bool statsSelect = false;
		ImGui::Selectable("Stats", &statsSelect, 0, ImVec2(30.0f, 0.0f));
		if (statsSelect)
		{
			ImVec2 lineStart = ImGui::GetCursorScreenPos();
			lineStart.x -= 40.0f;
			lineStart.y += ImGui::GetFrameHeightWithSpacing();
			ImGui::SetNextWindowBgAlpha(0.7f);
			ImGui::SetNextWindowPos(lineStart);
			ImGui::Begin("EditorAppInfo", 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMouseInputs);
			{
				ImGui::Text("FPS:%.1f(%.3fms)", ImGui::GetIO().Framerate, Application::GetFrameTime());
				ImGui::SameLine(150.0f);
				ImGui::Text("Screen:%.0f*%.0f", 1920.0f, 1080.0f);

				ImGui::Text("Tris:%d", SceneManager::GetDrawTriangles());
				ImGui::SameLine(150.0f);
				ImGui::Text("DrawCall:%d", SceneManager::GetDrawCall());

				ImGui::Text("Batch:%d", SceneManager::GetRenderBatch());
				ImGui::SameLine(150.0f);
				ImGui::Text("SetPass:%d", SceneManager::GetSetPassCall());
			}
			ImGui::End();
		}
		ImGui::Spacing();

		//layer
		const char* layers[] = { "Shaded", "Wireframe", "Scene Depth", "Scene Normal" };
		static int currentSelect = 0;
		ImGui::SetNextItemWidth(120.0f);
		if (ImGui::Combo("##Layer", &currentSelect, layers, ARRAY_SIZE(layers)))
		{
			if (currentSelect == 0 || currentSelect == 1)
			{
				Camera* camera = GetCamera();
				if(currentSelect == 0)camera->SetFillMode(FillMode::Solid);
				else if (currentSelect == 1)camera->SetFillMode(FillMode::Wire);

				_camera->ClearDepthTextureMode();
				_camera->RemovePostEffect(typeof(ShowDepthMapEffect));
				_camera->RemovePostEffect(typeof(ShowDepthNormalEffect));
			}
			else if(currentSelect == 2)
			{
				_camera->ClearDepthTextureMode();
				_camera->RemovePostEffect(typeof(ShowDepthMapEffect));
				_camera->RemovePostEffect(typeof(ShowDepthNormalEffect));

				_camera->AddDepthTextureMode(DepthTextureMode::Depth);
				_camera->AddPostEffect(typeof(ShowDepthMapEffect));
			}
			else if (currentSelect == 3)
			{
				_camera->ClearDepthTextureMode();
				_camera->RemovePostEffect(typeof(ShowDepthMapEffect));
				_camera->RemovePostEffect(typeof(ShowDepthNormalEffect));

				_camera->AddDepthTextureMode(DepthTextureMode::Depth | DepthTextureMode::Normals);
				_camera->AddPostEffect(typeof(ShowDepthNormalEffect));
			}
		}
		ImGui::Spacing();

		//gizmos
		ImVec2 popupStart = ImGui::GetCursorScreenPos(); popupStart.y += ImGui::GetFrameHeight() + 2.0f;
		if (ImGui::Selectable("Gizmos", false, 0, ImVec2(ImGui::CalcTextSize("Gizmos").x, 0.0f)))
			ImGui::OpenPopup("gizmos_popup");
		ImGui::SetNextWindowPos(popupStart);
		ImGui::SetNextWindowSize(ImVec2(300.0f, 300.0f));
		if (ImGui::BeginPopup("gizmos_popup"))
		{
			if (ImGui::MenuItem("Show Grid", "", &EditorConfig::ShowGrid))
			{
				if (_lineGrid)
				{
					_lineGrid->GetTransform()->SetLocalRotation(Quaternion(Vector3(EditorConfig::Is3D ? 0.0f : -90.0f, 0.0f, 0.0f)));
				}
			}
			bool enablePhysicsDebug = Physics::IsEnableDebug();
			if (ImGui::MenuItem("Physics Debug", "", &enablePhysicsDebug))
			{
				Physics::EnableDebug(enablePhysicsDebug);
			}
			ImGui::EndPopup();
		}
		ImGui::Spacing();

		//2d+3d
		if (ImGui::Selectable(EditorConfig::Is3D ? "  3D  " : "  2D  ", false, 0, ImVec2(ImGui::CalcTextSize("  2D  ").x, 0.0f)))
		{
			EditorConfig::Is3D = !EditorConfig::Is3D;
			EditorAppliction::SetInspectorType(InspectorType::None, 0);
			Set3D(EditorConfig::Is3D);
		}
		ImGui::Spacing();

		//speed
		ImGui::Spacing();
		ImGui::SetNextItemWidth(150.0f);
		ImGui::SliderFloat("##Speed", &EditorConfig::OperatorSpeed, 0.1f, 5.0f);

		ImGui::EndMenuBar();
	}
}
void EMain_SceneView::ShowSelectObject()
{
	Camera* camera = GetCamera();
	if (!camera)return;

	const float* matView = camera->GetViewMatrix().p;
	const float* matProj = camera->GetProjMatrix().p;
	if (EditorAppliction::GetInspectorId())
	{
		GameObject* selectObject = SceneManager::GetGameObject(EditorAppliction::GetInspectorId());
		if (selectObject)
		{
			Transform* objectTransform = selectObject->GetTransform();
			ImGuizmo::OPERATION imOperation = (ImGuizmo::OPERATION)0;
			GizmoOperation operation = EMain_Tools::GetOperation();
			switch (operation)
			{
			case GizmoOperation::Translate:
				imOperation = EditorConfig::Is3D ? ImGuizmo::TRANSLATE : ImGuizmo::TRANSLATE_X | ImGuizmo::TRANSLATE_Y;
				break;
			case GizmoOperation::Rotate:
				imOperation = EditorConfig::Is3D ? ImGuizmo::ROTATE : ImGuizmo::ROTATE_Z;
				break;
			case GizmoOperation::Scale:
				imOperation = EditorConfig::Is3D ? ImGuizmo::SCALE : ImGuizmo::SCALE_X | ImGuizmo::SCALE_Y;
				break;
			case GizmoOperation::Universal:
				imOperation = ImGuizmo::UNIVERSAL;
				break;
			}

			if (imOperation != (ImGuizmo::OPERATION)0)
			{
				Matrix4 matrix;
				Memory::Copy(matrix.p, objectTransform->GetLocalToWorldMatrix().p, sizeof(Matrix4));
				ImGuizmo::MODE im_mode = EditorConfig::IsLocal ? ImGuizmo::MODE::LOCAL : ImGuizmo::MODE::WORLD;
				if (ImGuizmo::Manipulate(matView, matProj, imOperation, im_mode, matrix.p, nullptr, nullptr, nullptr, nullptr))
				{
					//矩阵分解
					Vector3 translation, scale;
					Quaternion rotation;
					matrix.Decompose(translation, rotation, scale);
					if (objectTransform->GetEditorCanTranslate())objectTransform->SetPosition(translation);
					if (objectTransform->GetEditorCanRotate())objectTransform->SetRotation(rotation);
					if (objectTransform->GetEditorCanScale())objectTransform->SetScale(scale);
				}
			}
			if (EditorConfig::Is3D)
			{
				Transform* cameraTransform = camera->GetTransform();
				float matView[16] = { 0.0f };
				ImGuizmo::LookAt(cameraTransform->GetPosition().p, objectTransform->GetPosition().p, cameraTransform->GetUp().p, matView);
				float distance = cameraTransform->GetPosition().Distance(objectTransform->GetPosition());
				float pos[3] = { 0.0f }, lookat[3] = { 0.0f }, up[3] = { 0.0f };
				float pos_x = ImGui::GetWindowPos().x + ImGui::GetWindowWidth();
				float pos_y = ImGui::GetWindowPos().y;
				if (ImGuizmo::ViewManipulate(matView, distance, ImVec2(pos_x - 128.0f, pos_y + ImGui::GetFrameHeight() + 10.0f), ImVec2(128.0f, 128.0f), 0x10101010, pos, lookat, up))
				{
					cameraTransform->SetPosition(Vector3(pos[0], pos[1], pos[2]));
					cameraTransform->LookAt(Vector3(lookat[0], lookat[1], lookat[2]), Vector3(up[0], up[1], up[2]));
				}
			}
		}
	}
}
void EMain_SceneView::ShowLineGrid()
{
	Camera* camera = GetCamera();
	if (!camera)return;

	if (EditorConfig::ShowGrid)
	{
		if (!_lineGrid)
		{
			GameObject* obj = GameObject::Instantiation("LineGrid", SceneManager::GetEngineObject()->GetTransform());
			obj->InsertFlag(GameObjectFlag::AlwaysFindInScene);
			obj->InsertFlag(GameObjectFlag::VisibleEditorCamera);
			_lineGrid = obj->AddComponent<LineRender>();
			_lineGrid->SetPrimitiveType(PrimitiveType::LineList);
			_lineGrid->GetMaterial()->SetAlphaBlend(true);
			_lineGrid->GetTransform()->SetLocalRotation(Quaternion(Vector3(EditorConfig::Is3D ? 0.0f : -90.0f, 0.0f, 0.0f)));
		}
		_lineGrid->GetGameObject()->SetActive(true);

		const int GRID_SIZE = 100;
		int distance = (int)camera->GetTransform()->GetPosition().y;//根据相机Y轴高度决定网格间距
		distance = distance / 10 * 2;

		if (distance < 1)distance = 1;

		static float oldDistance = 0.0f;
		if (oldDistance != distance)
		{//需要重新构建网格
			oldDistance = distance;
			{
				_lineGrid->Begin();
				for (int row = -GRID_SIZE; row < GRID_SIZE; ++row)
				{//横向
					Color color = Color(0.2f, 0.2f, 0.2f, 0.2f);
					if (row % 10 == 0)color = Color(0.4f, 0.4f, 0.4f, 0.4f);
					Vector3 start(-GRID_SIZE * distance, 0, row*distance);
					Vector3 end(GRID_SIZE * distance, 0, row*distance);
					_lineGrid->AddVertex(start, color);
					_lineGrid->AddVertex(end, color);
				}
				for (int col = -GRID_SIZE; col < GRID_SIZE; ++col)
				{//纵向
					Color color = Color(0.2f, 0.2f, 0.2f, 0.2f);
					if (col % 10 == 0)color = Color(0.4f, 0.4f, 0.4f, 0.4f);
					Vector3 start(col * distance, 0, -GRID_SIZE * distance);
					Vector3 end(col * distance, 0, GRID_SIZE * distance);
					_lineGrid->AddVertex(start, color);
					_lineGrid->AddVertex(end, color);
				}
				_lineGrid->End();
			}
		}
	}
	else
	{
		if (_lineGrid)
		{
			_lineGrid->GetGameObject()->SetActive(false);
		}
	}
}
Camera* EMain_SceneView::GetCamera()
{
	if (!_camera)
	{
		GameObject* cameraObj = GameObject::Instantiation("Scene 3DCamera", SceneManager::GetEngineObject()->GetTransform());
		cameraObj->InsertFlag(GameObjectFlag::EditorCamera);
		cameraObj->InsertFlag(GameObjectFlag::VisibleEditorCamera);
		cameraObj->GetTransform()->SetLocalPosition(Vector3(0, 10, -10));
		cameraObj->GetTransform()->LookAt(Vector3(0, 0, 0), Vector3::up);

		//相机
		_camera = cameraObj->AddComponent<Camera>();
		_camera->SetAspect(1.0f);
		_camera->SetSkyBox();
		//_camera->SetClearColor(Color::Clear);

		//后处理
		cameraObj->AddComponent<PostProcess>();

		Set3D(EditorConfig::Is3D);
	}
	return _camera;
}
void EMain_SceneView::Set3D(bool is_3d)
{
	Transform* transform = _camera->GetTransform();
	static Vector3 position = transform->GetPosition();
	static Vector3 forward = transform->GetForward();
	if (!is_3d)
	{
		position = transform->GetPosition();
		forward = transform->GetForward();
		if (EditorAppliction::GetInspectorId())
		{
			GameObject* selectObject = SceneManager::GetGameObject(EditorAppliction::GetInspectorId());
			if (selectObject)
			{
				const Vector3& target_pos = selectObject->GetTransform()->GetPosition();
				transform->SetPosition(target_pos - target_pos.Distance(position)*Vector3::forward);
				transform->LookAt(target_pos, Vector3::up);
			}
		}
		else
		{
			transform->SetPosition(-transform->GetPosition().Lenght() * Vector3::forward);
			transform->LookAt(Vector3::zero, Vector3::up);
		}
	}
	else
	{
		transform->SetPosition(position);
		transform->LookTo(forward, Vector3::up);
	}

	if (_lineGrid)
	{
		_lineGrid->GetTransform()->SetLocalRotation(Quaternion(Vector3(is_3d ? 0.0f : -90.0f, 0.0f, 0.0f)));
	}
}
void EMain_SceneView::HandleInput()
{
	Camera* camera = GetCamera();
	if (!camera)return;

	//是否点中当前窗口
	bool has_input = false;
	for (int i = int(MouseBtnID::Left); i < int(MouseBtnID::Max); ++i)
	{
		if (Input::GetMouseButtonDown(MouseBtnID(i)))
		{
			has_input = true;
			break;
		}
	}
	if (has_input)_isForcus = _isMouseIn;

	if (_isForcus)
	{
		if (!Input::GetKey(KeyCode::LeftControl) && !Input::GetKey(KeyCode::LeftShift) && !Input::GetKey(KeyCode::LeftAlt) &&
			!Input::GetKey(KeyCode::RightControl) && !Input::GetKey(KeyCode::RightShift) && !Input::GetKey(KeyCode::RightAlt))
		{//移动镜头
			float moveSpeed = EditorConfig::OperatorSpeed * 0.6f;
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

		if (Input::IsMouseMove() && Input::GetMouseButton(MouseBtnID::Mid))
		{//拖动镜头
			float offsetX = Input::mousePositionDelta.x * 0.1f;
			float offsetY = Input::mousePositionDelta.y * 0.1f;
			if (Math::Abs(offsetX) > 0)
			{
				camera->MoveLeftRight(-offsetX);
			}
			if (Math::Abs(offsetY) > 0)
			{
				camera->MoveUpDown(-offsetY);
			}
		}

		if (EditorConfig::Is3D && Input::IsMouseMove() && Input::GetMouseButton(MouseBtnID::Right))
		{//旋转镜头
			float offsetX = Input::mousePositionDelta.x;
			float offsetY = -Input::mousePositionDelta.y;
			if (Math::Abs(offsetX) > 0 || Math::Abs(offsetY) > 0)
			{
				camera->Rotate(offsetX * EditorConfig::OperatorSpeed, offsetY * EditorConfig::OperatorSpeed);
			}
		}
		
		if (Input::GetMouseButton(MouseBtnID::Mid))
		{//拖动手势
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
		}
		else if (Input::GetMouseButtonUp(MouseBtnID::Mid))
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
		}
	}
	if (_isMouseIn)
	{
		if (Input::GetScrollDelta().Lenght() > 0)
		{//滑轮
			Vector3 forward = camera->GetTransform()->GetForward();
			Vector3 offset = forward * Input::GetScrollDelta().y * EditorConfig::OperatorSpeed;
			camera->SetPosition(camera->GetTransform()->GetPosition() + offset);
		}
		if (false && Input::GetMouseButtonDown(MouseBtnID::Left))
		{//点选
			Ray ray = camera->ScreenPointToRay(Input::mousePosition);
			Debuger::Log("ScreenPointToRay:%s", ray.ToString().c_str());
			List<GameObject*> list = SceneManager::FindGameObjects(ray);
			for (const auto& obj : list)
			{
				Debuger::Log("hit object:%s", obj->GetInstanceName().c_str());
			}
		}
	}
}
void EMain_SceneView::OnDoubleClick(Transform* node)
{
	Camera* camera = GetCamera();
	if (!camera)return;

	if (EditorConfig::Is3D)
	{
		const Vector3& halfSize = node->GetBoundingBox().GetHalfSize();
		Vector3 dir = (camera->GetTransform()->GetPosition() - node->GetPosition()).Normalize();//指向相机
		Vector3 camera_pos = node->GetPosition() + dir * halfSize.Lenght() * 10.0f;
		camera->GetTransform()->SetPosition(camera_pos);
		camera->GetTransform()->LookTo(-dir, Vector3::up);
	}
	else
	{
		camera->GetTransform()->SetPosition(node->GetPosition() - Vector3::forward * 5.0f);
		camera->GetTransform()->LookTo(Vector3::forward, Vector3::up);
	}
}
DC_END_NAMESPACE