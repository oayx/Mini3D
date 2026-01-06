#include "EInspector_Project_Material.h"
#include "runtime/graphics/Material.h"
#include "runtime/graphics/null/GraphicsDevice.h"
#include "runtime/resources/AssetsManager.h"
#include "runtime/resources/Resources.h"
#include "runtime/ui/GUI.h"
#include "editor/common/EAssetSelect.h"
#include "editor/common/ECommonComponent.h"
#include "editor/main/EMain_Project.h"
#include "editor/EditorAppliction.h"
#include "editor/EditorCursor.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
void EInspector_Project_Material::OnEnter()
{
	_material = Material::Create(EditorAppliction::GetInspectorName());
	_material->Retain();
}
void EInspector_Project_Material::OnLeave()
{
	if (_material && File::Exist(Resource::GetFullDataPath(_material->GetResFile())))
	{//有可能重命名或删除了资源，这里验证一下是否存在
		_material->Deserialize();
		SAFE_RELEASE(_material);
	}
}
void EInspector_Project_Material::Render(AssetMeta* meta)
{
	DC_PROFILE_FUNCTION;
	base::Render(meta);

	if (!meta)return;
	RenderMaterial(_material, nullptr);
}
bool EInspector_Project_Material::RenderMaterial(Material* material, Action action)
{
	if (!material)return false;

	bool result = false;
	{
		ImGuiEx::Label("Shader");
		Shader* shader = material->GetShader();
		String shaderFile = "##Empty Shader";
		if (shader)
		{
			shaderFile = shader->GetGroupInspector();
			if (shaderFile.IsEmpty())shaderFile = Path::GetFileNameWithoutExtension(shader->GetResName());
		}
		ImGui::Button(shaderFile.c_str(), ImVec2(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(ICON_FA_LINK).x - 5, 0));
		if (ImGui::IsItemClicked(0) && shader)
		{
			EMain_Project::SetSelectFile(shader->GetResFile());
		}
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ProjectAsset"))
			{
				const String& filePath = EditorCursor::GetDragFile();
				if (Resource::GetResourceType(filePath) == ResourceType::Shader)
				{
					shader = material->SetShaderFile(filePath);
					material->SetShaderVariable(shader->GetVariables());
					result = true;
				}
				EditorCursor::EndDrag();
			}
			ImGui::EndDragDropTarget();
		}
		ImGui::SameLine();
		if (ImGui::Selectable(ICON_FA_LINK, false, 0, ImGui::CalcTextSize(ICON_FA_LINK)))
		{
			ImGui::OpenPopup("AddShaderPopup");
		}
		{
			auto OnSelectAsset = [&shader, action, material](const String& file)
			{
				shader = material->SetShaderFile(file);
				material->SetShaderVariable(shader->GetVariables());
				if (action)action();
			};
			ShowAddShader(OnSelectAsset);
		}
		
		if (shader)
		{
			//纹理
			{
				for (int i = 0; i < material->GetTextureCount(); ++i)
				{
					ImGui::NewLine();

					ImGui::PushID(i);
					ShaderTexture& tex = material->GetTexture(i);

					ImGuiEx::Label(tex.Name.c_str());
					ImGui::Image((ImTextureID)GUITextureCache::Get(tex.File), ImVec2(22, 22));
					if (ImGui::IsItemClicked(0) && !tex.File.IsEmpty())
					{
						EMain_Project::SetSelectFile(tex.File);
					}
					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ProjectAsset"))
						{
							const String& filePath = EditorCursor::GetDragFile();
							if (Resource::GetResourceType(filePath) == ResourceType::Texture)
							{
								tex.File = filePath;
								shader->SetTexture(tex);
							}
							EditorCursor::EndDrag();
						}
						ImGui::EndDragDropTarget();
					}
					ImGui::SameLine();
					if (ImGui::Selectable(ICON_FA_LINK,false, 0, ImGui::CalcTextSize(ICON_FA_LINK)))
					{
						auto OnSelectAsset = [shader, &tex](const String& file)
						{
							tex.File = file;
							shader->SetTexture(tex);
						};

						EAssetSelect::Search(ResourceType::Texture, OnSelectAsset);
					}

					{
						ImGuiEx::Label("Tiling");
						const float width = ImGui::GetContentRegionAvail().x;
						const float charWidth = ImGui::GetFontSize();

						ImGui::SetNextItemWidth(charWidth);
						ImGui::Text("X");

						float min_v = -GetGraphicsCaps()->maxTextureRepeat;
						float max_v = GetGraphicsCaps()->maxTextureRepeat;
						ImGui::SameLine();
						ImGui::SetNextItemWidth(width * 0.5f - charWidth);
						if (ImGui::DragScalar("##TilingX", ImGuiDataType_Float, &tex.Scale.x, 0.01f, &min_v, &max_v, "%.3f"))
						{
							shader->SetTexture(tex);
						}

						ImGui::SameLine();
						ImGui::SetNextItemWidth(charWidth);
						ImGui::TextUnformatted("Y");

						ImGui::SameLine();
						ImGui::SetNextItemWidth(width * 0.5f - charWidth);
						if (ImGui::DragScalar("##TilingY", ImGuiDataType_Float, &tex.Scale.y, 0.01f, &min_v, &max_v, "%.3f"))
						{
							shader->SetTexture(tex);
						}
					}
					{
						ImGuiEx::Label("Offset");
						const float width = ImGui::GetContentRegionAvail().x;
						const float charWidth = ImGui::GetFontSize();

						ImGui::SetNextItemWidth(charWidth);
						ImGui::Text("X");

						float min_v = -GetGraphicsCaps()->maxTextureRepeat;
						float max_v = GetGraphicsCaps()->maxTextureRepeat;
						ImGui::SameLine();
						ImGui::SetNextItemWidth(width * 0.5f - charWidth);
						if (ImGui::DragScalar("##OffsetX", ImGuiDataType_Float, &tex.Offset.x, 0.01f, &min_v, &max_v, "%.3f"))
						{
							shader->SetTexture(tex);
						}

						ImGui::SameLine();
						ImGui::SetNextItemWidth(charWidth);
						ImGui::TextUnformatted("Y");

						ImGui::SameLine();
						ImGui::SetNextItemWidth(width * 0.5f - charWidth);
						if (ImGui::DragScalar("##OffsetY", ImGuiDataType_Float, &tex.Offset.y, 0.01f, &min_v, &max_v, "%.3f"))
						{
							shader->SetTexture(tex);
						}
					}
					ImGui::PopID();
				}
			}

			//变量
			ImGui::NewLine();
			{
				for (int i = 0; i < material->GetVariableCount(); ++i)
				{
					ShaderVariable& var = material->GetVariable(i);
					if(!var.HideInspector)
					{
						ImGui::PushID(i);
						const char* szName = var.Alias.c_str();
						if (szName[0] == '\0')szName = var.Name.c_str();
						ImGuiEx::Label(szName);
						switch (var.Type)
						{
						case ShaderVariableType::D2:
						case ShaderVariableType::D3:
						case ShaderVariableType::Cube:
						{
						}
						case ShaderVariableType::Int:
						{
							int v = var.Value.ToInt();
							if (var.Range != Vector2::zero)
							{
								if (ImGui::DragInt("##Int", &v, 1, (int)var.Range.x, (int)var.Range.y))
								{
									var.Value = String::ToString(v);
								}
							}
							else
							{
								if (ImGui::DragInt("##int", &v))
								{
									var.Value = String::ToString(v);
								}
							}
							break;
						}
						case ShaderVariableType::Float:
						{
							float v = var.Value.ToFloat();
							if (var.Range != Vector2::zero)
							{
								if (ImGui::DragFloat("##Float", &v, 0.01f, var.Range.x, var.Range.y))
								{
									var.Value = String::ToString(v);
								}
							}
							else
							{
								if (ImGui::DragFloat("##Float", &v))
								{
									var.Value = String::ToString(v);
								}
							}
							break;
						}
						case ShaderVariableType::Bool:
						{
							bool b = Bool::FromString(var.Value);
							if (ImGui::Checkbox("##Bool", &b))
							{
								var.Value = Bool::ToString(b);
							}
							break;
						}
						case ShaderVariableType::Slider:
						{
							float v = var.Value.ToFloat();
							if (var.Range != Vector2::zero)
							{
								if (ImGui::SliderFloat("##Bool", &v, var.Range.x, var.Range.y))
								{
									var.Value = String::ToString(v);
								}
							}
							else
							{
								if (ImGui::SliderFloat("##Bool", &v, -10000.0f, 10000.0f))
								{
									var.Value = String::ToString(v);
								}
							}
							break;
						}
						case ShaderVariableType::Color:
						{
							Color color(var.Value);
							if (ECommonComponent::ShowColor("##Color", color.ptr(), false))
							{
								var.Value = color.ToString();
							}
							break;
						}
						case ShaderVariableType::Vector:
						{
							Vector4 vec(var.Value);
							if (var.Range != Vector2::zero)
							{
								if (ImGui::DragFloat4("##Vector", (float*)vec.ptr(), 0.01f, var.Range.x, var.Range.y))
								{
									var.Value = vec.ToString();
								}
							}
							else
							{
								if (ImGui::DragFloat4("##Vector", (float*)vec.ptr(), 0.01f))
								{
									var.Value = vec.ToString();
								}
							}
							break;
						}
						}
						ImGui::PopID();
					}
				}
			}
		}
	}
	return result;
}
void EInspector_Project_Material::ShowAddShader(std::function<void(String)> action)
{
	ImGui::SetNextWindowPos(ImGui::GetCursorScreenPos());
	ImGui::SetNextWindowSize(ImVec2(ImGui::GetContentRegionAvail().x, 300.0f));

	if (ImGui::BeginPopup("AddShaderPopup", 0))
	{
		ShowAddShader(AssetsManager::GetShaderGroup(), action);

		ImGui::EndPopup();
	}
}
void EInspector_Project_Material::ShowAddShader(const ShaderGroup* parent_group, std::function<void(String)> action)
{
	for (const auto& obj : parent_group->Files)
	{
		const char* szName = obj.first.c_str();

		if (ImGui::MenuItem(szName))
		{
			AssetMeta* meta = AssetsManager::GetMeta(obj.second);
			if(meta)action(meta->GetFullPath());
		}
	}

	for (const auto& obj : parent_group->Folders)
	{
		const char* szName = obj.first.c_str();
		if (ImGui::BeginMenu(szName))
		{
			ShowAddShader(&obj.second, action);
			ImGui::EndMenu();
		}
	}
}
DC_END_NAMESPACE