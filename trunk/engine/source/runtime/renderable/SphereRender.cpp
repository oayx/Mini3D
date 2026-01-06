#include "SphereRender.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(SphereRender, Renderer);
void SphereRender::Awake() 
{
	base::Awake();
	this->SetSphere(10, 10);
}
void SphereRender::SetSphere(ushort segments, ushort rings)
{
	this->ClearData();

	int buffSize = (rings + 1) * (segments + 1);
	Vector3v vertexs; Vector3v normals; Vector2v texcoords;
	vertexs.Reserve(buffSize); normals.Reserve(buffSize); texcoords.Reserve(buffSize);

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
				float x0 = r0 * sinf(nCurrentSegment *rDeltaSegAngle);
				float z0 = r0 * cosf(nCurrentSegment *rDeltaSegAngle);

				vec3.x = x0;
				vec3.y = y0;
				vec3.z = z0;
				vertexs.Add(vec3);

				vec3.x = x0;
				vec3.y = y0;
				vec3.z = z0;
				vec3.Normalize();
				normals.Add(vec3);

				vec2.x = 1.0f - ((float)nCurrentSegment / (float)segments);
				vec2.y = (float)nCurrentRing / (float)rings;
				texcoords.Add(vec2);
			}
		}
		this->SetVertexs(std::move(vertexs));
		this->SetColor(Color::White);
		this->SetNormals(std::move(normals));
		this->SetTexcoords1(std::move(texcoords));
	}

	//索引信息
	{
		uint* indexs = Memory::NewArray<uint>(segments * rings * 6);
		int iPerEdgeVerCount = segments + 1;
		int q = 0; int index = 0;
		for (ushort j = 0; j < segments; j += 1)
		{
			int p = 0;
			for (ushort i = 0; i < rings; i += 1)
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
		this->SetIndices(segments * rings * 6, indexs);
		Memory::DeleteArray(indexs);
	}

	this->UploadData();
}
void SphereRender::Update()
{
	base::Update();
	if (_isDirty)
	{
		_isDirty = false;
		this->UploadData();
	}
}
void SphereRender::SetColor(const Color& color)
{
	base::SetColor(color);
	_isDirty = true;
}
DC_END_NAMESPACE