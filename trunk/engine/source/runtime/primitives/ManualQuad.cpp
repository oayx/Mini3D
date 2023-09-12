#include "ManualQuad.h"

BEGING_NAMESPACE_DC
/********************************************************************/
ManualQuad::ManualQuad(const String &name)
: base(OT_TRIANGLE_LIST, name)
{
	m_bUseIndexData = true;

	//����Ļƽ�У����ߴ�ֱ��Ļ����
	VecVector3 vertexs = { Vector3(-0.5f, 0.5f, 0), Vector3(0.5f, 0.5f, 0), Vector3(0.5f, -0.5f, 0), Vector3(-0.5f, -0.5f, 0) };
	SetVertex(vertexs);

	VecVector3 normals = { Vector3(0, 0, -1), Vector3(0, 0, -1), Vector3(0, 0, -1), Vector3(0, 0, -1) };
	SetVertexNormal(normals);

	//d3d�������������µģ�����v����ĸ����߷���
	float tangent_w = 1;
#if defined(USE_DIRECT3D)
	tangent_w = -1;
#endif
	VecVector4 tangents = { Vector4(1, 0, 0, tangent_w), Vector4(1, 0, 0, tangent_w), Vector4(1, 0, 0, tangent_w), Vector4(1, 0, 0, tangent_w) };
	SetVertexTangent(tangents);

	VecVector2 texcoords = { Vector2(0, 0), Vector2(1, 0), Vector2(1, 1), Vector2(0, 1) };
	AddTexcoord(texcoords);

	ushort fI1[] = { 0, 1, 2, 0, 2, 3 };
	SetIndex(6, fI1);
}
END_NAMESPACE_DC