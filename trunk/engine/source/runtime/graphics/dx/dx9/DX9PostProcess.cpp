#include "DX9PostProcess.h"
#include "DX9Device.h"
#include "DX9Program.h"
#include "DX9SwapChain.h"
#include "DX9RenderContent.h"
#include "runtime/input/Input.h"
#include "runtime/graphics/Material.h"
#include "runtime/graphics/null/HardwareBuffer.h"
#include "runtime/renderable/Renderer.h"
#include "runtime/scene/Camera.h"
#include "runtime/Application.h"
#include "editor/main/EMain_GameView.h"

DC_BEGIN_NAMESPACE
struct QuadVertex
{
	float x, y, z;
	float tu, tv;
	const static D3DVERTEXELEMENT9 Decl[3];
};
const D3DVERTEXELEMENT9 QuadVertex::Decl[3] =
{
	{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITIONT, 0 },
	{ 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,  0 },
	D3DDECL_END()
};
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(DX9FinalProcess, FinalProcess);
DX9FinalProcess::DX9FinalProcess()
{
	String shader_code = R"(
		texture g_BaseTexture;
		sampler2D _BaseTexture = sampler_state
		{
			Texture = <g_BaseTexture>;
			MipFilter = Linear;
			MinFilter = Linear;
			MagFilter = Linear;
			AddressU = Clamp;
			AddressV = Clamp;
		};
		struct v2f
		{
			float4 pos : SV_POSITION;
			float2 uv : TEXCOORD0;
		};
		v2f vs_main( float3 Pos : POSITION, float2 uv : TEXCOORD0)
		{
			v2f o;
			o.pos = float4(Pos, 1);
			o.uv = uv;
			return o;
		}
		float4 ps_main(v2f i) : SV_Target
		{
			float4 albedo = tex2D(_BaseTexture, i.uv);
			return albedo;
		}
		technique TShader
		{
			pass P0
			{
				VertexShader = compile vs_2_0 vs_main();
				PixelShader = compile ps_2_0 ps_main();
				ZEnable = false;
			}
		}
	)";

	DX9HR(D3DXCreateEffect(GetDX9Device()->GetDevice(), shader_code.c_str(), shader_code.Size(), NULL, NULL, 0, NULL, &_effect, NULL));
	DX9HR(GetDX9Device()->GetDevice()->CreateVertexDeclaration(QuadVertex::Decl, &_vertDecl));
}
DX9FinalProcess::~DX9FinalProcess()
{
	SAFE_RELEASE(_effect);
	SAFE_RELEASE(_vertDecl);
}
void DX9FinalProcess::Render(Camera* camera, RenderTexture* dest)
{
	CHECK_RETURN_PTR_VOID(dest);

	int w = camera->GetRenderTargetWidth(); 
	int h = camera->GetRenderTargetHeight();
	QuadVertex quad[4] =
	{
		{ -0.5f,		-0.5f,		0.5f, 0.0f, 0.0f},
		{ w - 0.5f,		-0.5f,		0.5f, 1.0f, 0.0f},
		{ -0.5f,		h - 0.5f,	0.5f, 0.0f, 1.0f},
		{ w - 0.5f,		h - 0.5f,	0.5f, 1.0f, 1.0f}
	};

	RenderFrameDesc desc;
	desc.viewPort = camera->GetViewPort();
	desc.clearFlag = camera->GetClearFlag();
	desc.clearColor = camera->GetClearColor();
	if (camera->GetRenderTexture())
	{//拷贝到纹理
		camera->GetRenderTexture()->PreRender();
		camera->GetRenderTexture()->BeginFrame(desc);
	}
	else if (Application::IsEditor() && EMain_GameView::GetRenderTexture())
	{
		EMain_GameView::GetRenderTexture()->PreRender();
		EMain_GameView::GetRenderTexture()->BeginFrame(desc);
	}
	else
	{//拷贝到默认缓冲区
		Application::GetGraphics()->GetSwapChain()->BeginFrame(desc);//防止深度缓存开启写入不进去
	}

	{
		GetDX9Device()->SetD3DSamplerState(0, D3DSAMP_SRGBTEXTURE, false);
		DX9HR(GetDX9Device()->GetDevice()->SetVertexDeclaration(_vertDecl));
		DX9HR(_effect->SetTechnique(_effect->GetTechnique(0)));
		DX9HR(_effect->SetTexture("g_BaseTexture", (IDirect3DTexture9*)dest->GetTexture()));

		UINT passes;
		DX9HR(_effect->Begin(&passes, 0));
		for (int p = 0; p < passes; ++p)
		{
			DX9HR(_effect->BeginPass(p));
			DX9HR(_effect->CommitChanges());
			DX9HR(GetDX9Device()->GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, quad, sizeof(QuadVertex)));
			DX9HR(_effect->EndPass());
		}
		DX9HR(_effect->End());
	}

	if (camera->GetRenderTexture())
	{
		camera->GetRenderTexture()->EndFrame();
		camera->GetRenderTexture()->PostRender();
	}
	else if (Application::IsEditor() && EMain_GameView::GetRenderTexture())
	{
		EMain_GameView::GetRenderTexture()->EndFrame();
		EMain_GameView::GetRenderTexture()->PostRender();
	}
	else
	{
		Application::GetGraphics()->GetSwapChain()->EndFrame();
	}
	//统计
	SceneManager::AddDrawCall(1);
	SceneManager::AddRenderBatch(1);
	SceneManager::AddSetPassCall(1);
	SceneManager::AddDrawTriangles(2);
}
DC_END_NAMESPACE