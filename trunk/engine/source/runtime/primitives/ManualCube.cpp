#include "ManualCube.h"

BEGING_NAMESPACE_DC
/********************************************************************/
ManualCube::ManualCube(const String &name)
: base(OT_TRIANGLE_LIST, name)
{
	m_bUseIndexData = true;

	//顶点信息
	{
		// 本来只需要8个顶点就可以了。
		// 但是由于6个四边形面的纹理坐标，不能共用顶点的纹理坐标
		// 所以需要8x3个顶点（同样的顶点被3个四边形面共用)
		VecVector3 vertexs; VecVector3 normals; VecVector4 tangents; VecVector2 texcoords;
		vertexs.reserve(24); normals.reserve(24); tangents.reserve(24);  texcoords.reserve(24);

		//d3d纹理坐标是向下的，导致v方向的副法线反了
		float tangent_w = 1;
#if defined(USE_DIRECT3D)
		tangent_w = -1;
#endif
		// 屏幕里面 0,1,2,3
		vertexs.push_back(Vector3(0.5f, -0.5f, 0.5f));
		vertexs.push_back(Vector3(-0.5f, -0.5f, 0.5f));
		vertexs.push_back(Vector3(0.5f, 0.5f, 0.5f));
		vertexs.push_back(Vector3(-0.5f, 0.5f, 0.5f));
		normals.push_back(Vector3::forward);
		normals.push_back(Vector3::forward);
		normals.push_back(Vector3::forward);
		normals.push_back(Vector3::forward);
		tangents.push_back(Vector4(-1, 0, 0, tangent_w));
		tangents.push_back(Vector4(-1, 0, 0, tangent_w));
		tangents.push_back(Vector4(-1, 0, 0, tangent_w));
		tangents.push_back(Vector4(-1, 0, 0, tangent_w));
		// 屏幕外面 4,5,6,7
		vertexs.push_back(Vector3(0.5f, 0.5f, -0.5f));
		vertexs.push_back(Vector3(-0.5f, 0.5f, -0.5f));
		vertexs.push_back(Vector3(0.5f, -0.5f, -0.5f));
		vertexs.push_back(Vector3(-0.5f, -0.5f, -0.5f));
		normals.push_back(Vector3::up);
		normals.push_back(Vector3::up);
		normals.push_back(-Vector3::forward);
		normals.push_back(-Vector3::forward);
		tangents.push_back(Vector4(-1, 0, 0, tangent_w));
		tangents.push_back(Vector4(-1, 0, 0, tangent_w));
		tangents.push_back(Vector4(-1, 0, 0, tangent_w));
		tangents.push_back(Vector4(-1, 0, 0, tangent_w));
		// 上面 8,9,10,11
		vertexs.push_back(Vector3(0.5f, 0.5f, 0.5f));
		vertexs.push_back(Vector3(-0.5f, 0.5f, 0.5f));
		vertexs.push_back(Vector3(0.5f, 0.5f, -0.5f));
		vertexs.push_back(Vector3(-0.5f, 0.5f, -0.5f));
		normals.push_back(Vector3::up);
		normals.push_back(Vector3::up);
		normals.push_back(-Vector3::forward);
		normals.push_back(-Vector3::forward);
		tangents.push_back(Vector4(-1, 0, 0, tangent_w));
		tangents.push_back(Vector4(-1, 0, 0, tangent_w));
		tangents.push_back(Vector4(-1, 0, 0, tangent_w));
		tangents.push_back(Vector4(-1, 0, 0, tangent_w));
		// 下面 12,13,14,15
		vertexs.push_back(Vector3(0.5f, -0.5f, -0.5f));
		vertexs.push_back(Vector3(0.5f, -0.5f, 0.5f));
		vertexs.push_back(Vector3(-0.5f, -0.5f, 0.5f));
		vertexs.push_back(Vector3(-0.5f, -0.5f, -0.5f));
		normals.push_back(-Vector3::up);
		normals.push_back(-Vector3::up);
		normals.push_back(-Vector3::up);
		normals.push_back(-Vector3::up);
		tangents.push_back(Vector4(-1, 0, 0, tangent_w));
		tangents.push_back(Vector4(-1, 0, 0, tangent_w));
		tangents.push_back(Vector4(-1, 0, 0, tangent_w));
		tangents.push_back(Vector4(-1, 0, 0, tangent_w));
		// 左面 16,17,18,19
		vertexs.push_back(Vector3(-0.5f, -0.5f, 0.5f));
		vertexs.push_back(Vector3(-0.5f, 0.5f, 0.5f));
		vertexs.push_back(Vector3(-0.5f, 0.5f, -0.5f));
		vertexs.push_back(Vector3(-0.5f, -0.5f, -0.5f));
		normals.push_back(-Vector3::right);
		normals.push_back(-Vector3::right);
		normals.push_back(-Vector3::right);
		normals.push_back(-Vector3::right);
		tangents.push_back(Vector4(0, 0, -1, tangent_w));
		tangents.push_back(Vector4(0, 0, -1, tangent_w));
		tangents.push_back(Vector4(0, 0, -1, tangent_w));
		tangents.push_back(Vector4(0, 0, -1, tangent_w));
		// 右面 20,21,22,23
		vertexs.push_back(Vector3(0.5f, -0.5f, -0.5f));
		vertexs.push_back(Vector3(0.5f, 0.5f, -0.5f));
		vertexs.push_back(Vector3(0.5f, 0.5f, 0.5f));
		vertexs.push_back(Vector3(0.5f, -0.5f, 0.5f));
		normals.push_back(Vector3::right);
		normals.push_back(Vector3::right);
		normals.push_back(Vector3::right);
		normals.push_back(Vector3::right);
		tangents.push_back(Vector4(0, 0, 1, tangent_w));
		tangents.push_back(Vector4(0, 0, 1, tangent_w));
		tangents.push_back(Vector4(0, 0, 1, tangent_w));
		tangents.push_back(Vector4(0, 0, 1, tangent_w));

		// 屏幕里面
		texcoords.push_back(Vector2(0, 0));
		texcoords.push_back(Vector2(1, 0));
		texcoords.push_back(Vector2(0, 1));
		texcoords.push_back(Vector2(1, 1));
		// 屏幕外面
		texcoords.push_back(Vector2(0, 1));
		texcoords.push_back(Vector2(1, 1));
		texcoords.push_back(Vector2(0, 1));
		texcoords.push_back(Vector2(1, 1));
		// 上面
		texcoords.push_back(Vector2(0, 0));
		texcoords.push_back(Vector2(1, 0));
		texcoords.push_back(Vector2(0, 0));
		texcoords.push_back(Vector2(1, 0));
		// 下面
		texcoords.push_back(Vector2(0, 0));
		texcoords.push_back(Vector2(0, 1));
		texcoords.push_back(Vector2(1, 1));
		texcoords.push_back(Vector2(1, 0));
		// 左面
		texcoords.push_back(Vector2(0, 0));
		texcoords.push_back(Vector2(0, 1));
		texcoords.push_back(Vector2(1, 1));
		texcoords.push_back(Vector2(1, 0));
		// 右面
		texcoords.push_back(Vector2(0, 0));
		texcoords.push_back(Vector2(0, 1));
		texcoords.push_back(Vector2(1, 1));
		texcoords.push_back(Vector2(1, 0));

		SetVertex(vertexs);
		SetVertexNormal(normals);
		SetVertexTangent(tangents);
		AddTexcoord(texcoords);
	}

	//索引信息
	{
		ushort index = 0;
		ushort* indexs = new ushort[36];

		/*front*/
		// 屏幕里面
		indexs[index++] = 0;
		indexs[index++] = 2;
		indexs[index++] = 3;

		indexs[index++] = 0;
		indexs[index++] = 3;
		indexs[index++] = 1;

		// 上面
		indexs[index++] = 8;
		indexs[index++] = 4;
		indexs[index++] = 5;

		indexs[index++] = 8;
		indexs[index++] = 5;
		indexs[index++] = 9;

		// 屏幕外面
		indexs[index++] = 10;
		indexs[index++] = 6;
		indexs[index++] = 7;

		indexs[index++] = 10;
		indexs[index++] = 7;
		indexs[index++] = 11;

		// 下面
		indexs[index++] = 12;
		indexs[index++] = 13;
		indexs[index++] = 14;

		indexs[index++] = 12;
		indexs[index++] = 14;
		indexs[index++] = 15;

		// 左面
		indexs[index++] = 16;
		indexs[index++] = 17;
		indexs[index++] = 18;

		indexs[index++] = 16;
		indexs[index++] = 18;
		indexs[index++] = 19;

		// 右面
		indexs[index++] = 20;
		indexs[index++] = 21;
		indexs[index++] = 22;

		indexs[index++] = 20;
		indexs[index++] = 22;
		indexs[index++] = 23;

		SetIndex(36, indexs);
		SAFE_DELETE_ARRAY(indexs);
	}
}
END_NAMESPACE_DC