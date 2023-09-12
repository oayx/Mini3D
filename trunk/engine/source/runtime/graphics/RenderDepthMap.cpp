#include "RenderDepthMap.h"
#include "Material.h"
#include "runtime/graphics/null/CGProgram.h"
#include "runtime/graphics/null/RenderTexture.h"
#include "runtime/input/Input.h"
#include "runtime/scene/Camera.h"
#include "runtime/scene/SceneManager.h"
#include "runtime/scene/Light.h"
#include "runtime/Application.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(RenderDepthMap, Object);
RenderDepthMap::RenderDepthMap(DepthTextureMode mode, int w, int h, ColorFormat format)
	: _depthTextureMode(mode)
{
	TextureDesc desc;
	desc.width = w; desc.height = h; desc.format = format;
	desc.flags = TextureFlag::COLOR_AND_DEPTH;
	_renderTexture = Application::GetGraphics()->CreateRenderTexture(desc);
	_renderTexture->Retain();
	this->CreateMaterial();
}
RenderDepthMap::~RenderDepthMap()
{
	SAFE_RELEASE(_renderTexture);
	SAFE_RELEASE(_defaultMaterial);
}
void RenderDepthMap::PreRender()
{
	RenderFrameDesc desc;
	desc.clear_flag = ClearFlag::SolidColor;
	desc.clear_color = Color::White;//这里一定要用1去填充，表示最远处
	_renderTexture->PreRender();
	_renderTexture->BeginFrame(desc);
}
void RenderDepthMap::PostRender()
{
	_renderTexture->EndFrame();
	_renderTexture->PostRender();

	if (Input::GetKeyDown(KeyCode::Alpha5))
		_renderTexture->SaveToFile("depth.tga", ImageType::TGA);
}
void RenderDepthMap::RenderOneObject(Camera* camera, Renderer *renderable)
{
	if (!camera || !renderable)return;
	if (!renderable->IsEnable())
		return;

	//获得shadow材质
	Material* curr_material = nullptr;
	Material* material = renderable->GetMaterial();//TODO:只获得第一个材质
	for (int i = 0; i < material->GetPassCount(); ++i)
	{
		Pass* pass = material->GetPass(i);
		if ((pass->LightMode.Equals("Depth", true) && _depthTextureMode == DepthTextureMode::Depth)
			|| (pass->LightMode.Equals("DepthNormal", true) && (_depthTextureMode & DepthTextureMode::Depth && _depthTextureMode & DepthTextureMode::Normals)))
		{
			curr_material = material;
			break;
		}
	}
	if (!curr_material)curr_material = _defaultMaterial;

	RenderMode mode = (_depthTextureMode == DepthTextureMode::Depth ? RenderMode::Depth : RenderMode::DepthNormal);
	Application::GetGraphics()->GetRenderContent()->RenderOneObject(camera, curr_material, renderable, mode);
}
void RenderDepthMap::CreateMaterial()
{
	if (_depthTextureMode & DepthTextureMode::Depth && _depthTextureMode & DepthTextureMode::Normals)
	{
		_defaultMaterial = Material::Create("internal/material/RenderDepthNormalMap.material");
		_defaultMaterial->Retain();
	}
	else if (_depthTextureMode & DepthTextureMode::Depth)
	{
		_defaultMaterial = Material::Create("internal/material/RenderDepthMap.material");
		_defaultMaterial->Retain();
	}
	else
	{
		Debuger::Error("Unknow depth mode:%d", int(_depthTextureMode));
	}
}
DC_END_NAMESPACE
