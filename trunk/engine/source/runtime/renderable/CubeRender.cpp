#include "CubeRender.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(CubeRender, Renderer);
void CubeRender::Awake()
{
	base::Awake();
	//顶点信息
	{
		// 本来只需要8个顶点就可以了。
		// 但是由于6个四边形面的纹理坐标，不能共用顶点的纹理坐标
		// 所以需要8x3个顶点（同样的顶点被3个四边形面共用)
		Vector3v vertexs; Vector3v normals; Vector3v tangents; Vector2v texcoords;
		vertexs.Reserve(24); normals.Reserve(24); tangents.Reserve(24);  texcoords.Reserve(24);

		// 屏幕里面 0,1,2,3
		vertexs.Add(Vector3(0.5f, -0.5f, 0.5f));
		vertexs.Add(Vector3(-0.5f, -0.5f, 0.5f));
		vertexs.Add(Vector3(0.5f, 0.5f, 0.5f));
		vertexs.Add(Vector3(-0.5f, 0.5f, 0.5f));

		normals.Add(Vector3::forward);
		normals.Add(Vector3::forward);
		normals.Add(Vector3::forward);
		normals.Add(Vector3::forward);
		tangents.Add(Vector3(-1, 0, 0));
		tangents.Add(Vector3(-1, 0, 0));
		tangents.Add(Vector3(-1, 0, 0));
		tangents.Add(Vector3(-1, 0, 0));
		// 屏幕外面 4,5,6,7
		vertexs.Add(Vector3(0.5f, 0.5f, -0.5f));
		vertexs.Add(Vector3(-0.5f, 0.5f, -0.5f));
		vertexs.Add(Vector3(0.5f, -0.5f, -0.5f));
		vertexs.Add(Vector3(-0.5f, -0.5f, -0.5f));
		normals.Add(Vector3::up);
		normals.Add(Vector3::up);
		normals.Add(-Vector3::forward);
		normals.Add(-Vector3::forward);
		tangents.Add(Vector3(-1, 0, 0));
		tangents.Add(Vector3(-1, 0, 0));
		tangents.Add(Vector3(-1, 0, 0));
		tangents.Add(Vector3(-1, 0, 0));
		// 上面 8,9,10,11
		vertexs.Add(Vector3(0.5f, 0.5f, 0.5f));
		vertexs.Add(Vector3(-0.5f, 0.5f, 0.5f));
		vertexs.Add(Vector3(0.5f, 0.5f, -0.5f));
		vertexs.Add(Vector3(-0.5f, 0.5f, -0.5f));
		normals.Add(Vector3::up);
		normals.Add(Vector3::up);
		normals.Add(-Vector3::forward);
		normals.Add(-Vector3::forward);
		tangents.Add(Vector3(-1, 0, 0));
		tangents.Add(Vector3(-1, 0, 0));
		tangents.Add(Vector3(-1, 0, 0));
		tangents.Add(Vector3(-1, 0, 0));
		// 下面 12,13,14,15
		vertexs.Add(Vector3(0.5f, -0.5f, -0.5f));
		vertexs.Add(Vector3(0.5f, -0.5f, 0.5f));
		vertexs.Add(Vector3(-0.5f, -0.5f, 0.5f));
		vertexs.Add(Vector3(-0.5f, -0.5f, -0.5f));
		normals.Add(-Vector3::up);
		normals.Add(-Vector3::up);
		normals.Add(-Vector3::up);
		normals.Add(-Vector3::up);
		tangents.Add(Vector3(-1, 0, 0));
		tangents.Add(Vector3(-1, 0, 0));
		tangents.Add(Vector3(-1, 0, 0));
		tangents.Add(Vector3(-1, 0, 0));
		// 左面 16,17,18,19
		vertexs.Add(Vector3(-0.5f, -0.5f, 0.5f));
		vertexs.Add(Vector3(-0.5f, 0.5f, 0.5f));
		vertexs.Add(Vector3(-0.5f, 0.5f, -0.5f));
		vertexs.Add(Vector3(-0.5f, -0.5f, -0.5f));
		normals.Add(-Vector3::right);
		normals.Add(-Vector3::right);
		normals.Add(-Vector3::right);
		normals.Add(-Vector3::right);
		tangents.Add(Vector3(0, 0, -1));
		tangents.Add(Vector3(0, 0, -1));
		tangents.Add(Vector3(0, 0, -1));
		tangents.Add(Vector3(0, 0, -1));
		// 右面 20,21,22,23
		vertexs.Add(Vector3(0.5f, -0.5f, -0.5f));
		vertexs.Add(Vector3(0.5f, 0.5f, -0.5f));
		vertexs.Add(Vector3(0.5f, 0.5f, 0.5f));
		vertexs.Add(Vector3(0.5f, -0.5f, 0.5f));
		normals.Add(Vector3::right);
		normals.Add(Vector3::right);
		normals.Add(Vector3::right);
		normals.Add(Vector3::right);
		tangents.Add(Vector3(0, 0, 1));
		tangents.Add(Vector3(0, 0, 1));
		tangents.Add(Vector3(0, 0, 1));
		tangents.Add(Vector3(0, 0, 1));

		// 屏幕里面
		texcoords.Add(Vector2(0, 0));
		texcoords.Add(Vector2(1, 0));
		texcoords.Add(Vector2(0, 1));
		texcoords.Add(Vector2(1, 1));
		// 屏幕外面
		texcoords.Add(Vector2(0, 1));
		texcoords.Add(Vector2(1, 1));
		texcoords.Add(Vector2(0, 1));
		texcoords.Add(Vector2(1, 1));
		// 上面
		texcoords.Add(Vector2(0, 0));
		texcoords.Add(Vector2(1, 0));
		texcoords.Add(Vector2(0, 0));
		texcoords.Add(Vector2(1, 0));
		// 下面
		texcoords.Add(Vector2(0, 0));
		texcoords.Add(Vector2(0, 1));
		texcoords.Add(Vector2(1, 1));
		texcoords.Add(Vector2(1, 0));
		// 左面
		texcoords.Add(Vector2(0, 0));
		texcoords.Add(Vector2(0, 1));
		texcoords.Add(Vector2(1, 1));
		texcoords.Add(Vector2(1, 0));
		// 右面
		texcoords.Add(Vector2(0, 0));
		texcoords.Add(Vector2(0, 1));
		texcoords.Add(Vector2(1, 1));
		texcoords.Add(Vector2(1, 0));

		this->SetVertexs(std::move(vertexs));
		this->SetColor(Color::White);
		this->SetNormals(std::move(normals));
		this->SetTangents(std::move(tangents));
		this->SetTexcoords1(std::move(texcoords));
	}

	//索引信息
	{
		Vector<uint> indexs;

		// 屏幕里面
		indexs.Add(0);
		indexs.Add(2);
		indexs.Add(3);

		indexs.Add(0);
		indexs.Add(3);
		indexs.Add(1);

		// 上面
		indexs.Add(8);
		indexs.Add(4);
		indexs.Add(5);

		indexs.Add(8);
		indexs.Add(5);
		indexs.Add(9);

		// 屏幕外面
		indexs.Add(10);
		indexs.Add(6);
		indexs.Add(7);

		indexs.Add(10);
		indexs.Add(7);
		indexs.Add(11);

		// 下面
		indexs.Add(12);
		indexs.Add(13);
		indexs.Add(14);

		indexs.Add(12);
		indexs.Add(14);
		indexs.Add(15);

		// 左面
		indexs.Add(16);
		indexs.Add(17);
		indexs.Add(18);

		indexs.Add(16);
		indexs.Add(18);
		indexs.Add(19);

		// 右面
		indexs.Add(20);
		indexs.Add(21);
		indexs.Add(22);

		indexs.Add(20);
		indexs.Add(22);
		indexs.Add(23);

		base::SetIndices(std::move(indexs));
	}
	this->UploadData();
}
void CubeRender::Update()
{
	base::Update();
	if (_isDirty)
	{
		_isDirty = false;
		this->UploadData();
	}
}
void CubeRender::SetColor(const Color& color)
{
	base::SetColor(color);
	_isDirty = true;
}
DC_END_NAMESPACE
