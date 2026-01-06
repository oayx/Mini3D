
/*****************************************************************************
* Author： hannibal
* Date：2009/12/8
* Description： 可渲染对象 
*****************************************************************************/
#pragma once

#include "runtime/component/GameObject.h"
#include "runtime/graphics/RenderDefine.h"
#include "runtime/graphics/null/HardwareBuffer.h"
#include "runtime/mesh/MeshUtils.h"
#include "runtime/Application.h"

DC_BEGIN_NAMESPACE
class Texture;
class Material;
class Renderer;
class HardwareVertexBuffer;
class HardwareIndexBuffer;
/********************************************************************/
//最小渲染单元
struct SubPrimitive
{
	int ElemCount = 0;
	int VtxOffset = 0;
	int IdxOffset = 0;
	int StartVertex = 0;
	int StartIndex = 0;

	int		TexIndex = 0;
	String	TexName = "";
	Texture*Tex = nullptr;
	bool	AlphaEnable = false;

	iRect ScissorRect = iRect(0, 0, 800, 800);

	int GetTringles(PrimitiveType type)const
	{
		int primCount = 0;
		switch (type)
		{
		case PrimitiveType::PointList:
			primCount = ElemCount;
			break;
		case PrimitiveType::LineList:
			primCount = ElemCount / 2;
			break;
		case PrimitiveType::LineStrip:
			primCount = ElemCount - 1;
			break;
		case PrimitiveType::TriangleList:
			primCount = ElemCount / 3;
			break;
		case PrimitiveType::TriangleStrip:
			primCount = ElemCount - 2;
			break;
		case PrimitiveType::TriangleFan:
			primCount = ElemCount - 2;
			break;
		}
		return primCount;
	}
};
/********************************************************************/
//单个可渲染图元
class Primitive : public Object
{
	typedef Vector<SubPrimitive> SubPrimitives;
	DISALLOW_COPY_ASSIGN(Primitive);
	BEGIN_DERIVED_REFECTION_TYPE(Primitive, Object)
	END_REFECTION_TYPE;

public:
	static Primitive* Create(Renderer* renderer, PrimitiveType type, VertexLayout layout = VertexLayout::Variable);
	Primitive(Renderer* renderer, PrimitiveType type);
	~Primitive();

public:
	virtual bool		UploadData(const PrimitiveUploadDesc& desc);
	virtual void		ClearData();

	virtual int			GetVertexCount() { return 0; }
			int			GetIndexCount() { return _indexes.Size(); }
	virtual bool		IsEmpty()const { return true; }

public:
	void				AddIndex(uint index) { _indexes.Add(index); }
	void				AddIndexRange(const Vector<uint>& indices);
	void				AddIndexRange(const uint* indices, int size);
	void				SetIndices(const Vector<uint>&& indices) { _indexes = std::move(indices); }
	void				SetIndices(uint index_nums, uint *index);

	SubPrimitive&		AddSubPrimitive(uint elem_count, uint vtx_offset, uint idxOffset, uint start_vertex, uint start_index);
	void				ClearSubPrimitive() { _subPrimitives.Clear(); }
	void				SetPrimitiveType(PrimitiveType type) { _primitiveType = type; }
	PrimitiveType		GetPrimitiveType()const { return _primitiveType; }
	int					GetSubPrimitiveCount()const { return _subPrimitives.Size(); }
	SubPrimitive&		GetSubPrimitive(int index) { AssertEx(index >= 0 && index < _subPrimitives.Size(), "index out range"); return _subPrimitives[index]; }

	void				SetMaterial(Material* material);
	Material*			GetMaterial()const { return _material; }

	const Aabb&			GetBoundingBox()const { return _boundingBox; }
	void				SetLocalMatrix(const Matrix4& mat) { _localMatrix = mat; }
	Matrix4				GetWorldMatrix()const;

	float				GetPointSize()const { return _pointSize; }
	void				SetPointSize(float size) { _pointSize = size; }
	float				GetLineWidth()const { return _lineWidth; }
	void				SetLineWidth(float width) { _lineWidth = width; }

	Vector<uint>&		GetIndexBuffer() { return _indexes; }
	HardwareVertexBuffer*GetVertexData()const { return _vertexData; }
	HardwareIndexBuffer*GetIndexData()const { return _indexData; }

private:
	void				BuildIndexBuffer(const PrimitiveUploadDesc& desc);
	uint				GetTringles()const;//取得构造的三角形数量

protected:
	Renderer*			_renderer = nullptr;
	PrimitiveType		_primitiveType = PrimitiveType::TriangleList;
	SubPrimitives		_subPrimitives;
	Aabb				_boundingBox;
	Matrix4				_localMatrix = Matrix4::identity;

