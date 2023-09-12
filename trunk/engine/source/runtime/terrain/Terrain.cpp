#include "Terrain.h"
#include "core/image/Image.h"
#include "runtime/graphics/Material.h"
#include "runtime/graphics/null/Texture.h" 
#include "runtime/graphics/Pass.h" 
#include "runtime/resources/Resources.h"
#include "runtime/physics/Collider.h"
#include "editor/common/ECommonComponent.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(Terrain, Renderer);
Terrain::~Terrain() 
{
	this->Clear();
}
bool Terrain::CanRemove()
{
	return !GetGameObject()->GetComponent<TerrainCollider>();
}
Object* Terrain::Clone(Object* new_obj)
{
	base::Clone(new_obj);
	Terrain* obj = dynamic_cast<Terrain*>(new_obj);
	if (!obj)return new_obj;

	obj->SetHeightMap(_heightMapFile, _heightLimit, _smoothIterator, _smoothRange);

	return obj;
}
void Terrain::SetHeightMap(const String& file, float height_limit, int smooth_iterator, int smooth_range)
{
	_heightMapFile = file; 
	_heightLimit = height_limit;
	_smoothIterator = smooth_iterator;
	_smoothRange = smooth_range;
	this->Build();
}
float Terrain::GetHeight(int row, int col)
{
	row = Math::Clamp(row, 0, _tileRows - 1);
	col = Math::Clamp(col, 0, _tileCols - 1);
	return _heightData[row * _tileCols + col];
}
void Terrain::SetHeight(int row, int col, float height)
{
	if (row < 0 || row >= _tileRows || col < 0 || col >= _tileCols)
	{
		Debuger::Warning("index out range:%d,%d", row, col);
		return;
	}
	_heightData[row * _tileCols + col] = height;
	_minHeight = height < _minHeight ? height : _minHeight;
	_maxHeight = height > _maxHeight ? height : _maxHeight;
}
int	Terrain::GetRow(float z)
{
	float scale = GetTransform()->GetScale().z;
	float half_height = (float)_tileRows / 2.0f * scale;
	int row = int(half_height - z - 0.5f * scale);
	return row;
}
int	Terrain::GetCol(float x)
{
	float scale = GetTransform()->GetScale().x;
	float half_width = (float)_tileCols / 2.0f * scale;
	int col = int(half_width + x - 0.5f * scale);
	return col;
}
float Terrain::GetPositionX(int col)
{
	float scale = GetTransform()->GetScale().x;
	float half_width = (float)_tileCols / 2.0f;
	float x = float(col) - half_width + 0.5f;
	return x * scale;
}
float Terrain::GetPositionZ(int row)
{
	float scale = GetTransform()->GetScale().z;
	float half_height = (float)_tileRows / 2.0f;
	float z = half_height - float(row) - 0.5f;
	return z * scale;
}
float Terrain::GetHeight(float x, float z)
{
	float scale = GetTransform()->GetScale().y;

	int row = GetRow(z), col = GetCol(x);
	float height1 = GetHeight(row, col);		//左上
	float height2 = GetHeight(row, col + 1);	//右上
	float height3 = GetHeight(row + 1, col + 1);//右下
	float height4 = GetHeight(row + 1, col);	//左下

	float position_x = GetPositionX(col);
	float position_z = GetPositionZ(row);
	float u = (x - position_x) / (GetPositionX(col + 1) - position_x);
	float v = (z - position_z) / (GetPositionZ(col + 1) - position_z);

	float height = (1.0f - u)*(1.0f - v)*height1 + u * (1.0f - v)*height2 + u * v * height3 + (1.0f - u)*v*height4;
	return height;
}
void Terrain::Build()
{
	LoadHeightMap();
	SmoothHeight();
	BuildTile();
}
void Terrain::LoadHeightMap()
{
	this->Clear();

	if (_heightMapFile.IsEmpty())
	{
		_tileRows = _tileRows = 0;
		return;
	}

	Image* image = Image::Create(_heightMapFile, false);
	if (image != nullptr)
	{
		_tileRows = image->GetHeight();
		_tileCols = image->GetWidth();
		if (_tileCols < 2 || _tileRows < 2)
		{
			_tileRows = _tileRows = 0;
			return;
		}
		bool is_valid_size = (Math::IsPOT2(_tileRows) && Math::IsPOT2(_tileCols)) || (Math::IsPOT2(_tileRows - 1) && Math::IsPOT2(_tileCols - 1));
		if (!is_valid_size)
		{
			Debuger::Error("The heightmap size must pod");
			_tileRows = _tileRows = 0;
			return;
		}

		_heightData = NewArray<float>(_tileRows * _tileCols);
		memset(_heightData, 0, sizeof(float) * _tileRows * _tileCols);

		//读取高度数据
		for (int row = 0; row < _tileRows; ++row)
		{
			for (int col = 0; col < _tileCols; ++col)
			{
				const Color& color = image->GetPixel(col, row);
				SetHeight(row, col, color.r * _heightLimit - _heightLimit *0.5f);
			}
		}
	}
}
void Terrain::SmoothHeight()
{
	if (!_heightData)return;
	if (_smoothIterator > 0 && _smoothRange > 0)
	{
		for (int it = 0; it < _smoothIterator; ++it)
		{
			for (int row = 0; row < _tileRows; ++row)
			{
				for (int col = 0; col < _tileCols; ++col)
				{
					float total_height = 0;
					int smooth_count = 0;
					for (int i = row - _smoothRange; i <= row + _smoothRange; ++i)
					{
						for (int j = col - _smoothRange; j <= col + _smoothRange; ++j)
						{
							if (i < 0 || i >= _tileRows || j < 0 || j >= _tileCols)
							{
								continue;
							}
							total_height += _heightData[i * _tileCols + j];
							smooth_count++;
						}
					}
					SetHeight(row, col, total_height / smooth_count);
				}
			}
		}
	}
}
void Terrain::BuildTile()
{
	this->ClearData();

	if (_tileCols < 2 || _tileRows < 2 || !_heightData)
		return;

	int buff_size = (_tileRows) * (_tileCols);
	Vector3v vertexs; Vector3v normals; Vector3v tangents; Vector2v texcoords;
	vertexs.Reserve(buff_size); normals.Reserve(buff_size); texcoords.Reserve(buff_size);

	//顶点信息
	{
		float half_width = (float)_tileCols / 2.0f;		//一半大小
		float half_height = (float)_tileRows / 2.0f;
		Vector3 vec3; Vector2 vec2;
		for (int row = 0; row < _tileRows; ++row)
		{
			for (int col = 0; col < _tileCols; ++col)
			{
				float height = GetHeight(row, col);

				//顶点数据
				vec3.x = (float)col - half_width + 0.5f;
				vec3.y = GetHeight(row, col);
				vec3.z = half_height - (float)row - 0.5f;
				vertexs.Add(vec3);

				//法线后面再计算
				normals.Add(Vector3::zero);

				//切线数据
				tangents.Add(Vector3(1, 0, 0));

				//纹理坐标
				vec2.x = (float)col / (float)_tileCols;
				vec2.y = (float)row / (float)_tileRows;
				texcoords.Add(vec2);
			}
		}
		base::SetVertexs(std::move(vertexs));
		base::SetTangents(std::move(tangents));
		base::SetTexcoords1(std::move(texcoords));
	}

	//索引信息
	{
		uint* indexs = NewArray<uint>((_tileRows - 1) * (_tileCols - 1) *  6);
		int iPerEdgeVerCount = _tileCols;
		int q = 0; int index = 0;
		for (ushort j = 0; j < _tileCols - 1; j += 1)
		{
			int p = 0;
			for (ushort i = 0; i < _tileRows - 1; i += 1)
			{
				//索引
				indexs[index + 0] = (p*iPerEdgeVerCount + q);
				indexs[index + 1] = (p*iPerEdgeVerCount + q + iPerEdgeVerCount + 1);
				indexs[index + 2] = (p*iPerEdgeVerCount + q + iPerEdgeVerCount);
				indexs[index + 3] = (p*iPerEdgeVerCount + q);
				indexs[index + 4] = (p*iPerEdgeVerCount + q + 1);
				indexs[index + 5] = (p*iPerEdgeVerCount + q + iPerEdgeVerCount + 1);

				//法线
				Vector3 v1 = vertexs[indexs[index + 0]];
				Vector3 v2 = vertexs[indexs[index + 1]];
				Vector3 v3 = vertexs[indexs[index + 2]];
				Vector3 n1 = Vector3(v2 - v1).Cross(Vector3(v3 - v1));
				normals[indexs[index + 0]] += n1;
				normals[indexs[index + 1]] += n1;
				normals[indexs[index + 2]] += n1;

				Vector3 v4 = vertexs[indexs[index + 3]];
				Vector3 v5 = vertexs[indexs[index + 4]];
				Vector3 v6 = vertexs[indexs[index + 5]];
				Vector3 n2 = Vector3(v5 - v4).Cross(Vector3(v6 - v4));
				normals[indexs[index + 3]] += n2;
				normals[indexs[index + 4]] += n2;
				normals[indexs[index + 5]] += n2;

				index += 6;
				p++;
			}
			q++;
		}
		base::SetIndices((_tileRows - 1) * (_tileCols - 1) * 6, indexs);
		base::SetNormals(std::move(normals));
		DeleteArray(indexs);
	}
	this->UploadData();
}
void Terrain::Clear()
{
	_minHeight = MAX_float;
	_maxHeight = MIN_float;
	if (_heightData)
	{
		DeleteArray(_heightData);
		_heightData = nullptr;
	}
}
void Terrain::OnDrawEditor()
{

	ImGuiTreeNodeFlags tree_flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;
	if (ImGui::TreeNodeEx("Terrain Data", tree_flags))
	{
		{
			ImGuiEx::Label("Width");
			ImGui::Text("%d", _tileCols);
			ImGuiEx::Label("Height");
			ImGui::Text("%d", _tileRows);
		}

		{
			ImGui::NewLine();
			{
				ImGuiEx::Label("Height Limit");
				float min_value = 0.01f;
				float max_value = MAX_float;
				float scale = _heightLimit;
				if (ImGui::DragScalar("##HeightLimit", ImGuiDataType_Float, &scale, 0.01f, &min_value, &max_value, "%.2f"))
				{
					if (scale != _heightLimit)
					{
						_heightLimit = scale;
						Build();
					}
				}
			}
			{
				ImGuiEx::Label("Smooth Iterator");
				int iterator = _smoothIterator;
				if (ImGui::InputInt("##SmoothIterator", &iterator))
				{
					iterator = Math::Clamp(iterator, 0, 10);
					if (iterator != _smoothIterator)
					{
						_smoothIterator = iterator;
						SmoothHeight();
						BuildTile();
					}
				}
			}
			if (_smoothIterator > 0)
			{
				ImGuiEx::Label("Smooth Range");
				int range = _smoothRange;
				if (ImGui::InputInt("##SmoothRange", &range))
				{
					range = Math::Clamp(range, 1, 10);
					if (range != _smoothRange)
					{
						_smoothRange = range;
						SmoothHeight();
						BuildTile();
					}
				}
			}
		}
		ImGui::TreePop();
	}

	base::OnDrawEditor();
}
/********************************************************************/
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(Terrain);
template<class TransferFunction> inline
void Terrain::Transfer(TransferFunction& transfer, void* ptr)
{
	base::Transfer(transfer, ptr);

	TRANSFER_SIMPLE(_heightMapFile);
	TRANSFER_SIMPLE(_heightLimit);
	TRANSFER_SIMPLE(_smoothIterator);
	TRANSFER_SIMPLE(_smoothRange);
	if (transfer.IsRead())
	{
		this->Build();
	}
}
DC_END_NAMESPACE
