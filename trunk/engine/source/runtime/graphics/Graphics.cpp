#include "Graphics.h"
#include "runtime/graphics/Material.h"
#include "runtime/graphics/null/SwapChain.h"
#include "runtime/input/Input.h"
#include "runtime/project/QualitySettings.h"
#include "runtime/renderable/Renderer.h"
#include "runtime/scene/Camera.h"
#include "runtime/Application.h"
#include "editor/EditorAppliction.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(Graphics, Object);
Material* Graphics::_blitMaterial = nullptr;
VariablePrimitive* Graphics::_primitive = nullptr;
void Graphics::Destroy()
{
	SAFE_RELEASE(_blitMaterial);
	SAFE_DELETE(_primitive);
}
void Graphics::Blit(Texture* source, RenderTexture* dest, Material* mat, int pass)
{
	Blit(nullptr, source, dest, mat, pass);
}
void Graphics::Blit(Camera* camera, Texture* source, RenderTexture* dest, Material* mat, int pass)
{
	{
		if (!_blitMaterial)
		{
			_blitMaterial = Material::Create("internal/material/PostProcessBlit.material");
			_blitMaterial->Retain();
		}
		if (!_primitive)
		{
			_primitive = dynamic_cast<VariablePrimitive*>(Primitive::Create(nullptr, PrimitiveType::TriangleList));
			_primitive->AddVertex(Vector3(-1, 1, 0.5f));
			_primitive->AddVertex(Vector3(1, 1, 0.5f));
			_primitive->AddVertex(Vector3(1, -1, 0.5f));
			_primitive->AddVertex(Vector3(-1, -1, 0.5f));
			_primitive->AddTexcoord1(Vector2(0.0f, 0.0f));
			_primitive->AddTexcoord1(Vector2(1.0f, 0.0f));
			_primitive->AddTexcoord1(Vector2(1.0f, 1.0f));
			_primitive->AddTexcoord1(Vector2(0.0f, 1.0f));
			_primitive->SetIndices({ 0, 1, 2, 0, 2, 3 });
			_primitive->UploadData(PrimitiveUploadDesc());
		}
	}

	if (!mat)mat = _blitMaterial;

	//shader处理
	mat->SetTexture(source, pass);

	RenderFrameDesc desc;
	desc.clear_flag = ClearFlag::SolidColor;
	desc.clear_color = Color::Clear;
	if (dest)
	{
		dest->PreRender();
		dest->BeginFrame(desc);
	}
	else
	{//拷贝到默认缓冲区
		Application::GetGraphics()->GetSwapChain()->BeginFrame(desc);//防止深度缓存开启写入不进去
	}

	{
		Application::GetGraphics()->GetRenderContent()->RenderOnePrimitive(camera, mat->GetPass(pass), _primitive, RenderMode::Blit);
	}

	if (dest)
	{
		dest->EndFrame();
		dest->PostRender();
	}
	else
	{
		Application::GetGraphics()->GetSwapChain()->EndFrame();
	}
}
DC_END_NAMESPACE