#include "TerrainObjectInstance.h"
#include "Terrain.h"
#include "runtime/Application.h"
#include "runtime/component/Component.inl"
#include "editor/main/EMain_GameView.h"
#include "editor/EditorAppliction.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
void TerrainObjectInstance::Start()
{
	base::Start();
	this->GenerateObjects();
}
void TerrainObjectInstance::GenerateObjects()
{
	DC_PROFILE_FUNCTION;
	//包围盒
	Vector3 min = Vector3::zero, max = Vector3::zero;
	Vector<Matrix4> mats;

	Terrain* terrain = GetGameObject()->GetComponentInParent<Terrain>(true);
	if (_density > 0.0f && _probability > 0 && terrain)
	{
		const Vector3& terrainScale = terrain->GetTransform()->GetScale();
		int rows = terrain->GetTileRows();
		int cols = terrain->GetTileCols();
		for (int row = 0; row < rows; row++)
		{
			for (int col = 0; col < cols; col++)
			{
				int count = 0;
				float density = _density;
				if (density >= 1.0f)
				{//大于1部分，直接加上
					count = (int)density;
					density = _density - count;
				}
				if (density < 1.0f && density > 0.0f)
				{//小于1部分，计算概率
					count += Math::RandomRange(0.0f, 1.0f) < density ? 1 : 0;
				}

				for (int c = 0; c < count; ++c)
				{
					//计算出现概率
					if (Math::RandomRange(0, 100) >= _probability)continue;

					Vector3 position; Vector3 scale = Vector3::one; Vector3 euler;
					position.y = terrain->GetHeight(row, col);
					position.x = terrain->GetPositionX(col);
					position.z = terrain->GetPositionZ(row);
					if (_positionMin != _positionMax)position += Vector3::Lerp(_positionMin, _positionMax, Math::RandomRange(0.0f, 1.0f));
					position = position / terrainScale;
					if (_scaleMin != _scaleMax)scale = Vector3::Lerp(_scaleMin, _scaleMax, Math::RandomRange(0.0f, 1.0f));
					if (_rotateMin != _rotateMax)euler = Vector3::Lerp(_rotateMin, _rotateMax, Math::RandomRange(0.0f, 1.0f));
					mats.Add(Matrix4(position, Quaternion(euler), scale));

					if (row == 0 && col == 0)
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
		}
	}
	this->SetInstanceTransform(std::move(mats));
	base::UploadData();

	this->GetComponent<Transform>()->SetLocalBoundingBox(Aabb(min, max));
}
IMPL_DERIVED_REFECTION_TYPE(TerrainObjectInstance, MeshRender);
void TerrainObjectInstance::OnDrawEditor()
{
	ImGuiTreeNodeFlags treeFlags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;
	if (ImGui::TreeNodeEx("Instance Data", treeFlags))
	{
		{
			ImGuiEx::Label("Density");
			float minValue = 0.0f;
			if (ImGui::DragScalar("##Density", ImGuiDataType_Float, &_density, 0.05f, &minValue, nullptr, "%.2f"))
			{
				this->GenerateObjects();
			}
		}
		{
			ImGuiEx::Label("Probability");
			int minValue = 0;
			int maxValue = 100;
			if (ImGui::DragScalar("##Probability", ImGuiDataType_U32, &_probability, 0.05f, &minValue, &maxValue))
			{
				this->GenerateObjects();
			}
		}
		{
			ImGui::PushID(1);
			ImGuiEx::Label("Position Min");

			const float width = ImGui::GetContentRegionAvail().x;
			const float charWidth = ImGui::GetFontSize();
			float min = MIN_float;
			float max = MAX_float;
			ImGui::TextUnformatted("X");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(width * 0.333f - charWidth);
			if (ImGui::DragScalar("##X", ImGuiDataType_Float, &_positionMin.x, 0.1f, &min, &max))
			{
				this->GenerateObjects();
			}

			ImGui::SameLine();
			ImGui::TextUnformatted("Y");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(width * 0.333f - charWidth);
			if (ImGui::DragScalar("##Y", ImGuiDataType_Float, &_positionMin.y, 0.1f, &min, &max))
			{
				this->GenerateObjects();
			}

			ImGui::SameLine();
			ImGui::TextUnformatted("Z");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(width * 0.333f - charWidth);
			if (ImGui::DragScalar("##Z", ImGuiDataType_Float, &_positionMin.z, 0.1f, &min, &max))
			{
				this->GenerateObjects();
			}
			ImGui::PopID();
		}
		{
			ImGui::PushID(2);
			ImGuiEx::Label("Position Max");

			const float width = ImGui::GetContentRegionAvail().x;
			const float charWidth = ImGui::GetFontSize();
			float max = MAX_float;
			ImGui::TextUnformatted("X");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(width * 0.333f - charWidth);
			if (ImGui::DragScalar("##X", ImGuiDataType_Float, &_positionMax.x, 0.1f, &_positionMin.x, &max))
			{
				this->GenerateObjects();
			}

			ImGui::SameLine();
			ImGui::TextUnformatted("Y");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(width * 0.333f - charWidth);
			if (ImGui::DragScalar("##Y", ImGuiDataType_Float, &_positionMax.y, 0.1f, &_positionMin.y, &max))
			{
				this->GenerateObjects();
			}

			ImGui::SameLine();
			ImGui::TextUnformatted("Z");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(width * 0.333f - charWidth);
			if (ImGui::DragScalar("##Z", ImGuiDataType_Float, &_positionMax.z, 0.1f, &_positionMin.z, &max))
			{
				this->GenerateObjects();
			}
			ImGui::PopID();
		}
		{
			ImGui::PushID(3);
			ImGuiEx::Label("Rotate Min");

			const float width = ImGui::GetContentRegionAvail().x;
			const float charWidth = ImGui::GetFontSize();
			float min = 0;
			float max = 360;
			ImGui::TextUnformatted("X");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(width * 0.333f - charWidth);
			if (ImGui::DragScalar("##X", ImGuiDataType_Float, &_rotateMin.x, 0.01f, &min, &max))
			{
				this->GenerateObjects();
			}

			ImGui::SameLine();
			ImGui::TextUnformatted("Y");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(width * 0.333f - charWidth);
			if (ImGui::DragScalar("##Y", ImGuiDataType_Float, &_rotateMin.y, 0.01f, &min, &max))
			{
				this->GenerateObjects();
			}

			ImGui::SameLine();
			ImGui::TextUnformatted("Z");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(width * 0.333f - charWidth);
			if (ImGui::DragScalar("##Z", ImGuiDataType_Float, &_rotateMin.z, 0.01f, &min, &max))
			{
				this->GenerateObjects();
			}
			ImGui::PopID();
		}
		{
			ImGui::PushID(4);
			ImGuiEx::Label("Rotate Max");

			const float width = ImGui::GetContentRegionAvail().x;
			const float charWidth = ImGui::GetFontSize();
			float max = 360;
			ImGui::TextUnformatted("X");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(width * 0.333f - charWidth);
			if (ImGui::DragScalar("##X", ImGuiDataType_Float, &_rotateMax.x, 0.01f, &_rotateMin.x, &max))
			{
				this->GenerateObjects();
			}

			ImGui::SameLine();
			ImGui::TextUnformatted("Y");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(width * 0.333f - charWidth);
			if (ImGui::DragScalar("##Y", ImGuiDataType_Float, &_rotateMax.y, 0.01f, &_rotateMin.y, &max))
			{
				this->GenerateObjects();
			}

			ImGui::SameLine();
			ImGui::TextUnformatted("Z");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(width * 0.333f - charWidth);
			if (ImGui::DragScalar("##Z", ImGuiDataType_Float, &_rotateMax.z, 0.01f, &_rotateMin.z, &max))
			{
				this->GenerateObjects();
			}
			ImGui::PopID();
		}
		{
			ImGui::PushID(5);
			ImGuiEx::Label("Scale Min");

			const float width = ImGui::GetContentRegionAvail().x;
			const float charWidth = ImGui::GetFontSize();
			float min = 0;
			float max = MAX_float;
			ImGui::TextUnformatted("X");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(width * 0.333f - charWidth);
			if (ImGui::DragScalar("##X", ImGuiDataType_Float, &_scaleMin.x, 0.01f, &min, &max))
			{
				this->GenerateObjects();
			}

			ImGui::SameLine();
			ImGui::TextUnformatted("Y");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(width * 0.333f - charWidth);
			if (ImGui::DragScalar("##Y", ImGuiDataType_Float, &_scaleMin.y, 0.01f, &min, &max))
			{
				this->GenerateObjects();
			}

			ImGui::SameLine();
			ImGui::TextUnformatted("Z");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(width * 0.333f - charWidth);
			if (ImGui::DragScalar("##Z", ImGuiDataType_Float, &_scaleMin.z, 0.01f, &min, &max))
			{
				this->GenerateObjects();
			}
			ImGui::PopID();
		}
		{
			ImGui::PushID(6);
			ImGuiEx::Label("Scale Max");

			const float width = ImGui::GetContentRegionAvail().x;
			const float charWidth = ImGui::GetFontSize();
			float max = MAX_float;
			ImGui::TextUnformatted("X");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(width * 0.333f - charWidth);
			if (ImGui::DragScalar("##X", ImGuiDataType_Float, &_scaleMax.x, 0.01f, &_scaleMin.x, &max))
			{
				this->GenerateObjects();
			}

			ImGui::SameLine();
			ImGui::TextUnformatted("Y");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(width * 0.333f - charWidth);
			if (ImGui::DragScalar("##Y", ImGuiDataType_Float, &_scaleMax.y, 0.01f, &_scaleMin.y, &max))
			{
				this->GenerateObjects();
			}

			ImGui::SameLine();
			ImGui::TextUnformatted("Z");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(width * 0.333f - charWidth);
			if (ImGui::DragScalar("##Z", ImGuiDataType_Float, &_scaleMax.z, 0.01f, &_scaleMin.z, &max))
			{
			}
			ImGui::PopID();
		}
		ImGui::TreePop();
	}

	base::OnDrawEditor();
}
/********************************************************************/
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(TerrainObjectInstance);
template<class TransferFunction> inline
void TerrainObjectInstance::Transfer(TransferFunction& transfer, void* ptr)
{
	base::Transfer(transfer, ptr);
	TRANSFER_SIMPLE(_density);
	TRANSFER_SIMPLE(_probability);
	TRANSFER_SIMPLE(_positionMin);
	TRANSFER_SIMPLE(_positionMax);
	TRANSFER_SIMPLE(_scaleMin);
	TRANSFER_SIMPLE(_scaleMax);
	TRANSFER_SIMPLE(_rotateMin);
	TRANSFER_SIMPLE(_rotateMax);
}
DC_END_NAMESPACE
