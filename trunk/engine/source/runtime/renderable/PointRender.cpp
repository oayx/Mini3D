#include "PointRender.h"
#include "runtime/graphics/null/Texture.h"
#include "runtime/graphics/Material.h"
#include "runtime/graphics/Pass.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(PointRender, Renderer);
void PointRender::Awake()
{
	base::Awake();
	SetPrimitiveType(PrimitiveType::PointList);
	CreateMaterial("internal/material/Point.material");
}
void PointRender::CreateMaterial(const String& file)
{
	base::SetMaterial(file);
}
void PointRender::SetVertexs(const Vector3v&& vertexs)
{
	this->ClearData();
	base::SetVertexs(std::move(vertexs));
	this->UploadData();
}
void PointRender::SetVertexs(const Vector3v&& vertexs, const Color& color)
{
	this->ClearData();
	base::SetVertexs(std::move(vertexs));
	base::SetColor(color);
	this->UploadData();
}
void PointRender::SetVertexs(const Vector3v&& vertexs, float size)
{
	this->ClearData();
	base::SetVertexs(std::move(vertexs));
	base::SetPointSize(size);
	this->UploadData();
}
void PointRender::SetVertexs(const Vector3v&& vertexs, const Color& color, float size)
{
	this->ClearData();
	base::SetVertexs(std::move(vertexs));
	base::SetColor(color);
	base::SetPointSize(size);
	this->UploadData();
}
void PointRender::SetIsSprite()
{
	CreateMaterial("internal/material/PointSprite.material");
}
DC_END_NAMESPACE