
/*****************************************************************************
* Author： hannibal
* Date：2021/10/3
* Description： 自定义渲染
*****************************************************************************/
#pragma once

#include "Renderer.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL ManualRender : public Renderer
{
	friend class GameObject;
	FRIEND_CONSTRUCT_DESTRUCT(ManualRender);
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(ManualRender);
	DECLARE_OBJECT_CLONE;
	DECLARE_OBJECT_SERIALIZE(ManualRender);
	BEGIN_DERIVED_REFECTION_TYPE(ManualRender, Renderer)
		CTORS(DEFAULT_CTOR(ManualRender))
	END_DERIVED_REFECTION_TYPE;

protected:
	~ManualRender();
	virtual bool CanRemove()override;
	virtual void OnDrawEditor()override;

public:
	virtual bool UploadData()override { return base::UploadData(); }
	virtual void ClearData()override { base::ClearData(); }

public:
	void AddVertex(const Vector3& pos, int prim = 0) { base::AddVertex(pos, prim); }
	void AddColor(const Color& color, int prim = 0) { base::AddColor(color, prim); }
	void AddNormal(const Vector3& normal, int prim = 0) { base::AddNormal(normal, prim); }
	void AddTangent(const Vector3& tangent, int prim = 0) { base::AddTangent(tangent, prim); }
	void AddTexcoord1(const Vector2& texcoord, int prim = 0) { base::AddTexcoord1(texcoord, prim); }
	void AddTexcoord2(const Vector2& texcoord, int prim = 0) { base::AddTexcoord2(texcoord, prim); }

	void SetVertexs(const Vector3v&& vertexs, int prim = 0) { base::SetVertexs(std::move(vertexs), prim); }
	void SetColor(const Color& color, int prim = 0) { base::SetColor(color, prim); }
	void SetColors(const Vector<Color>&& colors, int prim = 0) { base::SetColors(std::move(colors), prim); }
	void SetNormals(const Vector3v&& normals, int prim = 0) { base::SetNormals(std::move(normals), prim); }
	void SetTangents(const Vector3v&& tangents, int prim = 0) { base::SetTangents(std::move(tangents), prim); }
	void SetTexcoords1(const Vector2v&& texcoords, int prim = 0) { base::SetTexcoords1(std::move(texcoords), prim); }
	void SetTexcoords2(const Vector2v&& texcoords, int prim = 0) { base::SetTexcoords2(std::move(texcoords), prim); }

	void AddIndex(uint index, int prim = 0) { base::AddIndex(index, prim); }
	void SetIndices(const Vector<uint>&& indices, int prim = 0) { base::SetIndices(std::move(indices), prim); }
	void SetIndices(uint index_nums, uint *index, int prim = 0) { base::SetIndices(index_nums, index, prim); }

	void AddSubMesh(uint elem_count, uint vtx_offset, uint idx_offset, uint start_vertex, uint start_index, int prim = 0) 
	{ 
		base::AddSubPrimitive(elem_count, vtx_offset, idx_offset, start_vertex, start_index, prim);
	}
};
DC_END_NAMESPACE
