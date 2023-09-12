#include "Camera.h"
#include "Light.h"
#include "RenderQueue.h"
#include "core/geometry/Plane.h"
#include "runtime/component/Transform.h"
#include "runtime/component/Component.inl"
#include "runtime/input/Input.h"
#include "runtime/graphics/Graphics.h"
#include "runtime/graphics/Material.h"
#include "runtime/graphics/ShadowMap.h"
#include "runtime/graphics/RenderDepthMap.h"
#include "runtime/graphics/ReflectionProbe.h"
#include "runtime/graphics/null/PostProcess.h"
#include "runtime/graphics/null/RenderTexture.h"
#include "runtime/graphics/null/RenderContent.h"
#include "runtime/graphics/null/SwapChain.h"
#include "runtime/project/QualitySettings.h"
#include "runtime/renderable/Gizmos.h"
#include "runtime/renderable/SkyBox.h"
#include "runtime/scene/SceneManager.h"
#include "runtime/scene/tree/Scenetree.h"
#include "runtime/ui/UICanvas.h"
#include "runtime/Application.h"
#include "platform/WindowManager.h"
#include "editor/EditorDefine.h"
#include "editor/main/EMain_GameView.h"
#include "editor/common/ECommonComponent.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(Camera, Component);
Camera::Camera()
{
	_viewPort.Set(0.0f, 0.0f, 1.0f, 1.0f);
	_orthographicSize.Set(5, 5);
	_frustum = Frustum::Create();
	_renderQueue = RenderQueue::Create();
	SceneManager::AddCamera(this);
}
Camera::~Camera()
{
	_postProcess = nullptr;
	SceneManager::RemoveCamera(this);
	SAFE_DELETE(_skyBox);
	SAFE_DELETE(_frustum);
	SAFE_DELETE(_renderQueue);
	SAFE_DELETE(_renderDepthMap);
	SAFE_RELEASE(_renderTexture);
	SAFE_RELEASE(_hDRMaterial);
	SAFE_RELEASE(_hDRRenderTexture);
}
void Camera::OnAddComponent(Component* com)
{
	base::OnAddComponent(com);
	if (com && com->Is<PostProcess>())
	{
		_postProcess = dynamic_cast<PostProcess*>(com);
	}
}
void Camera::OnRemoveComponent(Component* com)
{
	base::OnRemoveComponent(com);
	if (com && com->Is<PostProcess>())
	{
		_postProcess = nullptr;
	}
}
Object* Camera::Clone(Object* new_obj)
{
	base::Clone(new_obj);
	Camera* obj = dynamic_cast<Camera*>(new_obj);
	if (!obj)return new_obj;

	obj->SetFov(m_fFov);
	obj->SetAspect(m_fAspect);
	obj->SetZFar(m_fZFar);
	obj->SetZNear(m_fZNear);
	obj->SetOrthographic(_isOrthographic);
	obj->SetOrthographicSize(_orthographicSize.width, _orthographicSize.height);

	obj->_clearColor = _clearColor;
	obj->SetClearFlag(_clearFlag);
	obj->SetViewport(_viewPort.x, _viewPort.y, _viewPort.w, _viewPort.h, _viewPort.z_near, _viewPort.z_far);
	obj->SetDepth(m_nDepth);
	obj->SetCullMask(_cullMask);
	obj->SetTargetDisplay(_targetDisplay);

	return obj;
}
const Matrix4 &Camera::GetViewMatrix()
{
	if (_dirtyViewMatrix)
	{
		m_matView = Matrix4::LookTo(this->GetTransform()->GetPosition(), this->GetTransform()->GetForward(), this->GetTransform()->GetUp());
		_dirtyViewMatrix = false;
	}
	return m_matView;
}
const Matrix4 &Camera::GetProjMatrix()
{
	if (_dirtyProjMatrix)
	{
		if (_isOrthographic)
			m_matProj = Matrix4::Ortho(_orthographicSize.width, _orthographicSize.height, m_fZNear, m_fZFar);
		else
			m_matProj = Matrix4::Perspective(m_fFov, m_fAspect, m_fZNear, m_fZFar);
		_dirtyProjMatrix = false;
	}
	return m_matProj;
}
const Matrix4 &Camera::GetViewProjMatrix()
{
	m_matViewProj = GetViewMatrix() * GetProjMatrix();
	return m_matViewProj;
}
void Camera::Resize(const WindowResizeDesc& desc)
{
	m_fAspect = (float)desc.width / (float)desc.height;
	_dirtyProjMatrix = true;

	if (_renderTexture)
	{
		ColorFormat format = _renderTexture->GetFormat();
		SAFE_RELEASE(_renderTexture);

		_renderTextureDesc.width = desc.width;
		_renderTextureDesc.height = desc.height;

		_renderTexture = Application::GetGraphics()->CreateRenderTexture(_renderTextureDesc);
		_renderTexture->Retain();
	}

	if (_postProcess)
	{
		_postProcess->Resize(desc.width, desc.height);
	}
}
void Camera::FindVisibleObjects()
{
	//先清空渲染队列
	ClearRenderable();

	//视锥体剔除，把可视物体添加到相机渲染队列
	GetSceneTree()->FindVisibleObjects(this, false);
}
void Camera::AddRenderable(Renderer *obj)
{
	if (obj == nullptr)return;
	_renderQueue->AddRenderable(this, obj);
	Material* material = obj->GetMaterial();
	if (material->IsCastShadow() || material->IsReceiveShadow())
	{
		_renderableBoundingBox.Merge(obj->GetTransform()->GetBoundingBox());
	}
}
void Camera::ClearRenderable()
{
	_renderableBoundingBox.SetNULL();
	_renderQueue->Clear();
}
Frustum* Camera::GetFrustum()
{
	if (_dirtyFrustum)
	{
		_frustum->Calculate(this->GetViewMatrix(), this->GetProjMatrix());
		_dirtyFrustum = false;
	}
	return _frustum;
}
FrustumVisible Camera::GetVisibility(const Aabb &bound)
{
	return GetFrustum()->GetVisibility(bound);
}
//世界空间中的视口乘以相机的View和Projection矩阵后会被转换到齐次空间中变为单位正六面体，
//那我们把单位正六面体的顶点乘上Projection和View矩阵的逆矩阵就可以算出八个顶点在世界空间中的位置了
Vector3v Camera::GetFrustumCornersInWorldSpace()
{
	const Matrix4& mat_view = GetViewMatrix();
	const Matrix4& mat_proj = GetProjMatrix();
	Matrix4 inv_view_proj = (mat_view * mat_proj).Inverse();

	Vector3 vecFrustum[8];
	vecFrustum[0] = Vector3(-1.0f, -1.0f, 0.0f);
	vecFrustum[1] = Vector3(1.0f, -1.0f, 0.0f);
	vecFrustum[2] = Vector3(-1.0f, 1.0f, 0.0f);
	vecFrustum[3] = Vector3(1.0f, 1.0f, 0.0f);
	vecFrustum[4] = Vector3(-1.0f, -1.0f, 1.0f);
	vecFrustum[5] = Vector3(1.0f, -1.0f, 1.0f);
	vecFrustum[6] = Vector3(-1.0f, 1.0f, 1.0f);
	vecFrustum[7] = Vector3(1.0f, 1.0f, 1.0f);

	for (int i = 0; i < 8; i++) 
	{
		vecFrustum[i] = inv_view_proj.TransformPoint(vecFrustum[i]);
	}

	Vector3v list; list.Reserve(8);
	list.Add(vecFrustum[0]);
	list.Add(vecFrustum[1]);
	list.Add(vecFrustum[2]);
	list.Add(vecFrustum[3]);
	list.Add(vecFrustum[4]);
	list.Add(vecFrustum[5]);
	list.Add(vecFrustum[6]);
	list.Add(vecFrustum[7]);
	return list;
}
int Camera::GetRenderTargetWidth() const
{
	if (_renderTexture != nullptr)
	{
		return _renderTexture->GetWidth();
	}
	else
	{
		return (int)WindowManager::GetWindow(_targetDisplay)->GetWidth();
	}
}
int Camera::GetRenderTargetHeight() const
{
	if (_renderTexture != nullptr)
	{
		return _renderTexture->GetHeight();
	}
	else
	{
		return (int)WindowManager::GetWindow(_targetDisplay)->GetHeight();
	}
}
Vector3 Camera::ScreenToViewportPoint(const Vector3& position)
{
	ViewPortDesc desc = this->GetViewPort();
	float screen_w = this->GetRenderTargetWidth(), screen_h = this->GetRenderTargetHeight();
	float x = (position.x / screen_w - desc.x) / (desc.w / screen_w);
	float y = (position.y / screen_h - desc.y) / (desc.h / screen_h);
	return Vector3(x, y, position.z);
}
Vector3 Camera::ViewportToScreenPoint(const Vector3& position)
{
	ViewPortDesc desc = this->GetViewPort();
	float x = (position.x * desc.w + desc.x) * this->GetRenderTargetWidth();
	float y = (position.y * desc.h + desc.y) * this->GetRenderTargetHeight();
	return Vector3(x, y, position.z);
}
Vector3 Camera::ScreenToWorldPoint(const Vector3& position)
{
	Vector3 pos_viewport = this->ScreenToViewportPoint(position);
	Vector3 pos_proj = pos_viewport * 2.0f - Vector3(1.0f, 1.0f, 0);
	Matrix4 vp_inverse = (this->GetViewMatrix() * this->GetProjMatrix()).Inverse();
	if (this->IsOrthographic())
	{
		Vector4 pos_world = Vector4(pos_proj.x, pos_proj.y, 0, 1.0f) * vp_inverse;
		pos_world *= 1.0f / pos_world.w;

		Vector3 origin = Vector3(pos_world.x, pos_world.y, pos_world.z);
		Vector3 direction = this->GetTransform()->GetForward();

		Ray ray_screen(origin, direction);
		float ray_len = position.z - m_fZNear;
		return ray_screen.GetPoint(ray_len);
	}
	else
	{
		Vector4 pos_world = Vector4(pos_proj.x, pos_proj.y, -1.0f, 1.0f) * vp_inverse;
		pos_world *= 1.0f / pos_world.w;

		Vector3 origin = this->GetTransform()->GetPosition();
		Vector3 direction = Vector3(pos_world.x, pos_world.y, pos_world.z) - origin;

		Ray ray_screen(origin, direction);
		Ray ray_forward(origin, this->GetTransform()->GetForward());
		Plane plane(ray_forward.dir.Normalize(), ray_forward.GetPoint(position.z));
		std::pair<bool, float> hit = ray_screen.Intersects(plane);
		if (!hit.first)
		{
			Debuger::Error("Camera::ScreenToWorldPoint");
		}
		return ray_screen.GetPoint(hit.second);
	}
}
Vector3 Camera::WorldToScreenPoint(const Vector3& position)
{
	Vector3 pos_view = this->GetViewMatrix().TransformPoint(position);
	Vector3 pos_proj = this->GetProjMatrix().TransformPoint(pos_view);
	Vector3 pos_viewport = (pos_proj + Vector3(1.0f, 1.0f, 0)) * 0.5f;
	pos_viewport.z = pos_view.z;
	return this->ViewportToScreenPoint(pos_viewport);
}
Vector3 Camera::WorldToViewportPoint(const Vector3& position)
{
	Vector3 screen_point = WorldToScreenPoint(position);
	return ScreenToViewportPoint(screen_point);
}
Ray Camera::ScreenPointToRay(const Vector3& position)
{
	Vector3 pos_world = this->ScreenToWorldPoint(Vector3(position.x, position.y, m_fZNear));
	Vector3 origin = pos_world;
	Vector3 direction;

	if (this->IsOrthographic())
	{
		direction = this->GetTransform()->GetForward();
	}
	else
	{
		direction = origin - this->GetTransform()->GetPosition();
	}

	return Ray(origin, direction.Normalize());
}
void Camera::SetClearFlag(ClearFlag flag) 
{
	_clearFlag = flag;
	if (ClearFlag::Skybox != flag)
	{//清除天空盒
		SAFE_DELETE(_skyBox);
	}
	else if (!_skyBox)
	{
		SetSkyBox();
	}
}
void Camera::SetSkyBox(const String& file)
{
	if (!_skyBox)
	{
		_skyBox = SkyBox::Create();
	}
	String sky_material = file;
	if (file.IsEmpty())
	{
		Scene* curr_scene = SceneManager::GetCurrScene();
		if (curr_scene)sky_material = curr_scene->GetSkyboxMaterial();
	}
	_skyBox->SetCubeFile(sky_material);
	SetClearFlag(ClearFlag::Skybox);
}
Texture* Camera::GetSkyBoxTexture()const
{
	if (_skyBox != nullptr)
		return _skyBox->GetCubeTexture();
	else
		return nullptr;
}
void Camera::SetViewport(float w, float h) 
{ 
	AssertEx(w >= 0.0f && w <= 1.0f && h >= 0.0f && h <= 1.0f, ""); 
	_viewPort.Set(w, h);
}
void Camera::SetViewport(float x, float y, float w, float h) 
{
	AssertEx(x >= 0.0f && x <= 1.0f && y >= 0.0f && y <= 1.0f, "");
	AssertEx(w >= 0.0f && w <= 1.0f && h >= 0.0f && h <= 1.0f, "");
	_viewPort.Set(x, y, w, h);
}
void Camera::SetViewport(float x, float y, float w, float h, float min_z, float max_z)
{
	AssertEx(x >= 0.0f && x <= 1.0f && y >= 0.0f && y <= 1.0f, "");
	AssertEx(w >= 0.0f && w <= 1.0f && h >= 0.0f && h <= 1.0f, "");
	AssertEx(min_z >= 0.0f && min_z <= 1.0f && max_z >= 0.0f && max_z <= 1.0f, "");
	_viewPort.Set(x, y, w, h, min_z, max_z);
	_dirtyProjMatrix = true;
}
ViewPortDesc Camera::GetViewPort()const 
{ 
	float width = this->GetRenderTargetWidth();
	float height = this->GetRenderTargetHeight();
	ViewPortDesc desc;
	desc.x = _viewPort.x * width;
	desc.y = _viewPort.y * height;
	desc.w = _viewPort.w * width;
	desc.h = _viewPort.h * height;
	desc.z_near = _viewPort.z_near;
	desc.z_far = _viewPort.z_far;
	return desc;
}
bool Camera::CanRemove()
{
	if (GetComponent<PostProcess>())
		return false;
	//作为UICamera
	UICanvas* canvas = this->GetComponent<UICanvas>();
	return !canvas || canvas->GetRenderMode() != UIRenderMode::ScreenSpace_Camera;
}
void Camera::PreRender()
{
}
void Camera::Render()
{
	DC_PROFILE_FUNCTION();
	if (this->IsSame<ReflectionProbe>())
	{//反射探头
		this->RenderReflectionProbe();
	}
	else
	{
		//找出可见对象
		this->FindVisibleObjects();

		//阴影图
		for (int i = 0; i < SceneManager::GetLightCount(); ++i)
		{
			Light* light = SceneManager::GetLight(i);
			if (light && light->IsEnable() && light->GetGameObject()->ActiveInHierarchy())
			{
				this->RenderShadowTexture(light);
			}
		}

		//深度图
		this->RenderDepthTexture();

		if (_enableHDR && _hDRRenderTexture)
		{
			this->RenderToTexture(_hDRRenderTexture);
		}
		else
		{//普通渲染
			bool has_postprocess = _postProcess && _postProcess->HasEffect() && _postProcess->IsEnable() && _postProcess->GetGameObject()->ActiveInHierarchy();
			if (_renderTexture || has_postprocess)
			{//渲染到纹理
				if (has_postprocess)
				{
					this->RenderToTexture(_postProcess->GetRenderTexture());
				}
				else if (_renderTexture)
				{//渲染到纹理和后处理都有的情况下，后处理的结果会渲染到纹理
					this->RenderToTexture(_renderTexture);
				}
			}
			else if (Application::IsEditor() && EMain_GameView::GetRenderTexture())
			{
				this->RenderToTexture(EMain_GameView::GetRenderTexture());
			}
			else
			{
				this->RenderNormal();
			}
		}
	}
}
void Camera::PostRender()
{
	bool has_postprocess = _postProcess && _postProcess->HasEffect() && _postProcess->IsEnable() && _postProcess->GetGameObject()->ActiveInHierarchy();

	if (_enableHDR && _hDRRenderTexture)
	{//色调映射
		if (!_hDRMaterial)
		{
			_hDRMaterial = Material::Create("internal/material/HDRToneMapping.material");
			_hDRMaterial->Retain();
		}
		RenderTexture* dest_texture = nullptr;
		if (_renderTexture || has_postprocess)
		{//渲染到纹理
			if (has_postprocess)
			{
				dest_texture = _postProcess->GetRenderTexture();
			}
			else if (_renderTexture)
			{//渲染到纹理和后处理都有的情况下，后处理的结果会渲染到纹理
				dest_texture = _renderTexture;
			}
		}
		else if (Application::IsEditor() && EMain_GameView::GetRenderTexture())
		{
			dest_texture = EMain_GameView::GetRenderTexture();
		}
		_hDRMaterial->SetFloat("_Exposure", QualitySettings::GetHDRExposure());
		Graphics::Blit(_hDRRenderTexture, dest_texture, _hDRMaterial);
	}

	if (has_postprocess)
	{//TODO:需要在HDR tonemapping前执行
		_postProcess->Render(this);
	}
}
void Camera::RenderNormal()
{
	ClearFlag flag = _clearFlag;
	if (_clearFlag == ClearFlag::Skybox && _skyBox == nullptr)
	{
		Debuger::Error("cannot find skybox");
		flag = ClearFlag::SolidColor;
	}

	RenderFrameDesc desc;
	desc.view_port = this->GetViewPort();
	desc.clear_flag = flag;
	desc.clear_color = _clearColor;
	Application::GetGraphics()->GetSwapChain()->BeginFrame(desc);

	if (_gameObject->HasFlag(GameObjectFlag::EditorCamera))Gizmos::PreRender(this);
	if (_skyBox && _clearFlag == ClearFlag::Skybox)_skyBox->Render(this);
	_renderQueue->Render(this);
	if (_gameObject->HasFlag(GameObjectFlag::EditorCamera))Gizmos::PostRender(this);

	Application::GetGraphics()->GetSwapChain()->EndFrame();
}
void Camera::RenderToTexture(RenderTexture* render_texture)
{
	CHECK_RETURN_PTR_VOID(render_texture);

	RenderFrameDesc desc;
	desc.clear_flag = _clearFlag;
	desc.clear_color = _clearColor;
	render_texture->PreRender();
	render_texture->BeginFrame(desc);

	if (_gameObject->HasFlag(GameObjectFlag::EditorCamera))Gizmos::PreRender(this);
	if (_skyBox && _clearFlag == ClearFlag::Skybox)_skyBox->Render(this);
	_renderQueue->Render(this);
	if (_gameObject->HasFlag(GameObjectFlag::EditorCamera))Gizmos::PostRender(this);

	render_texture->EndFrame();
	render_texture->PostRender();

	if (Input::GetKeyDown(KeyCode::Alpha4))
		render_texture->SaveToFile("rtt.png", ImageType::PNG);
}
void Camera::RenderDepthTexture()
{
	if (GetDepthTextureMode() != DepthTextureMode::None)
	{
		_renderDepthMap->PreRender();
		_renderQueue->RenderDepthTexture(this, _renderDepthMap);
		_renderDepthMap->PostRender();
	}
}
void Camera::RenderShadowTexture(Light* light)
{
	ShadowMap* shadow_map = light->GetShadowMap();
	if (shadow_map && light->GetValidShadowType() != ShadowType::None)
	{
		shadow_map->PreRender(this, light);
		_renderQueue->RenderShadowTexture(this, light);
		shadow_map->PostRender();
	}
}
bool Camera::CreateRenderTexture(const TextureDesc& desc)
{
	SAFE_RELEASE(_renderTexture);

	_renderTextureDesc = desc;
	_renderTexture = Application::GetGraphics()->CreateRenderTexture(desc);
	_renderTexture->Retain();
	return true;
}
void Camera::AddDepthTextureMode(DepthTextureMode mode)
{
	if (mode == DepthTextureMode::None || _depthTextureMode & mode)return;

	_depthTextureMode |= mode; 
	SAFE_DELETE(_renderDepthMap);
	_renderDepthMap = RenderDepthMap::Create(_depthTextureMode, GetRenderTargetWidth(), GetRenderTargetHeight(), ColorFormat::RGBA16F);
}
void Camera::RemoveDepthTextureMode(DepthTextureMode mode)
{
	if (mode == DepthTextureMode::None || !(_depthTextureMode & mode))return;

	_depthTextureMode ^= mode;
	if (_depthTextureMode == DepthTextureMode::None)
	{
		SAFE_DELETE(_renderDepthMap);
	}
}
void Camera::ClearDepthTextureMode()
{
	_depthTextureMode = DepthTextureMode::None;
	SAFE_DELETE(_renderDepthMap);
}
RenderTexture* Camera::GetDepthTexture()const 
{
	return _renderDepthMap->GetTexture();
}
bool Camera::AddPostEffect(const Type* type)
{
	if (_postProcess)
		return _postProcess->AddEffect(type) != nullptr;
	else
		return false;
}
void Camera::RemovePostEffect(const Type* type)
{
	if (_postProcess)
	{
		_postProcess->RemoveEffect(type);
	}
}
void Camera::ClearPostEffect()
{
	if (_postProcess)
	{
		_postProcess->ClearEffect();
	}
}
void Camera::EnableHDR(bool enable)
{
	if (enable)
	{
		_enableHDR = true;
		if (QualitySettings::IsEnableHDR())
		{
			if (!_hDRRenderTexture)
			{
				TextureDesc desc;
				desc.width = GetRenderTargetWidth(); desc.height = GetRenderTargetHeight(); desc.antiAlias = _enableMSAA ? int(QualitySettings::GetMSAAType()) : 1;
				desc.format = QualitySettings::GetHDRFormat();
				desc.flags = TextureFlag::COLOR_AND_DEPTH;
				_hDRRenderTexture = Application::GetGraphics()->CreateRenderTexture(desc);
				_hDRRenderTexture->Retain();
			}
		}
	}
	else
	{
		_enableHDR = false;
		SAFE_RELEASE(_hDRRenderTexture);
	}
}
void Camera::EnableMSAA(bool enable)
{
	_enableMSAA = enable;
	if (_enableMSAA)
	{
		if (Application::IsEditor())
		{
			EMain_GameView::EnableMSAA(true);
		}
	}
	else
	{
		if (Application::IsEditor())
		{
			EMain_GameView::EnableMSAA(false);
		}
	}
}
void Camera::SetPosition(float x, float y, float z)
{
	GetTransform()->SetPosition(Vector3(x, y, z));
}
void Camera::SetPosition(const Vector3& pos)
{
	GetTransform()->SetPosition(pos);
}
void Camera::MoveLeftRight(float d)
{
	Vector3 right_dir = GetTransform()->GetRight();
	GetTransform()->Translate(right_dir.Normalize(d));
}
void Camera::MoveForwardBack(float d)
{
	Vector3 front_dir = GetTransform()->GetForward();
	GetTransform()->Translate(front_dir.Normalize(d));
}
void Camera::MoveUpDown(float d)
{
	Vector3 front_dir = GetTransform()->GetUp();
	GetTransform()->Translate(front_dir.Normalize(d));
}
void Camera::Pitch(float degree)
{
	Vector3 euler = GetTransform()->GetLocalRotation().ToEuler();
	euler.x += degree;
	Quaternion q(euler);
	GetTransform()->SetLocalRotation(q);
}
void Camera::Yaw(float degree)
{
	Vector3 euler = GetTransform()->GetLocalRotation().ToEuler();
	euler.y += degree;
	Quaternion q(euler);
	GetTransform()->SetLocalRotation(q);
}
void Camera::Rotate(float x_degree, float y_degree)
{
	Vector3 euler = GetTransform()->GetLocalRotation().ToEuler();
	euler.x += y_degree;
	euler.y += x_degree;
	Quaternion q(euler);
	GetTransform()->SetLocalRotation(q);
}
void Camera::OnDrawEditor()
{
	base::OnDrawEditor();

	{//Clear Flags
		const char* sz_flags[] = { "Skybox", "Solid Color", "Depth Only", "Don't Clear" };
		ImGuiEx::Label("Clear Flags");
		static int current_flags = 0;
		switch (_clearFlag)
		{
		case ClearFlag::Skybox: current_flags = 0; break;
		case ClearFlag::SolidColor:current_flags = 1; break;
		case ClearFlag::DepthOnly:current_flags = 2; break;
		case ClearFlag::DonotClear:current_flags = 3; break;
		}
		if (ImGui::Combo("##Clear Flags", &current_flags, sz_flags, ARRAY_SIZE(sz_flags)))
		{
			switch (current_flags)
			{
			case 0: this->SetSkyBox(); break;
			case 1:	this->SetClearFlag(ClearFlag::SolidColor); break;
			case 2:	this->SetClearFlag(ClearFlag::DepthOnly); break;
			case 3:	this->SetClearFlag(ClearFlag::DonotClear); break;
			}
		}
		if (current_flags == 1)
		{//Solid Color
			ImGuiEx::Label("Background");
			ECommonComponent::ShowColor("Background", _clearColor.p, false);
		}
	}
	{//Color Mask
		ImGuiEx::Label("Color Mask");
		ECommonComponent::ShowCullmask("Color Mask", _cullMask);
	}
	{//Projection
		ImGui::NewLine();
		const char* sz_flags[] = { "Projection", "Orthographic" };
		ImGuiEx::Label("Projection");
		static int current_flags = 0;
		if (_isOrthographic)current_flags = 1;
		else current_flags = 0;
		if (ImGui::Combo("##Projection", &current_flags, sz_flags, ARRAY_SIZE(sz_flags)))
		{
			switch (current_flags)
			{
			case 0: SetOrthographic(false); break;
			case 1:	SetOrthographic(true); break;
			}
		}
		if (_isOrthographic)
		{
			//ImGuiEx::Label("Size");
			//float size = _orthographicSize.width;
			//if (ImGui::InputFloat("##Size", &size))
			//{
			//	SetOrthographicSize(size, size);
			//}
		}
		else
		{
			ImGuiEx::Label("Field of View");
			int fov = m_fFov * Math::Rad2Deg;
			if (ImGui::SliderInt("FieldofView", &fov, 1, 179))
			{
				SetFieldOfView(fov);
			}
		}

		{
			ImGuiEx::Label("Clipping Planes");
			float width = ImGui::GetContentRegionAvail().x;

			ImGui::SetNextItemWidth(ImGui::CalcTextSize("Near").x);
			ImGui::TextUnformatted("Near");

			float min_znear = 0.01f;
			ImGui::SameLine();
			ImGui::SetNextItemWidth(width - ImGui::CalcTextSize("Near").x);
			if (ImGui::DragScalar("##ZNear", ImGuiDataType_Float, &m_fZNear, 0.01f, &min_znear, nullptr, "%.2f"))
			{
				if (m_fZNear < min_znear)m_fZNear = min_znear;
				SetZNear(m_fZNear);
			}

			ImGuiEx::Label("##zfar");

			ImGui::SetNextItemWidth(ImGui::CalcTextSize("Far   ").x);
			ImGui::TextUnformatted("Far   ");

			float min_zfar = m_fZNear;
			ImGui::SameLine();
			ImGui::SetNextItemWidth(width - ImGui::CalcTextSize("Far   ").x);
			if (ImGui::DragScalar("##ZFar", ImGuiDataType_Float, &m_fZFar, 0.05f, &min_zfar, nullptr, "%.2f"))
			{
				if (m_fZFar < min_zfar)m_fZFar = min_zfar;
				SetZFar(m_fZFar);
			}
		}

		{
			ImGuiEx::Label("Viewport Rect");
			const float width = ImGui::GetContentRegionAvail().x;
			const float char_width = ImGui::GetFontSize() + 3.0f;

			ImGui::TextUnformatted("X  ");

			float value = _viewPort.x;
			float min_v = 0.00f;
			float max_v = 1.00f;
			ImGui::SameLine();
			ImVec2 curr_pos1 = ImGui::GetCursorScreenPos();
			ImGui::SetNextItemWidth(width * 0.5f - char_width);
			if (ImGui::DragScalar("##X", ImGuiDataType_Float, &value, 0.01f, &min_v, &max_v, "%.2f"))
			{
				SetViewport(value, _viewPort.y, _viewPort.w, _viewPort.h);
			}

			ImGui::SameLine();
			ImGui::TextUnformatted("Y ");

			value = _viewPort.y;
			ImGui::SameLine();
			ImVec2 curr_pos2 = ImGui::GetCursorScreenPos();
			ImGui::SetNextItemWidth(width * 0.5f - char_width);
			if (ImGui::DragScalar("##Y", ImGuiDataType_Float, &value, 0.01f, &min_v, &max_v, "%.2f"))
			{
				SetViewport(_viewPort.x, value, _viewPort.w, _viewPort.h);
			}


			ImGuiEx::Label("##wh");
			ImGui::TextUnformatted("W");

			value = _viewPort.w;
			ImGui::SameLine();
			ImGui::SetCursorScreenPos(ImVec2(curr_pos1.x, ImGui::GetCursorScreenPos().y));
			ImGui::SetNextItemWidth(width * 0.5f - char_width);
			if (ImGui::DragScalar("##W", ImGuiDataType_Float, &value, 0.01f, &min_v, &max_v, "%.2f"))
			{
				SetViewport(_viewPort.x, _viewPort.y, value, _viewPort.h);
			}

			ImGui::SameLine();
			ImGui::TextUnformatted("H");

			value = _viewPort.h;
			ImGui::SameLine();
			ImGui::SetCursorScreenPos(ImVec2(curr_pos2.x, ImGui::GetCursorScreenPos().y));
			ImGui::SetNextItemWidth(width * 0.5f - char_width);
			if (ImGui::DragScalar("##H", ImGuiDataType_Float, &value, 0.01f, &min_v, &max_v, "%.2f"))
			{
				SetViewport(_viewPort.x, _viewPort.y, _viewPort.w, value);
			}
		}
	}
	{
		ImGui::NewLine();

		ImGuiEx::Label("Depth");
		if (ImGui::DragScalar("##Depth", ImGuiDataType_S32, &m_nDepth, 0.1f, nullptr, nullptr, "%.2f"))
		{
			
		}

		ImGuiEx::Label("Allow HDR");
		if (ImGui::Checkbox("##Allow HDR", &_enableHDR))
		{
			EnableHDR(_enableHDR);
		}

		ImGuiEx::Label("Allow MSAA");
		if (ImGui::Checkbox("##Allow MSAA", &_enableMSAA))
		{
			EnableMSAA(_enableMSAA);
		}
	}
}
void Camera::OnDrawGizmos(Camera* camera)
{
	if (_drawGizmos)
	{
		Color color = Gizmos::SetColor(Color(1.0f, 1.0f, 1.0f, 0.5f));
		Gizmos::DrawFrustum(this);
		Gizmos::SetColor(color);
	}
}
/********************************************************************/
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(Camera);
template<class TransferFunction> inline
void Camera::Transfer(TransferFunction& transfer, void* ptr)
{
	base::Transfer(transfer, ptr);

	TRANSFER_ENUM(_targetDisplay);
	TRANSFER_SIMPLE(m_fFov);
	TRANSFER_SIMPLE(m_fAspect);
	TRANSFER_SIMPLE(m_fZNear);
	TRANSFER_SIMPLE(m_fZFar);
	TRANSFER_SIMPLE(_isOrthographic);
	TRANSFER_SIMPLE(_orthographicSize);
	TRANSFER_ENUM(_clearFlag);
	TRANSFER_SIMPLE(_clearColor);
	TRANSFER_SIMPLE(_viewPort);
	TRANSFER_SIMPLE(m_nDepth);
	TRANSFER_SIMPLE(_cullMask);
	TRANSFER_SIMPLE(_enableHDR);
	TRANSFER_SIMPLE(_enableMSAA);

	if (transfer.IsRead())
	{
		switch (_clearFlag)
		{
		case ClearFlag::Skybox:SetSkyBox(); break;
		case ClearFlag::SolidColor:SetClearColor(_clearColor); break;
		default: break;
		}
		this->EnableHDR(_enableHDR);
		this->EnableMSAA(_enableMSAA);
	}
}
DC_END_NAMESPACE