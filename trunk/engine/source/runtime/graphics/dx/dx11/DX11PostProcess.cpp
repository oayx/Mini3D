#include "DX11PostProcess.h"
#include "DX11Program.h"
#include "DX11Device.h"
#include "DX11RenderContent.h"
#include "runtime/graphics/Material.h"
#include "runtime/graphics/null/HardwareBuffer.h"
#include "runtime/renderable/Renderer.h"
#include "runtime/scene/Camera.h"
#include "runtime/Application.h"
#include "editor/main/EMain_GameView.h"
 
DC_BEGIN_NAMESPACE

struct DX11ScreenVertex
{
	float x, y, z;
	float tu, tv;
	const static D3D11_INPUT_ELEMENT_DESC Decl[2];
};
const D3D11_INPUT_ELEMENT_DESC DX11ScreenVertex::Decl[2] =
{
	{ "POSITION",0, DXGI_FORMAT_R32G32B32_FLOAT,0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD",0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA,  0 }
};
static DX11ScreenVertex screen_quad[4] =
{
	{ -1,	1,	0.5f,	0.0f, 0.0f},
	{ 1,	1,	0.5f,	1.0f, 0.0f},
	{ 1,	-1,	0.5f,	1.0f, 1.0f},
	{ -1,	-1,	0.5f,	0.0f, 1.0f}
};
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(DX11FinalProcess, FinalProcess);
DX11FinalProcess::DX11FinalProcess()
{
	String shader_code = R"(
	Texture2D _MainTex : register(t0);
	SamplerState _MainTexSam : register(s0);
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
		return _MainTex.Sample(_MainTexSam, i.uv);
	}
	)";
	// Create the vertex shader
	VecString defines;
	ID3DBlob* vs_blob = nullptr;
	const String& shader_version = Application::GetGraphics()->GetShaderVersion();
	if (!DX10CompileShader("internal/finalprocess", shader_code, defines, Default_VS_Enter, String("vs_") + shader_version, &vs_blob))
	{
		return;
	}
	DX_ERROR(GetDX11Device()->GetDevice()->CreateVertexShader(vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), NULL, &_vertexShader));

	// Create the input layout
	DX_ERROR(GetDX11Device()->GetDevice()->CreateInputLayout(DX11ScreenVertex::Decl, 2, vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), &_vertexLayout));
	vs_blob->Release();

	// Create the pixel shader
	ID3DBlob* ps_blob = nullptr;
	if (!DX10CompileShader("internal/finalprocess", shader_code, defines, Default_PS_Enter, String("ps_") + shader_version, &ps_blob))
	{
		return;
	}
	DX_ERROR(GetDX11Device()->GetDevice()->CreatePixelShader(ps_blob->GetBufferPointer(), ps_blob->GetBufferSize(), NULL, &_pixelShader));
	ps_blob->Release();

	// Create the vertex buffer
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(DX11ScreenVertex) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData = {};
	InitData.pSysMem = screen_quad;
	DX_ERROR(GetDX11Device()->GetDevice()->CreateBuffer(&bd, &InitData, &_vertexBuffer));

	// Create the index buffer
	ushort indexs[] = { 0, 1, 2, 0, 2, 3 };
	Memory::Clear(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = 12;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	InitData = {};
	InitData.pSysMem = indexs;
	DX_ERROR(GetDX11Device()->GetDevice()->CreateBuffer(&bd, &InitData, &_indexBuffer));

	// Create sampler state
	D3D11_SAMPLER_DESC samp_desc = {};
	samp_desc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	samp_desc.MaxAnisotropy = 4;
	samp_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samp_desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samp_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samp_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samp_desc.MinLOD = 0;
	samp_desc.MaxLOD = D3D11_FLOAT32_MAX;
	DX_ERROR(GetDX11Device()->GetDevice()->CreateSamplerState(&samp_desc, &_samplerState));
}
DX11FinalProcess::~DX11FinalProcess()
{
	SAFE_RELEASE(_vertexLayout);
	SAFE_RELEASE(_vertexBuffer);
	SAFE_RELEASE(_indexBuffer);
	SAFE_RELEASE(_vertexShader);
	SAFE_RELEASE(_pixelShader);
	SAFE_RELEASE(_samplerState);
}
void DX11FinalProcess::Render(Camera* camera, RenderTexture* dest)
{
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

	{//执行拷贝
		// Set the input layout
		GetDX11Device()->GetContent()->IASetInputLayout(_vertexLayout);

		// Set vertex buffer
		UINT stride = sizeof(DX11ScreenVertex);
		UINT offset = 0;
		GetDX11Device()->GetContent()->IASetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);
		GetDX11Device()->GetContent()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Set index buffer
		GetDX11Device()->GetContent()->IASetIndexBuffer(_indexBuffer, DXGI_FORMAT_R16_UINT, 0);

		// Set shader
		GetDX11Device()->GetContent()->VSSetShader(_vertexShader, NULL, 0);
		GetDX11Device()->GetContent()->PSSetShader(_pixelShader, NULL, 0);

		//设置纹理
		ID3D11ShaderResourceView* texture_view = (ID3D11ShaderResourceView*)dest->GetTextureView();
		GetDX11Device()->GetContent()->PSSetShaderResources(0, 1, &texture_view);

		//采样状态
		GetDX11Device()->GetContent()->PSSetSamplers(0, 1, &_samplerState);

		// draw
		GetDX11Device()->GetContent()->DrawIndexed(6, 0, 0);
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