	HardwareVertexBuffer*_vertexData = nullptr;
	HardwareIndexBuffer*_indexData = nullptr;
	Material*			_material = nullptr;

	float				_pointSize = 0.0f;		//点大小
	float				_lineWidth = 1.0f;		//线粗细
	
	Vector<uint>		_indexes;				//索引
};
/********************************************************************/
//顶点不定渲染图元
class VariablePrimitive : public Primitive
{
	DISALLOW_COPY_ASSIGN(VariablePrimitive);
	BEGIN_DERIVED_REFECTION_TYPE(VariablePrimitive, Primitive)
	END_REFECTION_TYPE;

public:
	VariablePrimitive(Renderer* renderer, PrimitiveType type);
	~VariablePrimitive();

public:
	virtual bool		UploadData(const PrimitiveUploadDesc& desc)override;
	virtual void		ClearData()override;

	virtual int			GetVertexCount()override { return _vertexs.Size(); }
	virtual bool		IsEmpty()const override { return _vertexs.IsEmpty(); }

public:
	void				AddVertex(const Vector3& pos) { _vertexs.Add(pos); }
	void 				AddColor(const Color& color) { _colors.Add(color); }
	void 				AddNormal(const Vector3& normal) { _normals.Add(normal); }
	void 				AddTangent(const Vector3& tangent)  { _tangents.Add(tangent); }
	void 				AddTexcoord1(const Vector2& texcoord) { _texCoords1.Add(texcoord); }
	void 				AddTexcoord2(const Vector2& texcoord) { _texCoords2.Add(texcoord); }
	void 				AddWeight(const VertexBoneData& weight) { _weights.Add(weight); }

	void 				AddVertexRange(const Vector3v& vertexs);
	void 				AddColorRange(const Vector<Color>& colors);
	void 				AddNormasRange(const Vector3v& normals);
	void 				AddTangentRange(const Vector3v& tangents);
	void 				AddTexcoord1Range(const Vector2v& texcoords);
	void 				AddTexcoord2Range(const Vector2v& texcoords);
	void				AddWeightRange(const Vector<VertexBoneData>& weights);

	void				AddVertexRange(const Vector3* vertexs, int size);
	void				AddColorRange(const Color* colors, int size);
	void				AddNormasRange(const Vector3* normals, int size);
	void				AddTangentRange(const Vector3v* tangents, int size);
	void				AddTexcoord1Range(const Vector2* texcoords, int size);
	void				AddTexcoord2Range(const Vector2* texcoords, int size);
	void				AddWeightRange(const VertexBoneData* weights, int size);

	void				SetVertexs(const Vector3v&& vertexs) { _vertexs = std::move(vertexs); }
	void				SetColor(const Color& color);
	void				SetColors(const Vector<Color>&& colors) { _colors = std::move(colors); }
	void				SetNormals(const Vector3v&& normals) { _normals = std::move(normals); }
	void				SetTangents(const Vector3v&& tangents) { _tangents = std::move(tangents); }
	void				SetTexcoords1(const Vector2v&& texcoords) { _texCoords1 = std::move(texcoords); }
	void				SetTexcoords2(const Vector2v&& texcoords) { _texCoords2 = std::move(texcoords); }
	void				SetWeight2(const Vector<VertexBoneData>&& weights) { _weights = std::move(weights); }

private:
	void				AddVertexDecl(VertexSemantic input_semantic);
	void				BuildVertexBuffer(VertexSemantic input_semantic, const PrimitiveUploadDesc& desc);

protected:
	Vector3v			_vertexs;
	Vector<Color>		_colors;
	Vector3v			_normals;
	Vector3v			_tangents;
	Vector2v			_texCoords1;			//多层纹理坐标，只支持2层
	Vector2v			_texCoords2;
	Vector<VertexBoneData> _weights;			//骨骼权重
};

/********************************************************************/
//固定布局
template<class T>
class FixedPrimitive : public Primitive
{
	typedef Primitive base;
	DISALLOW_COPY_ASSIGN(FixedPrimitive);

public:
	FixedPrimitive(Renderer* renderer, PrimitiveType type, VertexLayout layout_type) : base(renderer, type), _layoutType(layout_type) {}
	~FixedPrimitive() {}

public:
	virtual bool		UploadData(const PrimitiveUploadDesc& desc)override;
	virtual void		ClearData()override;

