#include "RenderContent.h"
#include "RenderTexture.h"
#include "SwapChain.h"
#include "GraphicsDevice.h"
#include "runtime/graphics/Material.h"
#include "runtime/renderable/Renderer.h"
#include "runtime/Application.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(RenderContent, Object);
RenderContent::RenderContent()
{
}
RenderContent::~RenderContent()
{
	RenderTexture::Free();
	Material::ReleaseDefault();
	_currRenderWindow = nullptr;
	SAFE_RELEASE(Texture::_blackTexture); 
	SAFE_RELEASE(Texture::_whiteTexture);
}
void RenderContent::PreRender(RenderWindow* window)
{
	_currRenderWindow = window;
	this->TriggerEvent(PreCommandEvt);
}
void RenderContent::PostRender()
{
	this->TriggerEvent(PostCommandEvt);
}	
void RenderContent::RenderOneObject(Camera* camera, Material* material, Renderer *renderable, RenderMode mode)
{
	for (int i = 0; i < renderable->GetPrimitiveCount(); ++i)
	{
		Primitive* primitive = renderable->GetPrimitive(i);
		RenderOnePrimitive(camera, material, primitive, mode);
	}
}
void RenderContent::RenderOneObject(Camera* camera, Vector<Material*> materials, Renderer *renderable, RenderMode mode)
{
	for (int i = 0; i < renderable->GetPrimitiveCount(); ++i)
	{
		Material* material = i >= materials.Size() ? materials[0] : materials[i];
		Primitive* primitive = renderable->GetPrimitive(i);
		RenderOnePrimitive(camera, material, primitive, mode);
	}
}
void RenderContent::RenderOneObject(Camera* camera, Pass* pass, Renderer *renderable, RenderMode mode)
{
	for (int i = 0; i < renderable->GetPrimitiveCount(); ++i)
	{
		Primitive* primitive = renderable->GetPrimitive(i);
		RenderOnePrimitive(camera, pass, primitive, mode);
	}
}
void RenderContent::RenderOnePrimitive(Camera* camera, Material* material, Primitive *primitive, RenderMode mode)
{
	int pass_count = material->GetPassCount();
	for (int i = 0; i < pass_count; ++i)
	{
		Pass* pass = material->GetPass(i);
		switch (mode)
		{
		case RenderMode::Normal:
		case RenderMode::Reflect:
		case RenderMode::Blit:
			if (!pass->LightMode.Equals("ForwardBase", true))continue;//TODO
			break;
		case RenderMode::Depth:
			if (!pass->LightMode.Equals("Depth", true))continue;
			break;
		case RenderMode::DepthNormal:
			if (!pass->LightMode.Equals("DepthNormal", true))continue;
			break;
		case RenderMode::ShadowMap:
			if (!pass->LightMode.Equals("ShadowCaster", true))continue;
			break;
		default: break;
		}

		RenderOnePrimitive(camera, pass, primitive, mode);
	}
}
void RenderContent::Resize(const WindowResizeDesc& desc)
{
	//释放所有target
	RenderTexture::Free();
}
DC_END_NAMESPACE
