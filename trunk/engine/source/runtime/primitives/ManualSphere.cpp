#include "ManualSphere.h"

BEGING_NAMESPACE_DC
/********************************************************************/
ManualSphere::ManualSphere(const String &name, ushort segments, ushort rings)
: base(OT_TRIANGLE_LIST, name)
{
	m_bUseIndexData = true;

	int buff_size = (rings + 1) * (segments + 1);
	VecVector3 vertexs; VecVector3 normals; VecVector2 texcoords;
	vertexs.reserve(buff_size); normals.reserve(buff_size); texcoords.reserve(buff_size);

	//顶点信息
	{
		Vector3 vec3; Vector2 vec2;
		float rDeltaRingAngle = (Math::PI / rings);
		float rDeltaSegAngle = (2.0f*Math::PI / segments);
		for (int nCurrentRing = 0; nCurrentRing < rings + 1; nCurrentRing++)
		{
			float r0 = sinf(nCurrentRing*rDeltaRingAngle);
			float y0 = cosf(nCurrentRing*rDeltaRingAngle);
			for (int nCurrentSegment = 0; nCurrentSegment < segments + 1; nCurrentSegment++)
			{
				float x0 = r0*sinf(nCurrentSegment *rDeltaSegAngle);
				float z0 = r0*cosf(nCurrentSegment *rDeltaSegAngle);

				vec3.x = x0;
				vec3.y = y0;
				vec3.z = z0;
				vertexs.push_back(vec3);

				vec3.x = x0;
				vec3.y = y0;
				vec3.z = z0;
				vec3.Normalize();
				normals.push_back(vec3);

				vec2.x = 1.0f - ((float)nCurrentSegment / (float)segments);
				vec2.y = (float)nCurrentRing / (float)rings;
				texcoords.push_back(vec2);
			}
		}
		SetVertex(vertexs);
		SetVertexNormal(normals);
		AddTexcoord(texcoords);
	}

	//索引信息
	{
		ushort* indexs = new ushort[segments * rings * 6];
		int iPerEdgeVerCount = segments + 1;
		int q = 0; int index = 0;
		for (ushort j = 0; j < segments; j += 1)
		{
			int p = 0;
			for (ushort i = 0; i < rings; i += 1)
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
		SetIndex(segments * rings * 6, indexs);
		SAFE_DELETE_ARRAY(indexs);
	}
}
END_NAMESPACE_DC