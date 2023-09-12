#include "Primitive.h"
#include "Renderer.h" 
#include "runtime/component/GameObject.h"
#include "runtime/component/Component.inl"
#include "runtime/graphics/Material.h"
#include "runtime/graphics/null/CGProgram.h"
#include "runtime/Application.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(Primitive, Object);
Primitive* Primitive::Create(Renderer* renderer, PrimitiveType type, VertexLayout layout)
{
	Primitive* primitive = nullptr;
	switch (layout)
	{
	case VertexLayout::Variable:
		primitive = DBG_NEW VariablePrimitive(renderer, type);
		break;
	case VertexLayout::VertexColor:
		primitive = DBG_NEW FixedPrimitive<VertexColorLayout>(renderer, type, layout);
		break;
	case VertexLayout::VertexColorTexcoord:
		primitive = DBG_NEW FixedPrimitive<VertexColorUVLayout>(renderer, type, layout);
		break;
	default:
		AssertEx(false, "");
		break;
	}
	return primitive;
}
Primitive::Primitive(Renderer* renderer, PrimitiveType type)
	: _renderer(renderer), _primitiveType(type)
{
}
Primitive::~Primitive()
{
	SAFE_DELETE(_vertexData);
	SAFE_DELETE(_indexData);
	SAFE_RELEASE(_material);
}
bool Primitive::UploadData(const PrimitiveUploadDesc& desc)
{
	DC_PROFILE_FUNCTION();
	if (!_indexes.IsEmpty())
	{
		if (_indexData == nullptr)_indexData = Application::GetGraphics()->CreateIndexBuffer();
		this->BuildIndexBuffer(desc);
	}
	else
	{
		SAFE_DELETE(_indexData);
	}

	//添加默认图元
	if (_subPrimitives.IsEmpty())
	{
		if (!_indexes.IsEmpty())
		{
			SubPrimitive prim = { _indexes.Size(), 0, 0, 0, 0 };
			_subPrimitives.Add(prim);
		}
		else
		{
			SubPrimitive prim = { GetVertexCount(), 0, 0, 0, 0 };
			_subPrimitives.Add(prim);
		}
	}

	return true;
}
void Primitive::ClearData()
{
	DC_PROFILE_FUNCTION();
	_indexes.Clear();
	_subPrimitives.Clear();
	if (_vertexData)
	{
		_vertexData->Clear();
	}
	if (_indexData)
	{
		_indexData->Clear();
	}
}
void Primitive::AddIndexRange(const Vector<uint>& indices)
{
	int old_size = _indexes.Size();
	_indexes.Resize(_indexes.Size() + indices.Size());
	Memory::Copy(&_indexes[old_size], indices.Data(), indices.Size() * sizeof(uint));
}
void Primitive::AddIndexRange(const uint* indices, int size)
{
	int old_size = _indexes.Size();
	_indexes.Resize(_indexes.Size() + size);
	Memory::Copy(&_indexes[old_size], indices, size * sizeof(uint));
}
void Primitive::SetIndices(uint index_nums, uint *index)
{
	_indexes.Clear();
	_indexes.Resize(index_nums);
	Memory::Copy(&_indexes[0], index, index_nums * sizeof(uint));
}
SubPrimitive& Primitive::AddSubPrimitive(uint elem_count, uint vtx_offset, uint idx_offset, uint start_vertex, uint start_index)
{
	SubPrimitive sub_prim = { (int)elem_count, (int)vtx_offset, (int)idx_offset, (int)start_vertex, (int)start_index };
	_subPrimitives.Add(sub_prim);
	return _subPrimitives.Last();
}
void Primitive::SetMaterial(Material* material)
{
	SAFE_RELEASE(_material);
	_material = material;
	if (_material != nullptr)
	{
		_material->Retain();
	}
}
Matrix4 Primitive::GetWorldMatrix()const
{ 
	if (_renderer != nullptr)
		return _localMatrix * _renderer->GetWorldMatrix();
	else
		return _localMatrix;
}
void Primitive::BuildIndexBuffer(const PrimitiveUploadDesc& desc)
{
	DC_PROFILE_FUNCTION();
	if (_indexes.IsEmpty())return;

	IndexType index_type = IndexType::B32;
	IndexBufferDesc ibd(index_type, _indexes.Size(), desc.usage);
	void* buffer = _indexData->Lock(ibd);
	if (buffer)
	{
		if (index_type == IndexType::B32)
		{
			Memory::Copy(buffer, &_indexes[0], sizeof(_indexes[0])*_indexes.Size());
		}
		else
		{
			ushort* by_buffer = (ushort*)buffer;
			for (int i = 0; i < _indexes.Size(); ++i)
			{
				(*by_buffer) = (ushort)_indexes[i];
				by_buffer += 1;
			}
		}
	}
	_indexData->Unlock();
}
uint Primitive::GetTringles()const
{
	if (!_vertexData)
		return 0;

	uint primCount = 0;
	switch (_primitiveType)
	{
	case PrimitiveType::PointList:
		primCount = (uint)(!_indexes.IsEmpty() ? _indexData->GetIndexCount() : _vertexData->GetVertexCount());
		break;
	case PrimitiveType::LineList:
		primCount = (uint)(!_indexes.IsEmpty() ? _indexData->GetIndexCount() : _vertexData->GetVertexCount()) / 2;
		break;
	case PrimitiveType::LineStrip:
		primCount = (uint)(!_indexes.IsEmpty() ? _indexData->GetIndexCount() : _vertexData->GetVertexCount()) - 1;
		break;
	case PrimitiveType::TriangleList:
		primCount = (uint)(!_indexes.IsEmpty() ? _indexData->GetIndexCount() : _vertexData->GetVertexCount()) / 3;
		break;
	case PrimitiveType::TriangleStrip:
		primCount = (uint)(!_indexes.IsEmpty() ? _indexData->GetIndexCount() : _vertexData->GetVertexCount()) - 2;
		break;
	case PrimitiveType::TriangleFun:
		primCount = (uint)(!_indexes.IsEmpty() ? _indexData->GetIndexCount() : _vertexData->GetVertexCount()) - 2;
		break;
	}
	return primCount;
}
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(VariablePrimitive, Primitive);
VariablePrimitive::VariablePrimitive(Renderer* renderer, PrimitiveType type)
	: base(renderer, type)
{
}
VariablePrimitive::~VariablePrimitive()
{
}
bool VariablePrimitive::UploadData(const PrimitiveUploadDesc& desc)
{
	DC_PROFILE_FUNCTION();
	if (IsEmpty())
	{
		return false;
	}
	if (!_colors.IsEmpty() && _colors.Size() != _vertexs.Size())
	{
		Debuger::Error("UploadData color count error");
		return false;
	}
	if (!_normals.IsEmpty() && _normals.Size() != _vertexs.Size())
	{
		Debuger::Error("UploadData normal count error");
		return false;
	}
	if (!_tangents.IsEmpty() && _tangents.Size() != _vertexs.Size())
	{
		Debuger::Error("UploadData tanget count error");
		return false;
	}
	if (!_texCoords1.IsEmpty() && _texCoords1.Size() != _vertexs.Size())
	{
		Debuger::Error("UploadData texcoord1 count error");
		return false;
	}
	if (!_texCoords2.IsEmpty() && _texCoords2.Size() != _vertexs.Size())
	{
		Debuger::Error("UploadData texcoord2 count error");
		return false;
	}
	if (!_weights.IsEmpty() && _weights.Size() != _vertexs.Size())
	{
		Debuger::Error("UploadData bones count error");
		return false;
	}

	if (_vertexData == nullptr)
	{
		_vertexData = Application::GetGraphics()->CreateVertexBuffer();
	}
	VertexSemantic input_semantic = VertexSemantic::None;
	if (_material != nullptr && _material->GetProgram() != nullptr)
	{
		input_semantic = _material->GetProgram()->GetInputVertexSemantic();
	}
	else
	{
		input_semantic = VertexSemantic::None;
	}
	this->AddVertexDecl(input_semantic);
	this->BuildVertexBuffer(input_semantic, desc);

	base::UploadData(desc);
	return true;
}
void VariablePrimitive::ClearData()
{
	_vertexs.Clear();
	_colors.Clear();
	_normals.Clear();
	_tangents.Clear();
	_texCoords1.Clear();
	_texCoords2.Clear();
	base::ClearData();
}
void VariablePrimitive::AddVertexRange(const Vector3v& vertexs)
{
	int old_size = _vertexs.Size();
	_vertexs.Resize(_vertexs.Size() + vertexs.Size());
	Memory::Copy(&_vertexs[old_size], vertexs.Data(), vertexs.Size() * sizeof(Vector3));
}
void VariablePrimitive::AddColorRange(const Vector<Color>& colors)
{
	int old_size = _colors.Size();
	_colors.Resize(_colors.Size() + colors.Size());
	Memory::Copy(&_colors[old_size], colors.Data(), colors.Size() * sizeof(Color));
}
void VariablePrimitive::AddNormasRange(const Vector3v& normals)
{
	int old_size = _normals.Size();
	_normals.Resize(_normals.Size() + normals.Size());
	Memory::Copy(&_normals[old_size], normals.Data(), normals.Size() * sizeof(Vector3));
}
void VariablePrimitive::AddTangentRange(const Vector3v& tangents)
{
	int old_size = _tangents.Size();
	_tangents.Resize(_tangents.Size() + tangents.Size());
	Memory::Copy(&_tangents[old_size], tangents.Data(), tangents.Size() * sizeof(Vector3));
}
void VariablePrimitive::AddTexcoord1Range(const Vector2v& texcoords)
{
	int old_size = _texCoords1.Size();
	_texCoords1.Resize(_texCoords1.Size() + texcoords.Size());
	Memory::Copy(&_texCoords1[old_size], texcoords.Data(), texcoords.Size() * sizeof(Vector2));
}
void VariablePrimitive::AddTexcoord2Range(const Vector2v& texcoords)
{
	int old_size = _texCoords2.Size();
	_texCoords2.Resize(_texCoords2.Size() + texcoords.Size());
	Memory::Copy(&_texCoords2[old_size], texcoords.Data(), texcoords.Size() * sizeof(Vector2));
}
void VariablePrimitive::AddWeightRange(const Vector<VertexBoneData>& weights)
{
	int old_size = _weights.Size();
	_weights.Resize(_weights.Size() + weights.Size());
	Memory::Copy(&_weights[old_size], weights.Data(), weights.Size() * sizeof(VertexBoneData));
}
void VariablePrimitive::AddVertexRange(const Vector3* vertexs, int size)
{
	int old_size = _vertexs.Size();
	_vertexs.Resize(_vertexs.Size() + size);
	Memory::Copy(&_vertexs[old_size], vertexs, size * sizeof(Vector3));
}
void VariablePrimitive::AddColorRange(const Color* colors, int size)
{
	int old_size = _colors.Size();
	_colors.Resize(_colors.Size() + size);
	Memory::Copy(&_colors[old_size], colors, size * sizeof(Color));
}
void VariablePrimitive::AddNormasRange(const Vector3* normals, int size)
{
	int old_size = _normals.Size();
	_normals.Resize(_normals.Size() + size);
	Memory::Copy(&_normals[old_size], normals, size * sizeof(Vector3));
}
void VariablePrimitive::AddTangentRange(const Vector3v* tangents, int size)
{
	int old_size = _tangents.Size();
	_tangents.Resize(_tangents.Size() + size);
	Memory::Copy(&_tangents[old_size], tangents, size * sizeof(Vector3));
}
void VariablePrimitive::AddTexcoord1Range(const Vector2* texcoords, int size)
{
	int old_size = _texCoords1.Size();
	_texCoords1.Resize(_texCoords1.Size() + size);
	Memory::Copy(&_texCoords1[old_size], texcoords, size * sizeof(Vector2));
}
void VariablePrimitive::AddTexcoord2Range(const Vector2* texcoords, int size)
{
	int old_size = _texCoords2.Size();
	_texCoords2.Resize(_texCoords2.Size() + size);
	Memory::Copy(&_texCoords2[old_size], texcoords, size * sizeof(Vector2));
}
void VariablePrimitive::AddWeightRange(const VertexBoneData* weights, int size)
{
	int old_size = _weights.Size();
	_weights.Resize(_weights.Size() + size);
	Memory::Copy(&_weights[old_size], weights, size * sizeof(VertexBoneData));
}
void VariablePrimitive::SetColor(const Color& color)
{
	_colors.Clear();
	_colors.Resize(_vertexs.Size(), color);
}
void VariablePrimitive::AddVertexDecl(VertexSemantic input_semantic)
{
	DC_PROFILE_FUNCTION();
	_vertexData->ClearElement();
	{
		int offset = 0;
		if ((!_vertexs.IsEmpty()) || (input_semantic != VertexSemantic::None && (input_semantic & VertexSemantic::Position)))
		{
			_vertexData->AddElement(0, VertexSize::Float3, VertexSemantic::Position, offset, 0);
			offset += GetSizeByVertex(VertexSize::Float3);
		}
		if ((!_colors.IsEmpty()) || (input_semantic != VertexSemantic::None && (input_semantic & VertexSemantic::Diffuse)))
		{
			_vertexData->AddElement(0, VertexSize::Color, VertexSemantic::Diffuse, offset, 0);
			offset += GetSizeByVertex(VertexSize::Color);
		}

		if ((!_normals.IsEmpty()) || (input_semantic != VertexSemantic::None && (input_semantic & VertexSemantic::Normal)))
		{
			_vertexData->AddElement(0, VertexSize::Float3, VertexSemantic::Normal, offset, 0);
			offset += GetSizeByVertex(VertexSize::Float3);
		}

		if ((!_tangents.IsEmpty()) || (input_semantic != VertexSemantic::None && (input_semantic & VertexSemantic::Tangent)))
		{
			_vertexData->AddElement(0, VertexSize::Float4, VertexSemantic::Tangent, offset, 0);
			offset += GetSizeByVertex(VertexSize::Float4);
		}
		if ((_pointSize != 0.0f) || (input_semantic != VertexSemantic::None && (input_semantic & VertexSemantic::Size)))
		{
			_vertexData->AddElement(0, VertexSize::Float1, VertexSemantic::Size, offset, 0);
			offset += GetSizeByVertex(VertexSize::Float1);
		}

		if ((!_texCoords1.IsEmpty()) || (input_semantic != VertexSemantic::None && (input_semantic & VertexSemantic::Coordinates1)))
		{
			_vertexData->AddElement(0, VertexSize::Float2, VertexSemantic::Coordinates1, offset, 0);
			offset += GetSizeByVertex(VertexSize::Float2);
		}
		if ((!_texCoords2.IsEmpty()) || (input_semantic != VertexSemantic::None && (input_semantic & VertexSemantic::Coordinates2)))
		{
			_vertexData->AddElement(0, VertexSize::Float2, VertexSemantic::Coordinates2, offset, 1);
			offset += GetSizeByVertex(VertexSize::Float2);
		}

		if ((!_weights.IsEmpty()) || (input_semantic != VertexSemantic::None && (input_semantic & VertexSemantic::Indices)))
		{
			_vertexData->AddElement(0, GetVertexSizeByBone(), VertexSemantic::Indices, offset, 0);
			offset += GetSizeByVertex(GetVertexSizeByBone());
		}
		if ((!_weights.IsEmpty()) || (input_semantic != VertexSemantic::None && (input_semantic & VertexSemantic::Weights)))
		{
			_vertexData->AddElement(0, GetVertexSizeByBone(), VertexSemantic::Weights, offset, 0);
			offset += GetSizeByVertex(GetVertexSizeByBone());
		}
	}

	{
		int instance_offset = 0;
		if (_renderer)
		{
			if ((!_renderer->GetInstanceTransform().IsEmpty()) || (input_semantic != VertexSemantic::None && (input_semantic & VertexSemantic::InstanceOffset)))
			{
				_vertexData->AddElement(1, VertexSize::Float4, VertexSemantic::InstanceOffset, instance_offset, 0);
				instance_offset += GetSizeByVertex(VertexSize::Float4);
				_vertexData->AddElement(1, VertexSize::Float4, VertexSemantic::InstanceOffset, instance_offset, 1);
				instance_offset += GetSizeByVertex(VertexSize::Float4);
				_vertexData->AddElement(1, VertexSize::Float4, VertexSemantic::InstanceOffset, instance_offset, 2);
				instance_offset += GetSizeByVertex(VertexSize::Float4);
				_vertexData->AddElement(1, VertexSize::Float4, VertexSemantic::InstanceOffset, instance_offset, 3);
				instance_offset += GetSizeByVertex(VertexSize::Float4);
			}
		}
	}
}
void VariablePrimitive::BuildVertexBuffer(VertexSemantic input_semantic, const PrimitiveUploadDesc& desc)
{
	DC_PROFILE_FUNCTION();
	//包围盒
	Vector3 min = Vector3::zero, max = Vector3::zero;

	//顶点信息
	bool has_position = false, has_color = false, has_normal = false, has_tangent = false, has_tex1 = false, has_tex2 = false, has_indices = false, has_weights = false, has_size = false;
	if ((!_vertexs.IsEmpty()) || (input_semantic != VertexSemantic::None && (input_semantic & VertexSemantic::Position)))
		has_position = true;
	if ((!_colors.IsEmpty()) || (input_semantic != VertexSemantic::None && (input_semantic & VertexSemantic::Diffuse)))
		has_color = true;
	if ((!_normals.IsEmpty()) || (input_semantic != VertexSemantic::None && (input_semantic & VertexSemantic::Normal)))
		has_normal = true;
	if ((!_tangents.IsEmpty()) || (input_semantic != VertexSemantic::None && (input_semantic & VertexSemantic::Tangent)))
		has_tangent = true;
	if ((_pointSize != 0.0f) || (input_semantic != VertexSemantic::None && (input_semantic & VertexSemantic::Size)))
		has_size = true;
	if ((!_texCoords1.IsEmpty()) || (input_semantic != VertexSemantic::None && (input_semantic & VertexSemantic::Coordinates1)))
		has_tex1 = true;
	if ((!_texCoords2.IsEmpty()) || (input_semantic != VertexSemantic::None && (input_semantic & VertexSemantic::Coordinates2)))
		has_tex2 = true;
	if ((!_weights.IsEmpty()) || (input_semantic != VertexSemantic::None && (input_semantic & VertexSemantic::Indices)))
		has_indices = true;
	if ((!_weights.IsEmpty()) || (input_semantic != VertexSemantic::None && (input_semantic & VertexSemantic::Weights)))
		has_weights = true;


	//填充顶点缓冲区
	VertexBufferDesc vbd(0, (uint)_vertexs.Size(), desc.usage);
	float *buf = (float*)_vertexData->Lock(vbd);
	if (buf)
	{
		for (int i = 0; i < _vertexs.Size(); i++)
		{
			Vector3 position;
			if (has_position)
			{
				if (!_vertexs.IsEmpty())position = _vertexs[i];
				::memcpy(buf, position.p, sizeof(Vector3));
				buf += 3;
			}

			if (has_color)
			{
				uint color = 0xffffffff;
				if (!_colors.IsEmpty())color = _colors[i].ToVertexColor();
				*buf++ = DWToF(color);
			}

			if (has_normal)
			{
				if (!_normals.IsEmpty())::memcpy(buf, _normals[i].p, sizeof(Vector3));
				buf += 3;
			}

			if (has_tangent)
			{
				if (!_tangents.IsEmpty())
				{
					const Vector3& tangents = _tangents[i];
					*buf++ = tangents.x;
					*buf++ = tangents.y;
					*buf++ = tangents.z;
#if defined(DC_GRAPHICS_API_DX9) || defined(DC_GRAPHICS_API_DX11)
					*buf++ = -1;
#else
					*buf++ = 1;
#endif
				}
				else
				{
					buf += 4;
				}
			}

			if (has_size)
			{
				*buf++ = _pointSize;
			}

			if (has_tex1)
			{
				if (!_texCoords1.IsEmpty())::memcpy(buf, _texCoords1[i].p, sizeof(Vector2));
				buf += 2;
			}
			if (has_tex2)
			{
				if (!_texCoords2.IsEmpty())::memcpy(buf, _texCoords2[i].p, sizeof(Vector2));
				buf += 2;
			}

			if (has_indices)
			{
				if (!_weights.IsEmpty())
				{
					for (int j = 0; j < NUM_BONES_PER_VEREX; ++j)
					{
						*buf++ = _weights[i].IDs[j];//这里不能用DWToF
					}
				}
				else
				{
					buf += NUM_BONES_PER_VEREX;
				}
			}

			if (has_weights)
			{
				if (!_weights.IsEmpty())
				{
					for (int j = 0; j < NUM_BONES_PER_VEREX; ++j)
					{
						*buf++ = _weights[i].Weights[j];
					}
				}
				else
				{
					buf += NUM_BONES_PER_VEREX;
				}
			}

			if (i == 0)
			{
				min = position;
				max = position;
			}
			else
			{
				min.MakeFloor(position);
				max.MakeCeil(position);
			}
		}
	}
	_vertexData->Unlock(vbd);

	//instance
	if (_renderer)
	{
		bool has_instance_offset = false;
		const Vector<Matrix4>& instances = _renderer->GetInstanceTransform();
		if ((!instances.IsEmpty()) || (input_semantic != VertexSemantic::None && (input_semantic & VertexSemantic::InstanceOffset)))
			has_instance_offset = true;
		if (has_instance_offset)
		{
			VertexBufferDesc vbd(1, (uint)instances.Size(), desc.usage);
			float* buf = (float*)_vertexData->Lock(vbd);
			if (buf)
			{
				int size = sizeof(Matrix4);
				for (int i = 0; i < instances.Size(); i++)
				{
#if defined(DC_GRAPHICS_API_OPENGLES3) || defined(DC_GRAPHICS_API_OPENGL)
					Memory::Copy(buf, instances[i].Transpose().p, size);
#else
					Memory::Copy(buf, instances[i].p, size);
#endif
					buf += 16;
				}
			}
			_vertexData->Unlock(vbd);
		}
	}

	_boundingBox = Aabb(min, max);
}
DC_END_NAMESPACE