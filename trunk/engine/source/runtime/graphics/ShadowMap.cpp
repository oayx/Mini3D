#include "ShadowMap.h"
#include "Material.h"
#include "runtime/graphics/null/CGProgram.h"
#include "runtime/graphics/null/RenderTexture.h"
#include "runtime/input/Input.h"
#include "runtime/project/QualitySettings.h"
#include "runtime/scene/Camera.h"
#include "runtime/scene/SceneManager.h"
#include "runtime/scene/Light.h"
#include "runtime/Application.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(ShadowMap, Object);
ShadowMap::ShadowMap(int w, int h, ColorFormat format)
{
	_shadowMapSize.Set((float)w, (float)h);
	TextureDesc desc;
	desc.width = w; desc.height = h; desc.format = format; desc.address = TextureAddress::Border; desc.border = Color::White;
	desc.flags = TextureFlag::DEPTH | TextureFlag::ShadowMap;
	_renderTexture = Application::GetGraphics()->CreateRenderTexture(desc);
	_renderTexture->Retain();
}
ShadowMap::~ShadowMap()
{
	SAFE_RELEASE(_renderTexture);
	SAFE_DELETE(_defaultMaterial);
}
void ShadowMap::PreRender(Camera* camera, Light* light)
{
	//转换到灯光坐标系
	Matrix4 matView, matProj;
	Transform* light_transform = light->GetTransform();
	const Vector3& light_pos = light_transform->GetPosition();
	if (light->mType == LightType::Direction)
	{
		Aabb aabb = camera->GetRenderableBoundingBox();
		matView = Matrix4::LookTo(light_pos, light_transform->GetForward(), Vector3::up);
		aabb = aabb * matView;
		float minX = aabb.GetMinimum().x;
		float maxX = aabb.GetMaximum().x;
		float minY = aabb.GetMinimum().y;
		float maxY = aabb.GetMaximum().y;
		float w = maxX - minX, h = maxY - minY;
		float size = Math::Max<float>(w, h);
		size = Math::Clamp(size, 30.0f, QualitySettings::GetShadowDistance());
		matProj = Matrix4::Ortho(size, size, -10.0f, 1000.0f);
	}
	else
	{
		matView = Matrix4::LookTo(light_transform->GetPosition(), light_transform->GetForward(), Vector3::up);
		matProj = camera->GetProjMatrix();
	}
	_viewProj = matView * matProj;

	RenderFrameDesc desc;
	desc.clearFlag = ClearFlag::SolidColor;
	desc.clearColor = Color::White;//这里一定要用1去填充，表示最远处
	_renderTexture->PreRender();
	_renderTexture->BeginFrame(desc);
}
void ShadowMap::PostRender()
{
	_renderTexture->EndFrame();
	_renderTexture->PostRender();

	//if (Input::GetKeyDown(KeyCode::Alpha5))
	//	_renderTexture->SaveToFile("shadowmap.png", ImageType::PNG);
}
void ShadowMap::RenderOneObject(Camera* camera, Light* light, Renderer *renderable)
{
	if (!camera || !light || !renderable)return;
	if (!renderable->IsEnable())
		return;

	//是否超出阴影最大范围
	const Vector3& camera_position = camera->GetTransform()->GetPosition();
	if (camera_position.Distance(renderable->GetTransform()->GetPosition()) > QualitySettings::GetShadowDistance())
		return;

	//投射矩阵
	const Matrix4& matWorld = renderable->GetWorldMatrix();
	Matrix4 mat = matWorld * _viewProj;

	//获得shadow材质
	Material* currMaterial = nullptr;
	Material* material = renderable->GetMaterial();//TODO:只获得第一个材质
	for (int i = 0; i < material->GetPassCount(); ++i)
	{
		Pass* pass = material->GetPass(i);
		if (pass->LightMode.Equals("ShadowCaster", true))
		{
			currMaterial = material;
			break;
		}
	}
	if(!currMaterial)currMaterial = GetMaterial();
	currMaterial->SetMatrix("LIGHT_MATRIX_MVP", mat);
	Application::GetGraphics()->GetRenderContent()->RenderOneObject(camera, currMaterial, renderable, RenderMode::ShadowMap);
}
Material* ShadowMap::GetMaterial()
{
	if (!_defaultMaterial)
	{
		_defaultMaterial = Material::Create("internal/material/ShadowMap.material");
		_defaultMaterial->Retain();
	}
	return _defaultMaterial;
}
DC_END_NAMESPACE