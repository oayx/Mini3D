
/*****************************************************************************
* Author： hannibal
* Date：2009/12/8
* Description： 可渲染对象 
*****************************************************************************/
#pragma once

#include "Primitive.h"
#include "runtime/component/GameObject.h"
#include "runtime/graphics/RenderDefine.h"
#include "runtime/mesh/MeshUtils.h"

DC_BEGIN_NAMESPACE
class CGProgram;
class Material;
/********************************************************************/
// 可渲染对象基类
class ENGINE_DLL Renderer : public Component
{
	friend class GameObject;
	typedef Vector<Primitive*> Primitives;
	typedef Vector<Material*> Materials;
	DECLARE_OBJECT_CLONE;
	DECLARE_OBJECT_SERIALIZE(Renderer);
	DISALLOW_COPY_ASSIGN(Renderer);
	FRIEND_CONSTRUCT_DESTRUCT(Renderer);
	BEGIN_DERIVED_REFECTION_TYPE(Renderer, Component)
	END_REFECTION_TYPE;

protected:
	Renderer();
	virtual ~Renderer();

	virtual void	OnDrawEditor()override;
	void			OnDrawMaterial();
	void			OnDrawShader();
	void			OnDrawLight();
	void			OnDrawOrder();

public:
	virtual const	Matrix4& GetWorldMatrix();//获得变换矩阵

	Material*		SetMaterial(const String& file);//内部会复制一份新材质，然后返回
	Material*		SetMaterial(const String& file, int index, bool rebuild = true);//内部会复制一份新材质，然后返回
	int				GetMaterialCount()const { return _materials.Size(); }
	Material*		GetMaterial(int index = 0)const;
	const Materials&GetMaterials()const { return _materials; }
	CGProgram*		GetProgram(int index = 0)const;

	PrimitiveType	GetPrimitiveType()const { return _primitiveType; }
	void			SetPrimitiveType(PrimitiveType type);

	int				GetPrimitiveCount()const { return _primitives.Size(); }
	Primitive*		GetPrimitive(int index = 0);

	void			CalLightes();//计算受影响的光照

public://排序
	int				GetSortLayer()const { return _sortLayer; }
	void			SetSortLayer(int layer) { _sortLayer = layer; }
	int				GetOrderInLayer()const { return _orderInLayer; }
	void			SetOrderInLayer(int order) { _orderInLayer = order; }
	int				GetRenderQueue(int index = 0)const;
	void			SetRenderQueue(int queue, int index = 0);

	void			AddInstanceTransform(const Matrix4& mat) { _instanceTransform.Add(mat); }
	void			AddInstanceTransformRange(const Vector<Matrix4>& mats);
	void			AddInstanceTransformRange(const Matrix4* mats, int size);
	void			SetInstanceTransform(const Vector<Matrix4>& mats) { _instanceTransform = mats; }
	void			ClearInstanceTransform() { _instanceTransform.Clear(); }
	const			Vector<Matrix4>& GetInstanceTransform()const { return _instanceTransform; }

protected:
	virtual bool	UploadData();
	virtual bool	UploadData(const PrimitiveUploadDesc& desc);
	virtual void	ClearData();

	void			AddVertex(const Vector3& pos, int prim = 0);
	void 			AddColor(const Color& color, int prim = 0);
	void 			AddNormal(const Vector3& normal, int prim = 0);
	void 			AddTangent(const Vector3& tangent, int prim = 0);
	void 			AddTexcoord1(const Vector2& texcoor, int prim = 0);
	void 			AddTexcoord2(const Vector2& texcoord, int prim = 0);

	void 			SetVertexs(const Vector3v&& vertexs, int prim = 0);
	void 			SetColor(const Color& color, int prim = 0);
	void 			SetColors(const Vector<Color>&& colors, int prim = 0);
	void 			SetNormals(const Vector3v&& normals, int prim = 0);
	void 			SetTangents(const Vector3v&& tangents, int prim = 0);
	void 			SetTexcoords1(const Vector2v&& texcoords, int prim = 0);
	void 			SetTexcoords2(const Vector2v&& texcoords, int prim = 0);

	void			SetPointSize(float size, int prim = 0);

	void 			AddIndex(uint index, int prim = 0);
	void 			SetIndices(const Vector<uint>&& indices, int prim = 0);
	void 			SetIndices(uint index_nums, uint *index, int prim = 0);

	int				AddSubPrimitive(uint elem_count, uint vtx_offset, uint idxOffset, uint start_vertex, uint start_index, int prim = 0);

protected:
	VertexLayout	_vertexLayout = VertexLayout::Variable;
	PrimitiveType	_primitiveType = PrimitiveType::TriangleList;
	Materials		_materials;
	Primitives		_primitives;

	Vector<Matrix4>	_instanceTransform;	//instance本地变换

	int				_sortLayer = 0;
	int				_orderInLayer = 0;
};
/********************************************************************/
// 虚拟可显示对象，不会实际渲染，特殊情况下使用
class ENGINE_DLL VirtualRenderer : public Renderer
{
	FRIEND_CONSTRUCT_DESTRUCT(VirtualRenderer);
	DISALLOW_COPY_ASSIGN(VirtualRenderer);
	BEGIN_DERIVED_REFECTION_TYPE(VirtualRenderer, Renderer)
	END_REFECTION_TYPE;

protected:
	VirtualRenderer() = default;
	~VirtualRenderer() = default;

	virtual bool ShowInEditor()override { return false; }
	virtual bool CanSerialize()override { return false; }
};
DC_END_NAMESPACE