#include "SkyBox.h"
#include "runtime/graphics/Material.h"
#include "runtime/graphics/null/Texture.h" 
#include "runtime/scene/Camera.h"
#include "runtime/scene/SceneManager.h"
#include "runtime/Application.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(SkyBox, Object);
SkyBox::SkyBox()
{
	_matProj = Matrix4::Perspective(Math::PI / 4.0f, 1.0f, 0.1f, 1000.0f);
	_primitive = dynamic_cast<VariablePrimitive*>(Primitive::Create(nullptr, PrimitiveType::TriangleList, VertexLayout::Variable));
	//顶点信息
	{
		// 屏幕里面 0,1,2,3
		_primitive->AddVertex(Vector3(0.5f, -0.5f, 0.5f));
		_primitive->AddVertex(Vector3(-0.5f, -0.5f, 0.5f));
		_primitive->AddVertex(Vector3(0.5f, 0.5f, 0.5f));
		_primitive->AddVertex(Vector3(-0.5f, 0.5f, 0.5f));
		// 屏幕外面 4,5,6,7
		_primitive->AddVertex(Vector3(0.5f, 0.5f, -0.5f));
		_primitive->AddVertex(Vector3(-0.5f, 0.5f, -0.5f));
		_primitive->AddVertex(Vector3(0.5f, -0.5f, -0.5f));
		_primitive->AddVertex(Vector3(-0.5f, -0.5f, -0.5f));
		// 上面 8,9,10,11
		_primitive->AddVertex(Vector3(0.5f, 0.5f, 0.5f));
		_primitive->AddVertex(Vector3(-0.5f, 0.5f, 0.5f));
		_primitive->AddVertex(Vector3(0.5f, 0.5f, -0.5f));
		_primitive->AddVertex(Vector3(-0.5f, 0.5f, -0.5f));
		// 下面 12,13,14,15
		_primitive->AddVertex(Vector3(0.5f, -0.5f, -0.5f));
		_primitive->AddVertex(Vector3(0.5f, -0.5f, 0.5f));
		_primitive->AddVertex(Vector3(-0.5f, -0.5f, 0.5f));
		_primitive->AddVertex(Vector3(-0.5f, -0.5f, -0.5f));
		// 左面 16,17,18,19
		_primitive->AddVertex(Vector3(-0.5f, -0.5f, 0.5f));
		_primitive->AddVertex(Vector3(-0.5f, 0.5f, 0.5f));
		_primitive->AddVertex(Vector3(-0.5f, 0.5f, -0.5f));
		_primitive->AddVertex(Vector3(-0.5f, -0.5f, -0.5f));
		// 右面 20,21,22,23
		_primitive->AddVertex(Vector3(0.5f, -0.5f, -0.5f));
		_primitive->AddVertex(Vector3(0.5f, 0.5f, -0.5f));
		_primitive->AddVertex(Vector3(0.5f, 0.5f, 0.5f));
		_primitive->AddVertex(Vector3(0.5f, -0.5f, 0.5f));
	}

	//索引信息
	{
		// 屏幕里面
		_primitive->AddIndex(0);
		_primitive->AddIndex(2);
		_primitive->AddIndex(3);

		_primitive->AddIndex(0);
		_primitive->AddIndex(3);
		_primitive->AddIndex(1);

		// 上面
		_primitive->AddIndex(8);
		_primitive->AddIndex(4);
		_primitive->AddIndex(5);

		_primitive->AddIndex(8);
		_primitive->AddIndex(5);
		_primitive->AddIndex(9);

		// 屏幕外面
		_primitive->AddIndex(10);
		_primitive->AddIndex(6);
		_primitive->AddIndex(7);

		_primitive->AddIndex(10);
		_primitive->AddIndex(7);
		_primitive->AddIndex(11);

		// 下面
		_primitive->AddIndex(12);
		_primitive->AddIndex(13);
		_primitive->AddIndex(14);

		_primitive->AddIndex(12);
		_primitive->AddIndex(14);
		_primitive->AddIndex(15);

		// 左面
		_primitive->AddIndex(16);
		_primitive->AddIndex(17);
		_primitive->AddIndex(18);

		_primitive->AddIndex(16);
		_primitive->AddIndex(18);
		_primitive->AddIndex(19);

		// 右面
		_primitive->AddIndex(20);
		_primitive->AddIndex(21);
		_primitive->AddIndex(22);

		_primitive->AddIndex(20);
		_primitive->AddIndex(22);
		_primitive->AddIndex(23);
	}
	_primitive->UploadData(PrimitiveUploadDesc());
}
SkyBox::~SkyBox()
{
	SAFE_RELEASE(_cubeTexture);
	SAFE_DELETE(_primitive);
}
void SkyBox::Render(Camera* camera)
{
	Matrix4 matView = camera->GetViewMatrix();
	matView.Translate(Vector3::zero);//去掉相机位移
	_primitive->GetMaterial()->SetMatrix("MATRIX_SKYBOX", matView * _matProj);

	Application::GetGraphics()->GetRenderContent()->RenderOnePrimitive(camera, _primitive->GetMaterial(), _primitive, RenderMode::Normal);
}
void SkyBox::SetCubeFile(const String& file)
{
	SAFE_RELEASE(_cubeTexture);

	Material* mat = Material::Create(file.IsEmpty() ? "internal/material/SkyBox.material" : file);
	_primitive->SetMaterial(mat);
	_cubeTexture = mat->GetTexture();
	if (_cubeTexture)_cubeTexture->Retain();
}
Texture* SkyBox::GetCubeTexture()
{
	if (!_cubeTexture)
	{
		_cubeTexture = _primitive->GetMaterial()->GetTexture();
		if (_cubeTexture)_cubeTexture->Retain();
	}
	return _cubeTexture;
}
DC_END_NAMESPACE