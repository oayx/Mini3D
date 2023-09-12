#include "ManualCube.h"

BEGING_NAMESPACE_DC
/********************************************************************/
ManualCube::ManualCube(const String &name)
: base(OT_TRIANGLE_LIST, name)
{
	m_bUseIndexData = true;

	//������Ϣ
	{
		// ����ֻ��Ҫ8������Ϳ����ˡ�
		// ��������6���ı�������������꣬���ܹ��ö������������
		// ������Ҫ8x3�����㣨ͬ���Ķ��㱻3���ı����湲��)
		VecVector3 vertexs; VecVector3 normals; VecVector4 tangents; VecVector2 texcoords;
		vertexs.reserve(24); normals.reserve(24); tangents.reserve(24);  texcoords.reserve(24);

		//d3d�������������µģ�����v����ĸ����߷���
		float tangent_w = 1;
#if defined(USE_DIRECT3D)
		tangent_w = -1;
#endif
		// ��Ļ���� 0,1,2,3
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
		// ��Ļ���� 4,5,6,7
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
		// ���� 8,9,10,11
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
		// ���� 12,13,14,15
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
		// ���� 16,17,18,19
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
		// ���� 20,21,22,23
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

		// ��Ļ����
		texcoords.push_back(Vector2(0, 0));
		texcoords.push_back(Vector2(1, 0));
		texcoords.push_back(Vector2(0, 1));
		texcoords.push_back(Vector2(1, 1));
		// ��Ļ����
		texcoords.push_back(Vector2(0, 1));
		texcoords.push_back(Vector2(1, 1));
		texcoords.push_back(Vector2(0, 1));
		texcoords.push_back(Vector2(1, 1));
		// ����
		texcoords.push_back(Vector2(0, 0));
		texcoords.push_back(Vector2(1, 0));
		texcoords.push_back(Vector2(0, 0));
		texcoords.push_back(Vector2(1, 0));
		// ����
		texcoords.push_back(Vector2(0, 0));
		texcoords.push_back(Vector2(0, 1));
		texcoords.push_back(Vector2(1, 1));
		texcoords.push_back(Vector2(1, 0));
		// ����
		texcoords.push_back(Vector2(0, 0));
		texcoords.push_back(Vector2(0, 1));
		texcoords.push_back(Vector2(1, 1));
		texcoords.push_back(Vector2(1, 0));
		// ����
		texcoords.push_back(Vector2(0, 0));
		texcoords.push_back(Vector2(0, 1));
		texcoords.push_back(Vector2(1, 1));
		texcoords.push_back(Vector2(1, 0));

		SetVertex(vertexs);
		SetVertexNormal(normals);
		SetVertexTangent(tangents);
		AddTexcoord(texcoords);
	}

	//������Ϣ
	{
		ushort index = 0;
		ushort* indexs = new ushort[36];

		/*front*/
		// ��Ļ����
		indexs[index++] = 0;
		indexs[index++] = 2;
		indexs[index++] = 3;

		indexs[index++] = 0;
		indexs[index++] = 3;
		indexs[index++] = 1;

		// ����
		indexs[index++] = 8;
		indexs[index++] = 4;
		indexs[index++] = 5;

		indexs[index++] = 8;
		indexs[index++] = 5;
		indexs[index++] = 9;

		// ��Ļ����
		indexs[index++] = 10;
		indexs[index++] = 6;
		indexs[index++] = 7;

		indexs[index++] = 10;
		indexs[index++] = 7;
		indexs[index++] = 11;

		// ����
		indexs[index++] = 12;
		indexs[index++] = 13;
		indexs[index++] = 14;

		indexs[index++] = 12;
		indexs[index++] = 14;
		indexs[index++] = 15;

		// ����
		indexs[index++] = 16;
		indexs[index++] = 17;
		indexs[index++] = 18;

		indexs[index++] = 16;
		indexs[index++] = 18;
		indexs[index++] = 19;

		// ����
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