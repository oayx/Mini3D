#include "GLRenderContent.h"
#include "GLDevice.h"
#include "GLHardwareVertexBuffer.h"
#include "GLHardwareIndexBuffer.h"
#include "runtime/graphics/Material.h"
#include "runtime/graphics/Pass.h"
#include "runtime/graphics/TextureUnit.h"
#include "runtime/graphics/null/Texture.h"
#include "runtime/graphics/null/CGProgram.h"
#include "runtime/renderable/Renderer.h"
#include "runtime/scene/Camera.h"
#include "platform/RenderWindow.h"
#include "runtime/Application.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(GLRenderContent, RenderContent);
GLRenderContent::~GLRenderContent()
{
}
void GLRenderContent::Initialize()
{
	base::Initialize();
}
void GLRenderContent::SetFillMode(FillMode mode)
{
#if GL_POLYGON_MODE
	GL_ERROR(glPolygonMode(GL_FRONT_AND_BACK, GL_GetFillMode(mode)));
#endif
}
void GLRenderContent::SetShadeMode(ShadeMode mode)
{
#if GL_SHADE_MODEL && !defined(DC_PLATFORM_MAC)
	GL_ERROR(glShadeModel(GL_GetShadeMode(mode)));
#endif
}
void GLRenderContent::PreRender(RenderWindow* window)
{
	base::PreRender(window);

	GL_ERROR(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));
	GL_ERROR(glDepthMask(GL_TRUE));
	GL_ERROR(glStencilMask(GL_TRUE));
}
void GLRenderContent::PostRender()
{
	base::PostRender();
}
void GLRenderContent::BeginFrame(RenderFrameDesc& desc)
{
	this->SetViewport(desc.viewPort);
	ClearBackbuffer(desc.targetBuffer, desc.depthStencilBuffer, desc.clearFlag, desc.clearColor);
}
void GLRenderContent::RenderOnePrimitive(Camera* camera, Pass* pass, Primitive* primitive, RenderMode mode)
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
	UpdataRenderState(camera, pass, primitive);

	//获得渲染类型和渲染三角形个数
	uint prim_type = GL_GetPrimitiveType(primitive->GetPrimitiveType());

	HardwareIndexBuffer *index_buffer = primitive->GetIndexData();
	if (index_buffer)
	{
		index_buffer->Render();
		int index_per_size = GL_GetIndexSize(index_buffer->GetIndexType());
		for (int i = 0; i < primitive->GetSubPrimitiveCount(); ++i)
		{
			const SubPrimitive& prim = primitive->GetSubPrimitive(i);
			int prim_count = prim.GetTringles(primitive->GetPrimitiveType());
			this->SetViewportScissor(prim.ScissorRect);
			this->SetSubPrimitiveState(&prim, pass);
			if (vertex_buffer->GetInstanceCount() > 0)
			{
				GL_ERROR(glDrawElementsInstanced(prim_type, prim.ElemCount, GL_GetIndexType(index_buffer->GetIndexType()), GL_BUFFER_OFFSET(prim.IdxOffset*index_per_size), vertex_buffer->GetInstanceCount()));
				SceneManager::AddDrawTriangles(prim_count * vertex_buffer->GetInstanceCount());
			}
#if !defined(DC_PLATFORM_WASM) && !defined(DC_PLATFORM_IOS)
			else
			{
				GL_ERROR(glDrawElementsBaseVertex(prim_type, prim.ElemCount, GL_GetIndexType(index_buffer->GetIndexType()), GL_BUFFER_OFFSET(prim.IdxOffset*index_per_size), (GLint)prim.VtxOffset));
				SceneManager::AddDrawTriangles(prim_count);
			}
//#else
//			else
//			{
//				GL_ERROR(glDrawElementsBaseVertexEXT(prim_type, prim.ElemCount, GL_GetIndexType(index_buffer->GetIndexType()), GL_BUFFER_OFFSET(prim.IdxOffset * index_per_size), (GLint)prim.VtxOffset));
//				SceneManager::AddDrawTriangles(prim_count);
//			}
#else
			else
			{
				GL_ERROR(glDrawElements(prim_type, prim.ElemCount, GL_GetIndexType(index_buffer->GetIndexType()), GL_BUFFER_OFFSET(prim.IdxOffset*index_per_size)));
				SceneManager::AddDrawTriangles(prim_count);
			}
#endif
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
				GL_ERROR(glDrawArraysInstanced(prim_type, prim.StartVertex, prim.ElemCount, vertex_buffer->GetInstanceCount()));
				SceneManager::AddDrawTriangles(prim_count * vertex_buffer->GetInstanceCount());
			}
			else
			{
				GL_ERROR(glDrawArrays(prim_type, prim.StartVertex, prim.ElemCount));
				SceneManager::AddDrawTriangles(prim_count);
			}
			SceneManager::AddDrawCall(1);
		}
	}

	//统计
	SceneManager::AddRenderBatch(1);
	SceneManager::AddSetPassCall(1);
}
void GLRenderContent::SetViewport(const ViewPortDesc& viewPort)
{
	GL_ERROR(glViewport((GLint)viewPort.x, (GLint)viewPort.y, (GLsizei)viewPort.w, (GLsizei)viewPort.h));
	GL_ERROR(glDepthRangef(viewPort.z_near, viewPort.z_far));
	this->SetViewportScissor(iRect((int)viewPort.x, (int)viewPort.y, (int)viewPort.w, (int)viewPort.h));
}
void GLRenderContent::SetViewportScissor(const iRect& clip)
{
	//glScissor以左下角为坐标原点(0,0)
	int x = clip.x;
	int y = _currRenderWindow->GetHeight() - (clip.y + clip.height);
	int w = clip.width;
	int h = clip.height;
	GL_ERROR(glScissor(x, y, w, h));
}
void GLRenderContent::ClearBackbuffer(void* targetBuffer, void* depthStencilBuffer, ClearFlag flag, const Color& color)
{
	uint clear_flags = 0;
	switch (flag)
	{
	case ClearFlag::Skybox:
	case ClearFlag::SolidColor:
		clear_flags = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;
		break;
	case ClearFlag::DepthOnly:
		clear_flags = GL_DEPTH_BUFFER_BIT;
		break;
	case ClearFlag::DonotClear:
		return;
	default: return;
	}

	if (clear_flags & GL_COLOR_BUFFER_BIT)
	{
		GL_ERROR(glClearColor(color.r, color.g, color.b, color.a));
	}

	if (clear_flags & GL_DEPTH_BUFFER_BIT)
	{
		GL_ERROR(glClearDepth(1.0f));
	}

	if (clear_flags & GL_STENCIL_BUFFER_BIT)
	{
		GL_ERROR(glClearStencil(0));
	}

	GL_ERROR(glClear(clear_flags));
}
void GLRenderContent::Resize(const WindowResizeDesc& desc)
{ 
	base::Resize(desc);
}
void GLRenderContent::UpdataRenderState(Camera* camera, Pass* pass, Primitive* primitive)
{
	//通用状态
	FillMode fill_mode = FillMode::Solid;
	if (camera)fill_mode = camera->GetFillMode();
	this->SetFillMode(fill_mode);
	this->SetShadeMode(SceneManager::GetShadeMode());

	{//光栅化
		//剔除模式
		SetCullMode(pass);
		//深度偏移
		SetDepthOffset(pass);
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
		float point_size = primitive->GetPointSize() > 0 ? primitive->GetPointSize() : 1.0f;
#if DC_DEBUG
		int max_size = GetGraphicsCaps()->maxPointSize;
		AssertEx(point_size <= (float)max_size, "");
#endif
#if defined(DC_GRAPHICS_API_OPENGL)
		GL_ERROR(glPointSize(point_size));
#endif
	}
	//线
	if (primitive->GetPrimitiveType() == PrimitiveType::LineList || primitive->GetPrimitiveType() == PrimitiveType::LineStrip)
	{
#if DC_DEBUG
		int max_width = GetGraphicsCaps()->maxLineWidth;
		AssertEx(primitive->GetLineWidth() <= (float)max_width, "");
#endif
		GL_ERROR(glLineWidth(primitive->GetLineWidth()));
	}
}
void GLRenderContent::SetCullMode(Pass* pass)
{
	CullMode mode = pass->Cullmode;
#if GL_CULL_FACE_MODE
	if (mode == CullMode::Off)
	{
		GL_ERROR(glDisable(GL_CULL_FACE));
	}
	else
	{
		GL_ERROR(glEnable(GL_CULL_FACE));
		GL_ERROR(glCullFace(GL_GetCullMode(mode)));
	}
#endif
}
void GLRenderContent::SetScissor(Pass* pass)
{
	if (pass->GetMaterial()->IsScissorEnable())
		GL_ERROR(glEnable(GL_SCISSOR_TEST));
	else
		GL_ERROR(glDisable(GL_SCISSOR_TEST));
}
void GLRenderContent::SetDepth(Pass* pass)
{
#if defined(GL_DEPTH_CLAMP)
	GL_ERROR(glDisable(GL_DEPTH_CLAMP));//禁用深度夹紧
#endif
	if (pass->DepthEnable)
		GL_ERROR(glEnable(GL_DEPTH_TEST));
	else
		GL_ERROR(glDisable(GL_DEPTH_TEST));
	GL_ERROR(glDepthMask(pass->DepthWriteEnable ? GL_TRUE : GL_FALSE));
	GL_ERROR(glDepthFunc(GL_GetStencilCmp(pass->DepthCmpFun)));
}
void GLRenderContent::SetStencil(Pass* pass)
{
	if (!pass->StencilEnable)
	{
		GL_ERROR(glDisable(GL_STENCIL_TEST));
		GL_ERROR(glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP));
		GL_ERROR(glStencilMask(0));
		return;
	}
	GL_ERROR(glEnable(GL_STENCIL_TEST));
	GL_ERROR(glStencilFunc(GL_GetStencilCmp(pass->StencilFunc), pass->StencilRef, pass->StencilReadMask));
	GL_ERROR(glStencilOp(GL_GetStencilOp(pass->StencilFailOp), GL_GetStencilOp(pass->StencilDepthFailOp), GL_GetStencilOp(pass->StencilPassOp)));
	GL_ERROR(glStencilMask(pass->StencilWriteMask));
}
void GLRenderContent::SetColorMask(Pass* pass)
{
	GL_ERROR(glColorMask(pass->ColorMask.x ? GL_TRUE : GL_FALSE, pass->ColorMask.y ? GL_TRUE : GL_FALSE, pass->ColorMask.z ? GL_TRUE : GL_FALSE, pass->ColorMask.w ? GL_TRUE : GL_FALSE));
}
void GLRenderContent::SetBlendState(Pass* pass)
{
	//alpha混合
	if (pass->BlendEnable)
		GL_ERROR(glEnable(GL_BLEND));
	else
		GL_ERROR(glDisable(GL_BLEND));
	GL_ERROR(glBlendFuncSeparate(GL_GetAlphaBlend(pass->SrcBlend), GL_GetAlphaBlend(pass->DstBlend),GL_GetAlphaBlend(pass->SrcAlphaSource), GL_GetAlphaBlend(pass->DstAlphaSource)));
	GL_ERROR(glBlendEquation(GL_FUNC_ADD));

	//alpha测试，mac平台4.1不支持
#if defined(DC_GRAPHICS_API_OPENGL) && !defined(DC_PLATFORM_MAC)
	if (pass->AlphaTestEnable)
		GL_ERROR(glEnable(GL_ALPHA_TEST));
	else
		GL_ERROR(glDisable(GL_ALPHA_TEST));
	if (pass->AlphaTestEnable)
	{
		GL_ERROR(glAlphaFunc(GL_GetStencilCmp(pass->AlphaTestCmpFun), pass->AlphaTestRef));
	}
#endif
}
/*防止深度冲突
	1.第一个也是最重要的技巧是让物体之间不要离得太近，以至于他们的三角形重叠。通过在物体之间制造一点用户无法察觉到的偏移，可以完全解决深度冲突。在容器和平面的条件下，我们可以把容器像+y方向上略微移动。这微小的改变可能完全不被注意但是可以有效地减少或者完全解决深度冲突。然而这需要人工的干预每个物体，并进行彻底地测试，以确保这个场景的物体之间没有深度冲突。
	2.另一个技巧是尽可能把近平面设置得远一些。前面我们讨论过越靠近近平面的位置精度越高。所以我们移动近平面远离观察者，我们可以在椎体内很有效的提高精度。然而把近平面移动的太远会导致近处的物体被裁剪掉。所以不断调整测试近平面的值，为你的场景找出最好的近平面的距离。
	3.另外一个技巧是放弃一些性能来得到更高的深度值的精度。大多数的深度缓冲区都是24位。但现在显卡支持32位深度值，这让深度缓冲区的精度提高了一大节。所以牺牲一些性能你会得到更精确的深度测试，减少深度冲突。
	我们已经讨论过的 3 个技术是最常见和容易实现消除深度冲突的技术。还有一些其他技术需要更多的工作，仍然不会完全消除深度冲突。深度冲突是一个常见的问题，但如果你将列举的技术适当结合你可能不会真的需要处理深度冲突
*/
void GLRenderContent::SetDepthOffset(Pass* pass)
{
	Material* material = pass->GetMaterial();
	if (material->IsDepthOffsetEnable())
	{
		GL_ERROR(glEnable(GL_POLYGON_OFFSET_FILL));
		GL_ERROR(glPolygonOffset(material->GetDepthOffsetFactor(), material->GetDepthOffsetUnits()));
	}
	else
	{
		GL_ERROR(glDisable(GL_POLYGON_OFFSET_FILL));
	}
}
void GLRenderContent::SetSubPrimitiveState(const SubPrimitive* subPrim, Pass* pass)
{
	if (!subPrim->Tex)return;
	CGProgram* shader = pass->GetProgram();
	shader->SetPassVariable(subPrim->TexIndex, subPrim->TexName, subPrim->Tex);

	bool old_alpha_state = pass->BlendEnable;
	pass->BlendEnable = subPrim->AlphaEnable;
	SetBlendState(pass);
	pass->BlendEnable = old_alpha_state;
}
DC_END_NAMESPACE