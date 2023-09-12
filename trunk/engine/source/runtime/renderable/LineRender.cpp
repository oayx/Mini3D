#include "LineRender.h"
#include "runtime/graphics/null/Texture.h"
#include "runtime/graphics/Material.h"
#include "runtime/graphics/Pass.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(LineRender, Renderer);
void LineRender::Awake()
{
	base::Awake();
	base::SetPrimitiveType(PrimitiveType::LineStrip);
	CreateMaterial();
}
void LineRender::CreateMaterial()
{
	base::SetMaterial("internal/material/Line.material");
}
void LineRender::AddVertex(const Vector3& pos, const Color& color)
{
	base::AddVertex(pos);
	base::AddColor(color);
}
void LineRender::Begin()
{
	this->ClearData();
}
void LineRender::End()
{
	this->UploadData();
}
void LineRender::SetVertexs(const Vector3v&& vertexs)
{
	this->ClearData();
	base::SetVertexs(std::move(vertexs));
	this->UploadData();
}
void LineRender::SetVertexs(const Vector3v&& vertexs, const Color& color)
{
	this->ClearData();
	base::SetVertexs(std::move(vertexs));
	base::SetColor(color);
	this->UploadData();
}
void LineRender::SetWidth(float width)
{
	this->GetPrimitive(0)->SetLineWidth(width);
}
void LineRender::SetPrimitiveType(PrimitiveType type)
{
	AssertEx(type == PrimitiveType::LineList || type == PrimitiveType::LineStrip, "PrimitiveType error:%d", (int)type);
	base::SetPrimitiveType(type);
}
DC_END_NAMESPACE