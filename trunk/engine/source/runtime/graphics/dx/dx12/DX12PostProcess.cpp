
#include "DX12PostProcess.h"
#include "DX12Program.h"
#include "DX12Device.h"
#include "DX12RenderContent.h"
#include "runtime/graphics/Material.h"
#include "runtime/graphics/null/HardwareBuffer.h"
#include "runtime/renderable/Renderer.h"
#include "runtime/scene/Camera.h"
#include "runtime/Application.h"
#include "editor/main/EMain_GameView.h"
 
DC_BEGIN_NAMESPACE

struct DX12ScreenVertex
{
	float x, y, z;
	float tu, tv;
	const static D3D12_INPUT_ELEMENT_DESC Decl[2];
};
const D3D12_INPUT_ELEMENT_DESC DX12ScreenVertex::Decl[2] =
{
	{ "POSITION",0, DXGI_FORMAT_R32G32B32_FLOAT,0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD",0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,  0 }
};
static DX12ScreenVertex screen_quad[4] =
{
	{ -1,	1,	0.5f,	0.0f, 0.0f},
	{ 1,	1,	0.5f,	1.0f, 0.0f},
	{ 1,	-1,	0.5f,	1.0f, 1.0f},
	{ -1,	-1,	0.5f,	0.0f, 1.0f}
};
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(DX12FinalProcess, FinalProcess);
DX12FinalProcess::DX12FinalProcess()
{
	{// Create the vertex buffer
		D3D12_HEAP_PROPERTIES props = {};
		props.Type = D3D12_HEAP_TYPE_UPLOAD;
		props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

		D3D12_RESOURCE_DESC sd = {};
		sd.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		sd.Width = sizeof(DX12ScreenVertex) * 4;
		sd.Height = 1;
		sd.DepthOrArraySize = 1;
		sd.MipLevels = 1;
		sd.Format = DXGI_FORMAT_UNKNOWN;
		sd.SampleDesc.Count = 1;
		sd.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		sd.Flags = D3D12_RESOURCE_FLAG_NONE;
		DX_ERROR(GetDX12Device()->GetDevice()->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &sd, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, IID_PPV_ARGS(&_vertexBuffer)));

		void* buffer_ptr = nullptr;
		DX_ERROR(_vertexBuffer->Map(0, nullptr, &buffer_ptr));
		Memory::Copy(buffer_ptr, screen_quad, sd.Width);
		_vertexBuffer->Unmap(0, nullptr);
	}
	{// Create the index buffer
		D3D12_HEAP_PROPERTIES props;
		memset(&props, 0, sizeof(D3D12_HEAP_PROPERTIES));
		props.Type = D3D12_HEAP_TYPE_UPLOAD;
		props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

		D3D12_RESOURCE_DESC sd;
		memset(&sd, 0, sizeof(D3D12_RESOURCE_DESC));
		sd.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		sd.Width = 12;
		sd.Height = 1;
		sd.DepthOrArraySize = 1;
		sd.MipLevels = 1;
		sd.Format = DXGI_FORMAT_UNKNOWN;
		sd.SampleDesc.Count = 1;
		sd.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		sd.Flags = D3D12_RESOURCE_FLAG_NONE;
		DX_ERROR(GetDX12Device()->GetDevice()->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &sd, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, IID_PPV_ARGS(&_indexBuffer)));

		void* buffer_ptr = nullptr;
		ushort indexs[] = { 0, 1, 2, 0, 2, 3 };
		DX_ERROR(_indexBuffer->Map(0, nullptr, &buffer_ptr));
		Memory::Copy(buffer_ptr, indexs, sd.Width);
		_indexBuffer->Unmap(0, nullptr);
	}
	{//pso
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
		ID3DBlob* vs_blob = NULL;
		const String& shader_version = Application::GetGraphics()->GetShaderVersion();
		if (!DX10CompileShader("internal/finalprocess", shader_code, defines, Default_VS_Enter, String("vs_") + shader_version, &vs_blob))
		{
			return;
		}
		// Create the pixel shader
		ID3DBlob* ps_blob = NULL;
		if (!DX10CompileShader("internal/finalprocess", shader_code, defines, Default_PS_Enter, String("ps_") + shader_version, &ps_blob))
		{
			return;
		}

		D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc = {};
		pso_desc.InputLayout = { DX12ScreenVertex::Decl, (UINT)ARRAY_SIZE(DX12ScreenVertex::Decl) };
		pso_desc.pRootSignature = GetDX12Device()->GetRootSignature();
		pso_desc.VS = { reinterpret_cast<BYTE*>(vs_blob->GetBufferPointer()),vs_blob->GetBufferSize() };
		pso_desc.PS = { reinterpret_cast<BYTE*>(ps_blob->GetBufferPointer()),ps_blob->GetBufferSize() };
		pso_desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		pso_desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		pso_desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		pso_desc.SampleMask = UINT_MAX;
		pso_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pso_desc.NumRenderTargets = 1;
		pso_desc.RTVFormats[0] = DX10GetTextureViewFormat(DX12RenderTargetFormat, false);
		pso_desc.DSVFormat = DX10GetTextureViewFormat(DX12DepthStencilFormat, false);
		pso_desc.SampleDesc.Count = 1;
		pso_desc.SampleDesc.Quality = 0;
		DX_ERROR(GetDX12Device()->GetDevice()->CreateGraphicsPipelineState(&pso_desc, IID_PPV_ARGS(&_pipelineState)));
		vs_blob->Release();
		ps_blob->Release();
	}
}
DX12FinalProcess::~DX12FinalProcess()
{
	SAFE_RELEASE(_vertexBuffer);
	SAFE_RELEASE(_indexBuffer);
	SAFE_RELEASE(_pipelineState);
}
void DX12FinalProcess::Render(Camera* camera, RenderTexture* dest)
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

	{
		// Set vertex buffer
		D3D12_VERTEX_BUFFER_VIEW vbv = {};
		vbv.BufferLocation = _vertexBuffer->GetGPUVirtualAddress();
		vbv.SizeInBytes = sizeof(DX12ScreenVertex) * 4;
		vbv.StrideInBytes = sizeof(DX12ScreenVertex);
		GetDX12Device()->GetCommandList()->IASetVertexBuffers(0, 1, &vbv);

		// Set index buffer
		D3D12_INDEX_BUFFER_VIEW ibv;
		memset(&ibv, 0, sizeof(D3D12_INDEX_BUFFER_VIEW));
		ibv.BufferLocation = _indexBuffer->GetGPUVirtualAddress();
		ibv.SizeInBytes = 12;
		ibv.Format = DXGI_FORMAT_R16_UINT;
		GetDX12Device()->GetCommandList()->IASetIndexBuffer(&ibv);

		//pso
		GetDX12Device()->GetCommandList()->SetPipelineState(_pipelineState);

		//draw
		GetDX12Device()->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
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

