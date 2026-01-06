
#include "DX12RenderContent.h"
#include "DX12Program.h"
#include "DX12Device.h"
#include "runtime/graphics/Material.h"
#include "runtime/graphics/Pass.h"
#include "runtime/graphics/TextureUnit.h"
#include "runtime/graphics/null/Texture.h"
#include "runtime/graphics/null/HardwareBuffer.h"
#include "runtime/renderable/Renderer.h"
#include "runtime/scene/Camera.h"
#include "platform/RenderWindow.h"
#include "runtime/Application.h"
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(DX12RenderContent, RenderContent);
DX12RenderContent::DX12RenderContent()
{
}
DX12RenderContent::~DX12RenderContent()
{
	for (auto state : _pipelineStates)
	{
		SAFE_RELEASE(state.second);
	}
	_pipelineStates.clear();
}
void DX12RenderContent::Initialize()
{
	base::Initialize();
}
void DX12RenderContent::BeginFrame(RenderFrameDesc& desc)
{
	D3D12_CPU_DESCRIPTOR_HANDLE* rt_view = (D3D12_CPU_DESCRIPTOR_HANDLE*)desc.targetBuffer;
	D3D12_CPU_DESCRIPTOR_HANDLE* ds_view = (D3D12_CPU_DESCRIPTOR_HANDLE*)desc.depthStencilBuffer;

	GetDX12Device()->GetCommandList()->OMSetRenderTargets(rt_view ? 1 : 0, rt_view, true, ds_view);

	this->SetViewport(desc.viewPort);
	this->SetViewportScissor(iRect((int)desc.viewPort.x, (int)desc.viewPort.y, (int)desc.viewPort.w, (int)desc.viewPort.h));
	this->ClearBackbuffer(rt_view, ds_view, desc.clearFlag, desc.clearColor);
}
void DX12RenderContent::RenderOnePrimitive(Camera* camera, Pass* pass, Primitive* primitive, RenderMode mode)
{
	//在后台缓冲区绘制图形
	HardwareVertexBuffer* vertex_buffer = primitive->GetVertexData();
	if (vertex_buffer == nullptr || vertex_buffer->GetVertexCount() == 0)//如果模型没有顶点数据，则buffer会为null
		return;

	//Gpu
	CGProgram* shader = pass->GetProgram();
	shader->PreRender();
	shader->Render(camera, pass, primitive->GetWorldMatrix());
	shader->PostRender();

	//顶点声明和数据流
	vertex_buffer->Render(shader);

	//渲染状态更新
	this->UpdataRenderState(camera, pass, vertex_buffer);

	//获得渲染类型和渲染三角形个数
	D3D_PRIMITIVE_TOPOLOGY prim_pype = DX12GetPrimitiveType(primitive->GetPrimitiveType());
	GetDX12Device()->GetCommandList()->IASetPrimitiveTopology(prim_pype);

	//画图元
	HardwareIndexBuffer *index_buffer = primitive->GetIndexData();
	if (index_buffer)
	{
		index_buffer->Render();
		for (int i = 0; i < primitive->GetSubPrimitiveCount(); ++i)
		{
			const SubPrimitive& prim = primitive->GetSubPrimitive(i);
			int prim_count = prim.GetTringles(primitive->GetPrimitiveType());
			this->SetViewportScissor(prim.ScissorRect);
			this->SetSubPrimitiveState(&prim, pass);

			int instanceCount = vertex_buffer->GetInstanceCount();
			instanceCount = instanceCount <= 0 ? 1 : instanceCount;
			GetDX12Device()->GetCommandList()->DrawIndexedInstanced(prim.ElemCount, instanceCount, prim.StartIndex, prim.StartVertex, 0);

			SceneManager::AddDrawTriangles(prim_count * instanceCount);
			SceneManager::AddDrawCall(1);
		}
	}
	else
	{
		for (int i = 0; i < primitive->GetSubPrimitiveCount(); ++i)
		{
			const SubPrimitive& prim = primitive->GetSubPrimitive(i);
			int prim_count = prim.GetTringles(primitive->GetPrimitiveType());
			this->SetViewportScissor(prim.ScissorRect);
			this->SetSubPrimitiveState(&prim, pass);

			int instanceCount = vertex_buffer->GetInstanceCount();
			instanceCount = instanceCount <= 0 ? 1 : instanceCount;
			GetDX12Device()->GetCommandList()->DrawInstanced(prim.ElemCount, instanceCount, prim.StartVertex, 0);

			SceneManager::AddDrawTriangles(prim_count * instanceCount);
			SceneManager::AddDrawCall(1);
		}
	}

	//统计
	SceneManager::AddRenderBatch(1);
	SceneManager::AddSetPassCall(1);
}
void DX12RenderContent::SetViewport(const ViewPortDesc& viewPort)
{
	D3D12_VIEWPORT port;
	port.TopLeftX = viewPort.x; port.TopLeftY = viewPort.y;
	port.Width = viewPort.w; port.Height = viewPort.h;
	port.MinDepth = viewPort.z_near; port.MaxDepth = viewPort.z_far;
	GetDX12Device()->GetCommandList()->RSSetViewports(1, &port);
}
void DX12RenderContent::SetViewportScissor(const iRect& clip)
{
	D3D12_RECT rect = { (LONG)clip.x, (LONG)(clip.y), (LONG)(clip.x + clip.width), (LONG)(clip.y + clip.height) };
	GetDX12Device()->GetCommandList()->RSSetScissorRects(1, &rect);
}
void DX12RenderContent::ClearBackbuffer(void* targetBuffer, void* depthStencilBuffer, ClearFlag flag, const Color& color)
{
	D3D12_CLEAR_FLAGS clear_flags = D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL;
	bool is_clear_color = false;
	switch (flag)
	{
	case ClearFlag::Skybox:
	case ClearFlag::SolidColor:
		clear_flags = D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL;
		is_clear_color = true;
		break;
	case ClearFlag::DepthOnly:
		clear_flags = D3D12_CLEAR_FLAG_DEPTH;
		break;
	case ClearFlag::DonotClear:
		return;
	}

	if (is_clear_color && targetBuffer != nullptr)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE target_view = *((D3D12_CPU_DESCRIPTOR_HANDLE*)targetBuffer);
		GetDX12Device()->GetCommandList()->ClearRenderTargetView(target_view, color.ptr(), 0, NULL);
	}
	if (depthStencilBuffer != nullptr)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE depth_stencil_view = *((D3D12_CPU_DESCRIPTOR_HANDLE*)depthStencilBuffer);
		GetDX12Device()->GetCommandList()->ClearDepthStencilView(depth_stencil_view, clear_flags, 1.0f, 0, 0, nullptr);
	}
}
void DX12RenderContent::UpdataRenderState(Camera* camera, Pass* pass, HardwareVertexBuffer* vertex_buffer)
{
	ID3D12PipelineState* pso = nullptr;
	uint64 key = (uint64)pass;
	auto it = _pipelineStates.find(key);
	if (it == _pipelineStates.end())
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
		desc.NodeMask = 1;
		desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		desc.pRootSignature = GetDX12Device()->GetRootSignature();
		desc.SampleMask = UINT_MAX;
		desc.NumRenderTargets = 1;
		desc.RTVFormats[0] = DX10GetTextureViewFormat(DX12RenderTargetFormat, false);
		desc.DSVFormat = DX10GetTextureViewFormat(DX12DepthStencilFormat, false);
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

		this->SetRasterizerState(desc, camera, pass);
		this->SetBlendState(desc, pass);
		this->SetDepthStencilTest(desc, pass);
		this->SetShader(desc, pass);

		Vector<D3D12_INPUT_ELEMENT_DESC> element_desc;
		this->SetVertexDeclaration(element_desc, vertex_buffer, pass->GetProgram()->GetInputVertexSemantic());
		desc.InputLayout = { element_desc.Data(), (UINT)element_desc.Size() };

		DX_ERROR(GetDX12Device()->GetDevice()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pso)));
		_pipelineStates.insert({ key, pso });
	}
	else
	{
		pso = it->second;
	}
	GetDX12Device()->GetCommandList()->SetPipelineState(pso);
	if (pass->StencilEnable)
	{//模板测试值需要独立设置
		GetDX12Device()->GetCommandList()->OMSetStencilRef(pass->StencilRef);
	}
}
void DX12RenderContent::SetRasterizerState(D3D12_GRAPHICS_PIPELINE_STATE_DESC& pso_desc, Camera* camera, Pass* pass)
{
	FillMode fill_mode = FillMode::Solid;
	if (camera)fill_mode = camera->GetFillMode();

	D3D12_RASTERIZER_DESC& desc = pso_desc.RasterizerState;
	desc.FillMode = DX12GetFillMode(fill_mode);//D3D12_FILL_MODE_WIREFRAME
	desc.CullMode = DX12GetCullMode(pass->Cullmode);
	desc.FrontCounterClockwise = FALSE;
	desc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	desc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	desc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	desc.DepthClipEnable = TRUE;
	desc.MultisampleEnable = FALSE;
	desc.AntialiasedLineEnable = FALSE;
	desc.ForcedSampleCount = 0;
	desc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
}
void DX12RenderContent::SetBlendState(D3D12_GRAPHICS_PIPELINE_STATE_DESC& pso_desc, Pass* pass)
{
	D3D12_BLEND_DESC& desc = pso_desc.BlendState;
	desc.AlphaToCoverageEnable = pass->AlphaToCoverage;
	desc.IndependentBlendEnable = FALSE;
	desc.RenderTarget[0].BlendEnable = pass->BlendEnable;
	desc.RenderTarget[0].SrcBlend = DX12GetAlphaBlend(pass->SrcBlend);	//源、目标颜色混合因子
	desc.RenderTarget[0].DestBlend = DX12GetAlphaBlend(pass->DstBlend);
	desc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;		//源、目标颜色的混合操作
	desc.RenderTarget[0].SrcBlendAlpha = DX12GetAlphaBlend(pass->SrcAlphaSource);//源、目标alpha值的混合因子
	desc.RenderTarget[0].DestBlendAlpha = DX12GetAlphaBlend(pass->DstAlphaSource);
	desc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;	//源、目标alpha值的混合操作

	//color mask
	DWORD color_mask = 0;
	if (pass->ColorMask.x)color_mask |= D3D11_COLOR_WRITE_ENABLE_RED;
	if (pass->ColorMask.y)color_mask |= D3D11_COLOR_WRITE_ENABLE_GREEN;
	if (pass->ColorMask.z)color_mask |= D3D11_COLOR_WRITE_ENABLE_BLUE;
	if (pass->ColorMask.w)color_mask |= D3D11_COLOR_WRITE_ENABLE_ALPHA;
	desc.RenderTarget[0].RenderTargetWriteMask = (UINT8)color_mask;
}
void DX12RenderContent::SetDepthStencilTest(D3D12_GRAPHICS_PIPELINE_STATE_DESC& pso_desc, Pass* pass)
{
	D3D12_DEPTH_STENCIL_DESC& desc = pso_desc.DepthStencilState;
	desc.DepthEnable = pass->DepthEnable;
	desc.DepthWriteMask = pass->DepthWriteEnable ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
	desc.DepthFunc = DX12GetStencilCmp(pass->DepthCmpFun);
	desc.StencilEnable = pass->StencilEnable;
	if (pass->StencilEnable)
	{
		desc.StencilReadMask = pass->StencilReadMask; //和OpenGL中函数glStencilFunc的第三个参数意思一样
		desc.StencilWriteMask = pass->StencilWriteMask; //和glStencilMask意思一样

		// 对于front face 像素使用的模版操作操作，三种状况下的stencil操作
		desc.FrontFace.StencilFailOp = DX12GetStencilOp(pass->StencilFailOp);
		desc.FrontFace.StencilDepthFailOp = DX12GetStencilOp(pass->StencilDepthFailOp);
		desc.FrontFace.StencilPassOp = DX12GetStencilOp(pass->StencilPassOp);
		desc.FrontFace.StencilFunc = DX12GetStencilCmp(pass->StencilFunc);

		// 对于back face像素使用的模版操作模式.
		desc.BackFace = desc.FrontFace;
	}
}
void DX12RenderContent::SetShader(D3D12_GRAPHICS_PIPELINE_STATE_DESC& pso_desc, Pass* pass)
{
	DX12Program* shader = dynamic_cast<DX12Program*>(pass->GetProgram());
	for (int i = int(ShaderType::Vertex); i < int(ShaderType::Max); ++i)
	{
		ShaderType type = ShaderType(i);
		ID3DBlob* shader_code = shader->GetCodeBuffer(type);
		switch (type)
		{
		case ShaderType::Vertex:
			if (shader_code == nullptr)
				pso_desc.VS = { nullptr, 0 };
			else
				pso_desc.VS = { reinterpret_cast<BYTE*>(shader_code->GetBufferPointer()),shader_code->GetBufferSize() };
			break;
		case ShaderType::Hull:
			if (shader_code == nullptr)
				pso_desc.HS = { nullptr, 0 };
			else
				pso_desc.HS = { reinterpret_cast<BYTE*>(shader_code->GetBufferPointer()),shader_code->GetBufferSize() };
			break;
		case ShaderType::Domain:
			if (shader_code == nullptr)
				pso_desc.DS = { nullptr, 0 };
			else
				pso_desc.DS = { reinterpret_cast<BYTE*>(shader_code->GetBufferPointer()),shader_code->GetBufferSize() };
			break;
		case ShaderType::Geometry:
			if (shader_code == nullptr)
				pso_desc.GS = { nullptr, 0 };
			else
				pso_desc.GS = { reinterpret_cast<BYTE*>(shader_code->GetBufferPointer()),shader_code->GetBufferSize() };
			break;
		case ShaderType::Pixel:
			if (shader_code == nullptr)
				pso_desc.PS = { nullptr, 0 };
			else
				pso_desc.PS = { reinterpret_cast<BYTE*>(shader_code->GetBufferPointer()),shader_code->GetBufferSize() };
			break;
		}
	}
}
void DX12RenderContent::SetSubPrimitiveState(const SubPrimitive* sub_prim, Pass* pass)
{
	if (!sub_prim->Tex)return;
	CGProgram* shader = pass->GetProgram();
	shader->SetPassVariable(sub_prim->TexIndex, sub_prim->TexName, sub_prim->Tex);

	//TODO
}
void DX12RenderContent::SetVertexDeclaration(Vector<D3D12_INPUT_ELEMENT_DESC>& element_desc, HardwareVertexBuffer* vertex_buffer, VertexSemantic input_semantic)
{
	for (byte stream = 0; stream < MAX_STREAM_COUNT; ++stream)
	{
		for (byte i = 0; i < vertex_buffer->GetElementCount(stream); ++i)
		{
			const VertexElement* element = vertex_buffer->GetElement(stream, i);
			if (input_semantic & element->GetSemantic())
			{
				bool is_instance = IsInstanceVertexSemantic(element->GetSemantic());
				if(is_instance)
					if (stream > INSTANCE_STREAM_INDEX)
						continue;
				D3D12_INPUT_ELEMENT_DESC desc = {};
				desc.SemanticName = DXGetInputSemantic(element->GetSemantic());
				desc.SemanticIndex = (UINT)element->GetIndex() + stream;
				desc.Format = DX10GetVertexType(element->GetType());
				desc.InputSlot = (UINT)element->GetStream();
				desc.AlignedByteOffset = (UINT)element->GetOffset();
				desc.InputSlotClass = is_instance ? D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA : D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
				desc.InstanceDataStepRate = (UINT)element->GetStream();
				element_desc.Add(desc);
			}
		}
	}
}
DC_END_NAMESPACE

