#include "ReflectionProbe.h"
#include "Material.h"
#include "runtime/graphics/null/RenderTexture.h" 
#include "runtime/input/Input.h"
#include "runtime/scene/Camera.h"
#include "runtime/scene/SceneManager.h"
#include "runtime/scene/RenderQueue.h"
#include "runtime/Application.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(ReflectionProbe, Camera);
ReflectionProbe::ReflectionProbe()
{
	uint mask = LayerMask::Everything;
	mask ^= LayerMask::GetMask(LayerMask::UI);
	mask ^= LayerMask::GetMask(LayerMask::IgnoreRaycast);
	_cullMask = mask;

	//反射纹理
	TextureDesc desc;
	desc.type = TextureType::Cube; desc.width = _resolution; desc.height = _resolution; desc.format = ColorFormat::R8G8B8A8;
	desc.flags = TextureFlag::COLOR | TextureFlag::CUBE;
	_renderTexture = Application::GetGraphics()->CreateRenderTexture(desc);
	_renderTexture->Retain();
}
ReflectionProbe::~ReflectionProbe()
{
	SAFE_RELEASE(_renderTexture);
}
void ReflectionProbe::Start()
{
	base::Start();

	//相机信息
	base::SetClearFlag(ClearFlag::SolidColor);
	base::SetClearColor(_clearColor);
	base::SetFieldOfView(90);
	base::SetDepth(INT_MIN);

	//激活就渲染
	if (_mode == ReflectionMode::OnAwake)
	{
		this->RenderImpl();
	}
}
void ReflectionProbe::RenderReflectionProbe()
{
	if (_mode == ReflectionMode::EveryFrame)
	{
		this->RenderImpl();
	}
}
void ReflectionProbe::RenderImpl()
{
	static Vector3 looks[6] = {
		{ 1.0f, 0.0f, 0.0f },	// +X
		{ -1.0f, 0.0f, 0.0f },	// -X
		{ 0.0f, 1.0f, 0.0f },	// +Y
		{ 0.0f, -1.0f, 0.0f },	// -Y
		{ 0.0f, 0.0f, 1.0f },	// +Z
		{ 0.0f, 0.0f, -1.0f },	// -Z
	};
	static Vector3 ups[6] = {
		{ 0.0f, 1.0f, 0.0f },	// +X
		{ 0.0f, 1.0f, 0.0f },	// -X
		{ 0.0f, 0.0f, -1.0f },	// +Y
		{ 0.0f, 0.0f, 1.0f },	// -Y
		{ 0.0f, 1.0f, 0.0f },	// +Z
		{ 0.0f, 1.0f, 0.0f }	// -Z
	};

	_renderTexture->PreRender();
	for (int face = 0; face < 6; ++face)
	{
		//修改相机信息
		GetTransform()->LookTo(looks[face], ups[face]);

		//收集可渲染对象
		this->FindVisibleObjects();

		//渲染
		RenderFrameDesc desc;
		desc.face = face;
		desc.clear_flag = _clearFlag;
		desc.clear_color = _clearColor;
		_renderTexture->BeginFrame(desc);
		_renderQueue->Render(this);
		_renderTexture->EndFrame();
	}
	_renderTexture->PostRender();

	//if (Input::GetKeyDown(KeyCode::Alpha5))
	//	_renderTexture->SaveToFile("reflect.png", ImageType::PNG);
}
DC_END_NAMESPACE