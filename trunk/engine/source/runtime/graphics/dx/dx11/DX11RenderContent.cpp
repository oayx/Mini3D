#include "DX11RenderContent.h"
#include "DX11Program.h"
#include "DX11Device.h"
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
IMPL_DERIVED_REFECTION_TYPE(DX11RenderContent, RenderContent);
DX11RenderContent::DX11RenderContent()
{
}
DX11RenderContent::~DX11RenderContent()
{
	for (auto obj : _rasterizerStates)
	{
		SAFE_RELEASE(obj.second);
	}
	_rasterizerStates.clear();
	for (auto obj : _blendStates)
	{
		SAFE_RELEASE(obj.second);
	}
	_blendStates.clear();
	for (auto obj : _depthStencilStates)
	{
		SAFE_RELEASE(obj.second);
	}
	_depthStencilStates.clear();
	for (auto obj : _samplerStates)
	{
		SAFE_RELEASE(obj.second);
	}
	_samplerStates.clear();
}
void DX11RenderContent::Initialize()
{
	base::Initialize();
}
void DX11RenderContent::BeginFrame(RenderFrameDesc& desc)
{
	DC_PROFILE_FUNCTION();
	ID3D11RenderTargetView* rt_view = (ID3D11RenderTargetView*)desc.target_buffer;
	ID3D11DepthStencilView* ds_view = (ID3D11DepthStencilView*)desc.depth_stencil_buffer;

	GetDX11Device()->GetContent()->OMSetRenderTargets(1, &rt_view, ds_view);
	this->SetViewport(desc.view_port);
	this->SetViewportScissor(iRect((int)desc.view_port.x, (int)desc.view_port.y, (int)desc.view_port.w, (int)desc.view_port.h));
	this->ClearBackbuffer(rt_view, ds_view, desc.clear_flag, desc.clear_color);
}
void DX11RenderContent::RenderOnePrimitive(Camera* camera, Pass* pass, Primitive* primitive, RenderMode mode)
{
	DC_PROFILE_FUNCTION();
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
	this->UpdataRenderState(camera, pass);

	//获得渲染类型和渲染三角形个数
	D3D_PRIMITIVE_TOPOLOGY prim_pype = DX11GetPrimitiveType(primitive->GetPrimitiveType());
	GetDX11Device()->GetContent()->IASetPrimitiveTopology(prim_pype);

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
			if (vertex_buffer->GetInstanceCount() > 0)
			{
				GetDX11Device()->GetContent()->DrawIndexedInstanced(prim.ElemCount, vertex_buffer->GetInstanceCount(), prim.StartIndex, prim.StartVertex, 0);
				SceneManager::AddDrawTriangles(prim_count * vertex_buffer->GetInstanceCount());
			}
			else
			{
				GetDX11Device()->GetContent()->DrawIndexed(prim.ElemCount, prim.StartIndex, prim.StartVertex);
				SceneManager::AddDrawTriangles(prim_count);
			}
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
			if (vertex_buffer->GetInstanceCount() > 0)
			{
				GetDX11Device()->GetContent()->DrawInstanced(prim.ElemCount, vertex_buffer->GetInstanceCount(), prim.StartVertex, 0);
				SceneManager::AddDrawTriangles(prim_count * vertex_buffer->GetInstanceCount());
			}
			else
			{
				GetDX11Device()->GetContent()->Draw(prim.ElemCount, prim.StartVertex);
				SceneManager::AddDrawTriangles(prim_count);
			}
			SceneManager::AddDrawCall(1);
		}
	}

	//统计
	SceneManager::AddRenderBatch(1);
	SceneManager::AddSetPassCall(1);
}
void DX11RenderContent::SetViewport(const ViewPortDesc& view_port)
{
	DC_PROFILE_FUNCTION();
	D3D11_VIEWPORT port;
	port.TopLeftX = view_port.x; port.TopLeftY = view_port.y;
	port.Width = view_port.w; port.Height = view_port.h;
	port.MinDepth = view_port.z_near; port.MaxDepth = view_port.z_far;
	GetDX11Device()->GetContent()->RSSetViewports(1, &port);
}
void DX11RenderContent::SetViewportScissor(const iRect& clip)
{
	DC_PROFILE_FUNCTION();
	CD3D11_RECT rect((LONG)clip.x, (LONG)(clip.y), (LONG)(clip.x + clip.width), (LONG)(clip.y + clip.height));
	GetDX11Device()->GetContent()->RSSetScissorRects(1, &rect);
}
void DX11RenderContent::ClearBackbuffer(void* target_buffer, void* depth_stencil_buffer, ClearFlag flag, const Color& color)
{
	DC_PROFILE_FUNCTION();
	DWORD clear_flags = 0;
	bool is_clear_color = false;
	switch (flag)
	{
	case ClearFlag::Skybox:
	case ClearFlag::SolidColor:
		clear_flags = D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL;
		is_clear_color = true;
		break;
	case ClearFlag::DepthOnly:
		clear_flags = D3D11_CLEAR_DEPTH;
		break;
	case ClearFlag::DonotClear:
		return;
	}
	
	ID3D11RenderTargetView* rt_view = (ID3D11RenderTargetView*)target_buffer;
	ID3D11DepthStencilView* ds_view = (ID3D11DepthStencilView*)depth_stencil_buffer;
	if (target_buffer != nullptr && is_clear_color)GetDX11Device()->GetContent()->ClearRenderTargetView(rt_view, color.ptr());
	if (depth_stencil_buffer != nullptr)GetDX11Device()->GetContent()->ClearDepthStencilView(ds_view, clear_flags, 1.0f, 0);
}
void DX11RenderContent::Resize(const WindowResizeDesc& desc)
{
	base::Resize(desc);
}
ID3D11SamplerState* DX11RenderContent::GetSamplerState(SamplerStateKey state)
{
	DC_PROFILE_FUNCTION();
	auto it = _samplerStates.find(state.u);
	if (it == _samplerStates.end())
	{
		D3D11_SAMPLER_DESC samp_desc = {};
		samp_desc.Filter = DX11GetTextureFilter(state.FilterType, state.Anisotropy);
		samp_desc.AddressU = DX11GetTextureAddress(state.AddressMode);
		samp_desc.AddressV = DX11GetTextureAddress(state.AddressMode);
		samp_desc.AddressW = DX11GetTextureAddress(state.AddressMode);
		Memory::Copy(samp_desc.BorderColor, Color(state.BorderColor).p, sizeof(Color));
		samp_desc.MaxAnisotropy = state.Anisotropy;
		samp_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		samp_desc.MinLOD = 0;
		samp_desc.MaxLOD = D3D11_FLOAT32_MAX;

		ID3D11SamplerState* sampler_state = nullptr;
		HR(GetDX11Device()->GetDevice()->CreateSamplerState(&samp_desc, &sampler_state));
		it = _samplerStates.insert({ state.u, sampler_state }).first;
	}
	return it->second;
}
void DX11RenderContent::UpdataRenderState(Camera* camera, Pass* pass)
{
	DC_PROFILE_FUNCTION();
	//光栅化
	SetRasterizerState(camera, pass);

	//深度测试+模板测试
	SetDepthStencilTest(pass);

	//纹理
	SetBlendState(pass);
}
void DX11RenderContent::SetRasterizerState(Camera* camera, Pass* pass)
{
	DC_PROFILE_FUNCTION();

	FillMode fill_mode = FillMode::Solid;
	if (camera)fill_mode = camera->GetFillMode();

	RasterizerStateKey state = {};
	state.CullMode = pass->Cullmode;
	state.FillMode = fill_mode;
	state.ScissorEnable = pass->GetMaterial()->IsScissorEnable();
	auto it = _rasterizerStates.find(state.u);
	if (it == _rasterizerStates.end())
	{
		D3D11_RASTERIZER_DESC desc = {};
		desc.FrontCounterClockwise = FALSE;
		desc.FillMode = DX11GetFillMode(state.FillMode);
		desc.CullMode = DX11GetCullMode(state.CullMode);
		desc.DepthClipEnable = TRUE;
		desc.FrontCounterClockwise = FALSE;
		desc.ScissorEnable = state.ScissorEnable;
		ID3D11RasterizerState* raster = nullptr;
		HR(GetDX11Device()->GetDevice()->CreateRasterizerState(&desc, &raster));
		it = _rasterizerStates.insert({ state.u, raster }).first;
	}
	GetDX11Device()->GetContent()->RSSetState(it->second);
}
void DX11RenderContent::SetBlendState(Pass* pass)
{
	DC_PROFILE_FUNCTION();
	BlendStateKey state = {};
	state.alpha_to_coverage = pass->AlphaToCoverage;
	state.enable_alpha = pass->BlendEnable;
	state.src_alpha_blend = pass->SrcAlphaSource;
	state.dst_alpha_blend = pass->DstAlphaSource;
	state.src_color_blend = pass->SrcBlend;
	state.dst_color_blend = pass->DstBlend;
	if (pass->ColorMask.x)state.color_mask_r = true;
	if (pass->ColorMask.y)state.color_mask_g = true;
	if (pass->ColorMask.z)state.color_mask_g = true;
	if (pass->ColorMask.w)state.color_mask_a = true;

	auto it = _blendStates.find(state.u);
	if (it == _blendStates.end())
	{
		D3D11_BLEND_DESC desc = {};
		desc.AlphaToCoverageEnable = pass->AlphaToCoverage;//在开启Multi-sample多重采样时，在透明和不透明之间达到抗锯齿的效果；
		desc.IndependentBlendEnable = FALSE;//是否针对多个RenderTarget使用不同的混合状态

		//alpha blend
		desc.RenderTarget[0].BlendEnable = pass->BlendEnable;
		desc.RenderTarget[0].SrcBlend = DX11GetAlphaBlend(pass->SrcBlend);	//源、目标颜色混合因子
		desc.RenderTarget[0].DestBlend = DX11GetAlphaBlend(pass->DstBlend);
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;		//源、目标颜色的混合操作
		desc.RenderTarget[0].SrcBlendAlpha = DX11GetAlphaBlend(pass->SrcAlphaSource);//源、目标alpha值的混合因子
		desc.RenderTarget[0].DestBlendAlpha = DX11GetAlphaBlend(pass->DstAlphaSource);
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;	//源、目标alpha值的混合操作

		//color mask
		DWORD color_mask = 0;
		if (pass->ColorMask.x)color_mask |= D3D11_COLOR_WRITE_ENABLE_RED;
		if (pass->ColorMask.y)color_mask |= D3D11_COLOR_WRITE_ENABLE_GREEN;
		if (pass->ColorMask.z)color_mask |= D3D11_COLOR_WRITE_ENABLE_BLUE;
		if (pass->ColorMask.w)color_mask |= D3D11_COLOR_WRITE_ENABLE_ALPHA;
		desc.RenderTarget[0].RenderTargetWriteMask = (UINT8)color_mask;

		ID3D11BlendState* blend = nullptr;
		HR(GetDX11Device()->GetDevice()->CreateBlendState(&desc, &blend));
		it = _blendStates.insert({ state.u, blend }).first;
	}
	//第三个参数为采样点掩码。D3D11中的多重采样可以支持32个采样点，该参数用来决定”使用 / 丢弃"哪些采样点。
	//该参数类型为UINT，32位，其中从最低位到最高位分别代表一个采样点。比如，如果第5位指定为0，则第5个采样点将被丢弃。
	//当然，只有当开启至少5重采样时该设定才有效。如果当前设置为单个采样点，则只有最低位才对我们有用。默认情况下，该参数值为0xFFFFFFFF，即对所有采样点有效
	GetDX11Device()->GetContent()->OMSetBlendState(it->second, nullptr, 0xffffffff);
}
void DX11RenderContent::SetDepthStencilTest(Pass* pass)
{
	DC_PROFILE_FUNCTION();
	DepthStencilStateKey state = {};
	state.DepthEnable = pass->DepthEnable;
	state.DepthWriteEnable = pass->DepthWriteEnable;
	state.DepthCmpFun = pass->DepthCmpFun;
	state.StencilEnable = pass->StencilEnable;
	state.StencilReadMask = pass->StencilReadMask;
	state.StencilWriteMask = pass->StencilWriteMask;
	state.StencilFailOp = pass->StencilFailOp;
	state.StencilDepthFailOp = pass->StencilDepthFailOp;
	state.StencilPassOp = pass->StencilPassOp;
	state.StencilFunc = pass->StencilFunc;
	state.StencilRef = pass->StencilRef;

	auto it = _depthStencilStates.find(state.u);
	if (it == _depthStencilStates.end())
	{
		// 设置深度模版状态描述.
		D3D11_DEPTH_STENCIL_DESC desc = {};
		desc.DepthEnable = pass->DepthEnable;
		desc.DepthWriteMask = pass->DepthWriteEnable ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
		desc.DepthFunc = DX11GetStencilCmp(pass->DepthCmpFun);

		desc.StencilEnable = pass->StencilEnable;
		if (pass->StencilEnable)
		{
			desc.StencilReadMask = pass->StencilReadMask; //和OpenGL中函数glStencilFunc的第三个参数意思一样
			desc.StencilWriteMask = pass->StencilWriteMask; //和glStencilMask意思一样

			// 对于front face 像素使用的模版操作操作，三种状况下的stencil操作
			desc.FrontFace.StencilFailOp = DX11GetStencilOp(pass->StencilFailOp);
			desc.FrontFace.StencilDepthFailOp = DX11GetStencilOp(pass->StencilDepthFailOp);
			desc.FrontFace.StencilPassOp = DX11GetStencilOp(pass->StencilPassOp);
			desc.FrontFace.StencilFunc = DX11GetStencilCmp(pass->StencilFunc);

			// 对于back face像素使用的模版操作模式.
			desc.BackFace = desc.FrontFace;
		}

		ID3D11DepthStencilState* depth = nullptr;
		HR(GetDX11Device()->GetDevice()->CreateDepthStencilState(&desc, &depth));
		it = _depthStencilStates.insert({ state.u, depth }).first;
	}

	GetDX11Device()->GetContent()->OMSetDepthStencilState(it->second, pass->StencilRef);
}
void DX11RenderContent::SetSubPrimitiveState(const SubPrimitive* sub_prim, Pass* pass)
{
	if (!sub_prim->Tex)return;
	CGProgram* shader = pass->GetProgram();
	shader->SetPassVariable(sub_prim->TexIndex, sub_prim->TexName, sub_prim->Tex);
	
	bool old_alpha_state = pass->BlendEnable;
	pass->BlendEnable = sub_prim->AlphaEnable;
	SetBlendState(pass);
	pass->BlendEnable = old_alpha_state;
}
DC_END_NAMESPACE