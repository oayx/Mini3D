#include "QuadRender.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(QuadRender, Renderer);
void QuadRender::Awake()
{
	base::Awake();

	{
		//与屏幕平行，法线垂直屏幕向外
		this->SetVertexs({ Vector3(-0.5f, 0.5f, 0), Vector3(0.5f, 0.5f, 0), Vector3(0.5f, -0.5f, 0), Vector3(-0.5f, -0.5f, 0) });
		this->SetColor(Color::White);
		this->SetNormals({ Vector3(0, 0, -1), Vector3(0, 0, -1), Vector3(0, 0, -1), Vector3(0, 0, -1) });
		this->SetTangents({ Vector3(1, 0, 0), Vector3(1, 0, 0), Vector3(1, 0, 0), Vector3(1, 0, 0) });
		this->SetTexcoords1({ Vector2(0, 0), Vector2(1, 0), Vector2(1, 1), Vector2(0, 1) });

		uint index[] = { 0, 1, 2, 0, 2, 3 };
		this->SetIndices(6, index);
	}

	this->UploadData();
}
void QuadRender::Update()
{
	base::Update();
	if (_isDirty)
	{
		_isDirty = false;
		this->UploadData();
	}
}
void QuadRender::SetColor(const Color& color)
{
	base::SetColor(color);
	_isDirty = true;
}
DC_END_NAMESPACE
