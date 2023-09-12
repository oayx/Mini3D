#include "DX9RenderContent.h"
#include "DX9Caps.h"
#include "DX9Device.h"
#include "DX9Program.h"
#include "DX9SwapChain.h"
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
IMPL_DERIVED_REFECTION_TYPE(DX9RenderContent, RenderContent);
DX9RenderContent::DX9RenderContent()
{
	GetDX9Device()->AddEventListener(LostDeviceEvt, this, MakeEventHandler(DX9RenderContent::HandleLostDevice));
	GetDX9Device()->AddEventListener(ResetDeviceEvt, this, MakeEventHandler(DX9RenderContent::HandleResetDevice));
}
DX9RenderContent::~DX9RenderContent()
{
	GetDX9Device()->RemoveEventListener(LostDeviceEvt, this, MakeEventHandler(DX9RenderContent::HandleLostDevice));
	GetDX9Device()->RemoveEventListener(ResetDeviceEvt, this, MakeEventHandler(DX9RenderContent::HandleResetDevice));

	for (auto decl : _vertexDeclarations)
	{
		SAFE_RELEASE(decl.second);
	}
	_vertexDeclarations.clear();
}
void DX9RenderContent::Initialize()
{
	base::Initialize();
}
void DX9RenderContent::SetFillMode(FillMode mode)
{
	GetDX9Device()->SetD3DRenderState(D3DRS_FILLMODE, DX9GetFillMode(mode));
}
void DX9RenderContent::SetShadeMode(ShadeMode mode)
{
	GetDX9Device()->SetD3DRenderState(D3DRS_SHADEMODE, DX9GetShadeMode(mode));
}
void DX9RenderContent::PreRender(RenderWindow* window)
{
	base::PreRender(window);

	GetDX9Device()->CheckDeviceLost();

	DX9HR(GetDX9Device()->GetDevice()->BeginScene());
}
void DX9RenderContent::PostRender()
{
	DX9HR(GetDX9Device()->GetDevice()->EndScene());
	base::PostRender();
}
void DX9RenderContent::BeginFrame(RenderFrameDesc& desc)
{
	base::BeginFrame(desc);

	if (GetDX9Device()->IsDeviceInLostState())
		return;

	IDirect3DSurface9* rt_view = (IDirect3DSurface9*)desc.target_buffer;
	IDirect3DSurface9* ds_view = (IDirect3DSurface9*)desc.depth_stencil_buffer;
	DX9HR(GetDX9Device()->GetDevice()->SetRenderTarget(0, rt_view));
	DX9HR(GetDX9Device()->GetDevice()->SetDepthStencilSurface(ds_view));

	this->SetViewport(desc.view_port);
	this->SetViewportScissor(iRect((int)desc.view_port.x, (int)desc.view_port.y, (int)desc.view_port.w, (int)desc.view_port.h));
	this->ClearBackbuffer(desc.target_buffer, desc.depth_stencil_buffer, desc.clear_flag, desc.clear_color);
}
void DX9RenderContent::EndFrame()
{
	if (GetDX9Device()->IsDeviceInLostState())
		return;

	base::EndFrame();
}
void DX9RenderContent::RenderOnePrimitive(Camera* camera, Pass* pass, Primitive* primitive, RenderMode mode)
{
	if (GetDX9Device()->IsDeviceInLostState())
		return;

	//在后台缓冲区绘制图形
	HardwareVertexBuffer* vertex_buffer = primitive->GetVertexData();
	if (vertex_buffer == nullptr || vertex_buffer->GetVertexCount() == 0)//如果模型没有顶点数据，则buffer会为null
		return;

	//重置固定渲染管线
	this->ResetRenderState();

	//Gpu
	CGProgram* shader = pass->GetProgram();
	shader->PreRender();
	shader->Render(camera, pass, primitive->GetWorldMatrix());
	shader->PostRender();

	//顶点声明和数据流
	vertex_buffer->Render(shader);

	//渲染状态更新
	this->UpdataRenderState(camera, pass, primitive);

	//获得渲染类型和渲染三角形个数
	D3DPRIMITIVETYPE prim_type = DX9GetPrimitiveType(primitive->GetPrimitiveType());

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
			DX9HR(GetDX9Device()->GetDevice()->DrawIndexedPrimitive(prim_type, prim.StartVertex, 0, vertex_buffer->GetVertexCount(), prim.StartIndex, prim_count));
			SceneManager::AddDrawCall(1);
			SceneManager::AddDrawTriangles(prim_count);
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
			DX9HR(GetDX9Device()->GetDevice()->DrawPrimitive(prim_type, prim.StartVertex, prim_count));
			SceneManager::AddDrawCall(1);
			SceneManager::AddDrawTriangles(prim_count);
		}
	}

	//统计
	SceneManager::AddRenderBatch(1);
	SceneManager::AddSetPassCall(1);
}
void DX9RenderContent::SetViewport(const ViewPortDesc& view_port)
{
	D3DVIEWPORT9 port;
	port.X = view_port.x; port.Y = view_port.y;
	port.Width = view_port.w; port.Height = view_port.h;
	port.MinZ = view_port.z_near; port.MaxZ = view_port.z_far;
	DX9HR(GetDX9Device()->GetDevice()->SetViewport(&port));
}
void DX9RenderContent::SetViewportScissor(const iRect& clip)
{
	RECT rect;
	rect.left = (LONG)clip.x;
	rect.top = (LONG)clip.y;
	rect.right = (LONG)(clip.x + clip.width);
	rect.bottom = (LONG)(clip.y + clip.height);
	DX9HR(GetDX9Device()->GetDevice()->SetScissorRect(&rect));
}
void DX9RenderContent::ClearBackbuffer(void* target_buffer, void* depth_stencil_buffer, ClearFlag flag, const Color& color)
{
	DWORD clear_flags = 0;
	if ((flag & ClearFlag::Color))clear_flags |= D3DCLEAR_TARGET;
	if ((flag & ClearFlag::Depth) && depth_stencil_buffer)clear_flags |= D3DCLEAR_ZBUFFER;
	if ((flag & ClearFlag::Stencil) && depth_stencil_buffer)clear_flags |= D3DCLEAR_STENCIL;
	if (!clear_flags)return;

	//depth:新的 zdepth 值，由这个方法储存在深度缓冲区中。这个参数在 0.0 到 1.0 的范围內 (针对以 z 为基础或以 w 为基础的深度缓冲区)。0.0 的值表示到监视器的最近距離，1.0 的值则表示最远距离
	DX9HR(GetDX9Device()->GetDevice()->Clear(0, NULL, clear_flags, DX9ToColor(color), 1.0f, 0));
}
void DX9RenderContent::Resize(const WindowResizeDesc& desc)
{
	base::Resize(desc);
}
void DX9RenderContent::ResetRenderState()
{
	//alpha混合
	GetDX9Device()->SetD3DRenderState(D3DRS_ALPHABLENDENABLE, false);
	GetDX9Device()->SetD3DRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	GetDX9Device()->SetD3DRenderState(D3DRS_BLENDOPALPHA, D3DBLENDOP_ADD);
	GetDX9Device()->SetD3DRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	GetDX9Device()->SetD3DRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
	GetDX9Device()->SetD3DRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_ONE);
	GetDX9Device()->SetD3DRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_ZERO);

	//alpha测试
	GetDX9Device()->SetD3DRenderState(D3DRS_ALPHATESTENABLE, false);

	GetDX9Device()->SetD3DRenderState(D3DRS_COLORVERTEX, FALSE);

	//灯光
	for (int i = 0; i < 8; ++i)
		GetDX9Device()->GetDevice()->LightEnable(i, FALSE);
	GetDX9Device()->GetDevice()->SetRenderState(D3DRS_AMBIENT, 0);
	GetDX9Device()->GetDevice()->SetRenderState(D3DRS_LIGHTING, FALSE);
	GetDX9Device()->GetDevice()->SetRenderState(D3DRS_SPECULARENABLE, FALSE);

	//重置多纹理混合
	for (int i = 0; i < 8; i++)
	{
		//纹理过滤
		GetDX9Device()->SetD3DSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);//放大
		GetDX9Device()->SetD3DSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);//缩小

		//mipmap
		GetDX9Device()->SetD3DSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

		//各向异性
		GetDX9Device()->SetD3DSamplerState(i, D3DSAMP_MAXANISOTROPY, 1);

		//纹理寻址模式
		GetDX9Device()->SetD3DSamplerState(i, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		GetDX9Device()->SetD3DSamplerState(i, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

		GetDX9Device()->ClearD3DTexture(i);
		GetDX9Device()->SetD3DTextureStageState(i, D3DTSS_COLOROP, D3DTOP_DISABLE);
		GetDX9Device()->SetD3DTextureStageState(i, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	}
}
void DX9RenderContent::UpdataRenderState(Camera* camera, Pass* pass, Primitive* primitive)
{
	//通用状态
	FillMode fill_mode = FillMode::Solid;
	if (camera)fill_mode = camera->GetFillMode();
	this->SetFillMode(fill_mode);
	this->SetShadeMode(SceneManager::GetShadeMode());

	{//光栅化
		//剔除模式
		SetCullMode(pass);
	}
	{//输出合并
		//裁剪
		SetScissor(pass);
		//模板测试
		SetStencil(pass);
		//深度测试
		SetDepth(pass);
		//mask
		SetColorMask(pass);
		//混合
		SetBlendState(pass);
	}
	//点
	if (primitive->GetPrimitiveType() == PrimitiveType::PointList)
	{
		uint point_size = primitive->GetPointSize() > 1.0f ? (uint)primitive->GetPointSize() : 1;
#if DC_DEBUG
		int max_size = GetGraphicsCaps()->maxPointSize;
		AssertEx(point_size <= max_size, "");
#endif
		GetDX9Device()->SetD3DRenderState(D3DRS_POINTSPRITEENABLE, FALSE);
		GetDX9Device()->SetD3DRenderState(D3DRS_POINTSIZE, point_size);
	}
	//线
	if (primitive->GetPrimitiveType() == PrimitiveType::LineList || primitive->GetPrimitiveType() == PrimitiveType::LineStrip)
	{
	}
}
void DX9RenderContent::SetCullMode(Pass* pass)
{
	GetDX9Device()->SetD3DRenderState(D3DRS_CULLMODE, DX9GetCullMode(pass->Cullmode));
}
void DX9RenderContent::SetScissor(Pass* pass)
{
	//undo在渲染时设置
}
void DX9RenderContent::SetDepth(Pass* pass)
{
	GetDX9Device()->SetD3DRenderState(D3DRS_ZENABLE, pass->DepthEnable);
	GetDX9Device()->SetD3DRenderState(D3DRS_ZWRITEENABLE, pass->DepthWriteEnable);
	GetDX9Device()->SetD3DRenderState(D3DRS_ZFUNC, DX9GetStencilCmp(pass->DepthCmpFun));
}
void DX9RenderContent::SetStencil(Pass* pass)
{
	if (!pass->StencilEnable)
	{
		GetDX9Device()->SetD3DRenderState(D3DRS_STENCILENABLE, false);
		GetDX9Device()->SetD3DRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
		GetDX9Device()->SetD3DRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
		GetDX9Device()->SetD3DRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
		return;
	}
	GetDX9Device()->SetD3DRenderState(D3DRS_STENCILENABLE, true);

	GetDX9Device()->SetD3DRenderState(D3DRS_STENCILFUNC, DX9GetStencilCmp(pass->StencilFunc));
	GetDX9Device()->SetD3DRenderState(D3DRS_STENCILREF, pass->StencilRef);
	GetDX9Device()->SetD3DRenderState(D3DRS_STENCILMASK, pass->StencilReadMask);
	GetDX9Device()->SetD3DRenderState(D3DRS_STENCILWRITEMASK, pass->StencilWriteMask);

	GetDX9Device()->SetD3DRenderState(D3DRS_STENCILFAIL, DX9GetStencilOp(pass->StencilFailOp));
	GetDX9Device()->SetD3DRenderState(D3DRS_STENCILZFAIL, DX9GetStencilOp(pass->StencilDepthFailOp));
	GetDX9Device()->SetD3DRenderState(D3DRS_STENCILPASS, DX9GetStencilOp(pass->StencilPassOp));
}
void DX9RenderContent::SetColorMask(Pass* pass)
{
	DWORD color_mask = 0;
	if (pass->ColorMask.x)color_mask |= D3DCOLORWRITEENABLE_RED;
	if (pass->ColorMask.y)color_mask |= D3DCOLORWRITEENABLE_GREEN;
	if (pass->ColorMask.z)color_mask |= D3DCOLORWRITEENABLE_BLUE;
	if (pass->ColorMask.w)color_mask |= D3DCOLORWRITEENABLE_ALPHA;
	GetDX9Device()->SetD3DRenderState(D3DRS_COLORWRITEENABLE, color_mask);
}
void DX9RenderContent::SetBlendState(Pass* pass)
{
	//alpha混合
	GetDX9Device()->SetD3DRenderState(D3DRS_ALPHABLENDENABLE, pass->BlendEnable);
	if (pass->BlendEnable)
	{
		GetDX9Device()->SetD3DRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		GetDX9Device()->SetD3DRenderState(D3DRS_BLENDOPALPHA, D3DBLENDOP_ADD);
		GetDX9Device()->SetD3DRenderState(D3DRS_SRCBLEND, DX9GetAlphaBlend(pass->SrcBlend));
		GetDX9Device()->SetD3DRenderState(D3DRS_DESTBLEND, DX9GetAlphaBlend(pass->DstBlend));
		GetDX9Device()->SetD3DRenderState(D3DRS_SRCBLENDALPHA, DX9GetAlphaBlend(pass->SrcAlphaSource));
		GetDX9Device()->SetD3DRenderState(D3DRS_DESTBLENDALPHA, DX9GetAlphaBlend(pass->DstAlphaSource));
	}
	else
	{
		GetDX9Device()->SetD3DRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		GetDX9Device()->SetD3DRenderState(D3DRS_BLENDOPALPHA, D3DBLENDOP_ADD);
		GetDX9Device()->SetD3DRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		GetDX9Device()->SetD3DRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
		GetDX9Device()->SetD3DRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_ONE);
		GetDX9Device()->SetD3DRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_ZERO);
	}

	//alpha测试
	GetDX9Device()->SetD3DRenderState(D3DRS_ALPHATESTENABLE, pass->AlphaTestEnable);
	if (pass->AlphaTestEnable)
	{
		GetDX9Device()->SetD3DRenderState(D3DRS_ALPHAFUNC, DX9GetStencilCmp(pass->AlphaTestCmpFun));
		GetDX9Device()->SetD3DRenderState(D3DRS_ALPHAREF, (uint)(pass->AlphaTestRef * 255));
	}
}
void DX9RenderContent::SetSubPrimitiveState(const SubPrimitive* sub_prim, Pass* pass)
{
	if (!sub_prim->Tex)return;
	CGProgram* shader = pass->GetProgram();
	shader->SetPassVariable(sub_prim->TexIndex, sub_prim->TexName, sub_prim->Tex);

	bool old_alpha_state = pass->BlendEnable;
	pass->BlendEnable = sub_prim->AlphaEnable;
	SetBlendState(pass);
	pass->BlendEnable = old_alpha_state;
}
void DX9RenderContent::HandleLostDevice()
{
	for (auto decl : _vertexDeclarations)
	{
		SAFE_RELEASE(decl.second);
	}
	_vertexDeclarations.clear();
}
void DX9RenderContent::HandleResetDevice()
{
}
DC_END_NAMESPACE