	virtual int			GetVertexCount()override { return _vertexSize; }
	virtual bool		IsEmpty()const override { return _vertexSize == 0; }

public:
	void				AddVertex(const T& vertex);
	void 				AddVertexRange(const Vector<T>& vertexs);
	void				AddVertexRange(const T* vertexs, int size);

	void				SetVertexSize(int size);
	Vector<T>&			GetVertexBuffer() { return _vertexs; }

private:
	void				AddVertexDecl();
	void				BuildVertexBuffer(const PrimitiveUploadDesc& desc);

protected:
	VertexLayout		_layoutType = VertexLayout::VertexColor;
	int					_vertexSize = 0;
	Vector<T>			_vertexs;
};
template<class T>
bool FixedPrimitive<T>::UploadData(const PrimitiveUploadDesc& desc)
{
	DC_PROFILE_FUNCTION;
	if (IsEmpty())
	{
		return false;
	}
	if (_vertexData == nullptr)
	{
		_vertexData = Application::GetGraphics()->CreateVertexBuffer();
	}
	this->AddVertexDecl();
	this->BuildVertexBuffer(desc);
	base::UploadData(desc);
	return true;
}
template<class T>
void FixedPrimitive<T>::ClearData()
{
	_vertexSize = 0;
	base::ClearData();
}
template<class T>
void FixedPrimitive<T>::AddVertex(const T& vertex)
{
	if (_vertexs.Size() <= _vertexSize)
	{
		_vertexs.Resize(Math::Max(_vertexs.Size(), 1) * 2);
	}
	_vertexs[_vertexSize] = vertex;
	_vertexSize++;
}
template<class T>
void FixedPrimitive<T>::AddVertexRange(const Vector<T>& vertexs)
{
	if (_vertexs.Size() < _vertexSize + vertexs.Size())
	{
		_vertexs.Resize(_vertexs.Size() + vertexs.Size());
	}
	Memory::Copy(&_vertexs[_vertexSize], vertexs.Data(), vertexs.Size() * sizeof(T));
	_vertexSize += vertexs.Size();
}
template<class T>
void FixedPrimitive<T>::AddVertexRange(const T* vertexs, int size)
{
	if (_vertexs.Size() < _vertexSize + size)
	{
		_vertexs.Resize(_vertexs.Size() + size);
	}
	Memory::Copy(&_vertexs[_vertexSize], vertexs, size * sizeof(T));
	_vertexSize += size;
}
template<class T>
void FixedPrimitive<T>::SetVertexSize(int size)
{
	if (_vertexs.Size() < size)
	{
		_vertexs.Resize(size);
	}
	_vertexSize = size;
}
template<class T>
void FixedPrimitive<T>::AddVertexDecl()
{
	DC_PROFILE_FUNCTION;
	_vertexData->ClearElement();
	{
		int offset = 0;
		switch (_layoutType)
		{
		case VertexLayout::VertexColor:
			_vertexData->AddElement(0, VertexSize::Float3, VertexSemantic::Position, offset, 0);
			offset += GetSizeByVertex(VertexSize::Float3);

			_vertexData->AddElement(0, VertexSize::Color, VertexSemantic::Diffuse, offset, 0);
			offset += GetSizeByVertex(VertexSize::Color);
			break;
		case VertexLayout::VertexColorTexcoord:
			_vertexData->AddElement(0, VertexSize::Float3, VertexSemantic::Position, offset, 0);
			offset += GetSizeByVertex(VertexSize::Float3);

			_vertexData->AddElement(0, VertexSize::Color, VertexSemantic::Diffuse, offset, 0);
			offset += GetSizeByVertex(VertexSize::Color);

			_vertexData->AddElement(0, VertexSize::Float2, VertexSemantic::Coordinates1, offset, 0);
			offset += GetSizeByVertex(VertexSize::Float2);
			break;
		default:break;
		}
	}
}
template<class T>
void FixedPrimitive<T>::BuildVertexBuffer(const PrimitiveUploadDesc& desc)
{
	DC_PROFILE_FUNCTION;
	//包围盒
	//TODO
	Vector3 min = -Vector3::one, max = Vector3::one;

	//填充顶点缓冲区
	VertexBufferDesc vbd(0, (uint)_vertexSize, desc.usage);
	float *buf = (float*)_vertexData->Lock(vbd);
	if (buf)
	{
		Memory::Copy(buf, (float*)_vertexs.Data(), _vertexSize * sizeof(T));
	}
	_vertexData->Unlock(vbd);

	_boundingBox = Aabb(min, max);
}
DC_END_NAMESPACE