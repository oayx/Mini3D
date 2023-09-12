#include "ManualPlane.h"


BEGING_NAMESPACE_DC
/********************************************************************/
ManualPlane::ManualPlane(const String &name, float fWidth, float fHeight, ushort iXSegments, ushort iYSegments, ushort iUTiles, ushort iVTiles)
: base(OT_TRIANGLE_LIST, name)
{
	m_bUseIndexData = true;

	//顶点信息
	{
		int buff_size = (iXSegments + 1) * (iYSegments + 1);
		VecVector3 vertexs; VecVector3 normals; VecVector4 tangents; VecVector2 texcoords;
		vertexs.reserve(buff_size); normals.reserve(buff_size); tangents.reserve(buff_size);  texcoords.reserve(buff_size);

		//d3d纹理坐标是向下的，导致v方向的副法线反了
		float tangent_w = 1;
#if defined(USE_DIRECT3D)
			tangent_w = -1;
#endif
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
				vertexs.push_back(vec3);

				//法线数据
				normals.push_back(-Vector3::forward);
				//切线数据
				tangents.push_back(Vector4(1, 0, 0, tangent_w));

				//纹理坐标
				vec2.x = x * xTex;
				vec2.y = y * yTex;
				texcoords.push_back(vec2);
			} // x
		} // y
		SetVertex(vertexs);
		SetVertexNormal(normals);
		SetVertexTangent(tangents);
		AddTexcoord(texcoords);
	}

	//索引信息
	{
		ushort* indexs = new ushort[iXSegments * iYSegments * 6];
		int iPerEdgeVerCount = iXSegments + 1;
		int q = 0; int index = 0;
		for (ushort j = 0; j < iXSegments; j += 1)
		{
			int p = 0;
			for (ushort i = 0; i < iYSegments; i += 1)
			{
				indexs[index++] = (ushort)(p*iPerEdgeVerCount + q);
				indexs[index++] = (ushort)(p*iPerEdgeVerCount + q + iPerEdgeVerCount + 1);
				indexs[index++] = (ushort)(p*iPerEdgeVerCount + q + iPerEdgeVerCount);
				indexs[index++] = (ushort)(p*iPerEdgeVerCount + q);
				indexs[index++] = (ushort)(p*iPerEdgeVerCount + q + 1);
				indexs[index++] = (ushort)(p*iPerEdgeVerCount + q + iPerEdgeVerCount + 1);
				p++;
			}
			q++;
		}
		SetIndex(iXSegments * iYSegments * 6, indexs);
		SAFE_DELETE_ARRAY(indexs);
	}
}
END_NAMESPACE_DC