#include "PlaneRender.h"

 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(PlaneRender, Renderer);
void PlaneRender::Awake()
{
	base::Awake();
	SetPlane(10, 10, 10, 10, 1, 1);
}
void PlaneRender::SetPlane(float fWidth, float fHeight, ushort iXSegments, ushort iYSegments, ushort iUTiles, ushort iVTiles)
{
	this->ClearData();
	//顶点信息
	{
		int buffSize = (iXSegments + 1) * (iYSegments + 1);
		Vector3v vertexs; Vector3v normals; Vector3v tangents; Vector2v texcoords;
		vertexs.Reserve(buffSize); normals.Reserve(buffSize); tangents.Reserve(buffSize);  texcoords.Reserve(buffSize);

		Vector3 vec3; Vector2 vec2;
		float xGrid = fWidth / iXSegments;  //每格大小
		float yGrid = fHeight / iYSegments;
		float halfWidth = fWidth / 2;		//一半大小
		float halfHeight = fHeight / 2;
		float xTex = (1.0f * iUTiles) / iXSegments;
		float yTex = (1.0f * iVTiles) / iYSegments;
		for (int y = 0; y < iYSegments + 1; ++y)
		{
			for (int x = 0; x < iXSegments + 1; ++x)
			{
				//顶点数据
				vec3.x = (x * xGrid) - halfWidth;
				vec3.y = halfHeight - (y * yGrid);
				vec3.z = 0.0f;
				vertexs.Add(vec3);

				//法线数据
				normals.Add(-Vector3::forward);
				//切线数据
				tangents.Add(Vector3(1, 0, 0));

				//纹理坐标
				vec2.x = x * xTex;
				vec2.y = y * yTex;
				texcoords.Add(vec2);
			} // x
		} // y
		this->SetVertexs(std::move(vertexs));
		this->SetColor(Color::White);
		this->SetNormals(std::move(normals));
		this->SetTangents(std::move(tangents));
		this->SetTexcoords1(std::move(texcoords));
	}

	//索引信息
	{
		uint* indexs = Memory::NewArray<uint>(iXSegments * iYSegments * 6);
		int iPerEdgeVerCount = iXSegments + 1;
		int q = 0; int index = 0;
		for (ushort j = 0; j < iXSegments; j += 1)
		{
			int p = 0;
			for (ushort i = 0; i < iYSegments; i += 1)
			{
				indexs[index++] = (p*iPerEdgeVerCount + q);
				indexs[index++] = (p*iPerEdgeVerCount + q + iPerEdgeVerCount + 1);
				indexs[index++] = (p*iPerEdgeVerCount + q + iPerEdgeVerCount);
				indexs[index++] = (p*iPerEdgeVerCount + q);
				indexs[index++] = (p*iPerEdgeVerCount + q + 1);
				indexs[index++] = (p*iPerEdgeVerCount + q + iPerEdgeVerCount + 1);
				p++;
			}
			q++;
		}
		this->SetIndices(iXSegments * iYSegments * 6, indexs);
		Memory::DeleteArray(indexs);
	}
	this->UploadData();
}
void PlaneRender::Update()
{
	base::Update();
	if (_isDirty)
	{
		_isDirty = false;
		this->UploadData();
	}
}
void PlaneRender::SetColor(const Color& color)
{
	base::SetColor(color);
	_isDirty = true;
}
DC_END_NAMESPACE
