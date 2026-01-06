#include "EInspector_Project_Texture.h"
#include "runtime/graphics/null/Texture.h"
#include "runtime/resources/AssetsManager.h"
#include "runtime/ui/GUI.h"
#include "runtime/ui/UIAtlas.h"
#include "runtime/Application.h"
#include "editor/main/EMain_Hierarchy.h"
#include "editor/EditorAppliction.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
void EInspector_Project_Texture::Render(AssetMeta* meta)
{
	DC_PROFILE_FUNCTION;
	base::Render(meta);

	if (!meta)return;
	TextureMeta* tex_meta = meta->As<TextureMeta>();
	if (!tex_meta)return;

	RenderGUI(tex_meta);
	RenderModal(tex_meta);
}
void EInspector_Project_Texture::RenderGUI(TextureMeta* meta)
{
	{
		const char* values[] = { "Default", "Normal map", "Sprite Atlas" };
		ImGuiEx::Label("Texture Type");
		int selectIndex = (int)_textureMeta._textureGroup;
		if (ImGui::Combo("##Texture Group", &selectIndex, values, ARRAY_SIZE(values)))
		{
			if (selectIndex == (int)TextureGroup::SpriteAtlas)
			{//判断是否合法图集
				String json_file = UIAtlasManager::GetAtlasJsonFile(meta->GetFullPath());
				if (!File::Exist(Resource::GetFullDataPath(json_file)))selectIndex = (int)TextureGroup::Default;
			}
			_textureMeta.SetTextureGroup((TextureGroup)selectIndex);
		}
	}

	{
		const char* values[] = { "2D", "Cube" };
		ImGuiEx::Label("Texture Shape");
		int selectIndex = _textureMeta._textureType == TextureType::Cube ? 1 : 0;
		if (ImGui::Combo("##Texture Type", &selectIndex, values, ARRAY_SIZE(values)))
		{
			if (_textureMeta._textureGroup != TextureGroup::SpriteAtlas)
			{
				_textureMeta._textureType = selectIndex == 1 ? TextureType::Cube : TextureType::D2;
			}
		}
	}

	{
		ImGui::NewLine();

		ImGuiEx::Label("sRGB");
		ImGui::Checkbox("##srgb", &_textureMeta._sRGB);

		ImGuiEx::Label("Read/Write Enabled");
		ImGui::Checkbox("##Read/Write", &_textureMeta._enableReadWrite);

		ImGuiEx::Label("Generate Mip Maps");
		ImGui::Checkbox("##GenerateMipMaps", &_textureMeta._generateMipMaps);
	}

	{
		ImGui::NewLine();
		{
			const char* values[] = { "Wrap", "Mirror", "Clamp" };
			ImGuiEx::Label("Wrap Mode");
			int selectIndex = (int)_textureMeta._addressMode;
			if (ImGui::Combo("##Wrap Mode", &selectIndex, values, ARRAY_SIZE(values)))
			{
				_textureMeta._addressMode = TextureAddress(selectIndex);
			}
		}
		{
			const char* values[] = { "Point", "Bilinear", "Trilinear" };
			ImGuiEx::Label("Filter Mode");
			int selectIndex = (int)_textureMeta._filterType;
			if (ImGui::Combo("##Filter Mode", &selectIndex, values, ARRAY_SIZE(values)))
			{
				_textureMeta._filterType = TextureFilter(selectIndex);
			}

			ImGuiEx::Label("Aniso Level");
			int aniso = (int)_textureMeta._anisotropy;
			if (ImGui::SliderInt("##Aniso Level", &aniso, 1, GetGraphicsCaps()->maxAnisoLevel))
			{
				_textureMeta._anisotropy = aniso;
			}
		}
	}

	{//格式
		ImGui::NewLine();
		if (ImGui::BeginTabBar("Compression", ImGuiTabBarFlags_None))
		{
			if (ImGui::BeginTabItem("  Default  "))
			{
				{
					const char* values[] = { "32", "64", "128", "256", "512", "1024", "2048", "4096", "8192" };
					ImGuiEx::Label("Max Size");
					int selectIndex = 5;
					if (ImGui::Combo("##Max Size", &selectIndex, values, ARRAY_SIZE(values), ARRAY_SIZE(values)))
					{
					}
				}
				{
					const char* values[] = { "None", "Low Quality", "Normal Quality", "High Quality" };
					ImGuiEx::Label("Compression");
					int selectIndex = 2;
					if (ImGui::Combo("##Compression", &selectIndex, values, ARRAY_SIZE(values), ARRAY_SIZE(values)))
					{
					}
				}

				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("  PC  "))
			{
				static bool selected = false;
				ImGui::Checkbox("Override for PC，Mac & Linux Standalone", &selected);

				const char* values[] = { "RGB24", "RGBA32", "DXT1", "DXT3", "DXT5",
					"ASTC_RGB_4x4", "ASTC_RGB_5x5", "ASTC_RGB_6x6", "ASTC_RGB_8x8", "ASTC_RGB_10x10", "ASTC_RGB_12x12",
				"ASTC_RGBA_4x4", "ASTC_RGBA_5x5", "ASTC_RGBA_6x6", "ASTC_RGBA_8x8", "ASTC_RGBA_10x10", "ASTC_RGBA_12x12" };
				ImGuiEx::Label("Compression");
				int selectIndex = 4;
				if (ImGui::Combo("##FormatPC", &selectIndex, values, ARRAY_SIZE(values), ARRAY_SIZE(values)))
				{
				}

				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("  Android  "))
			{
				static bool selected = false;
				ImGui::Checkbox("Override for Android", &selected);

				const char* values[] = { "RGB24", "RGBA32", "ETC1", "ETC2_RGB4", "ETC2_RGBA8",
					"ASTC_RGB_4x4", "ASTC_RGB_5x5", "ASTC_RGB_6x6", "ASTC_RGB_8x8", "ASTC_RGB_10x10", "ASTC_RGB_12x12",
				"ASTC_RGBA_4x4", "ASTC_RGBA_5x5", "ASTC_RGBA_6x6", "ASTC_RGBA_8x8", "ASTC_RGBA_10x10", "ASTC_RGBA_12x12" };
				ImGuiEx::Label("Compression");
				int selectIndex = 1;
				if (ImGui::Combo("##FormatAndroid", &selectIndex, values, ARRAY_SIZE(values), ARRAY_SIZE(values)))
				{
				}

				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("  iOS  "))
			{
				static bool selected = false;
				ImGui::Checkbox("Override for iOS", &selected);

				const char* values[] = { "RGB24", "RGBA32", "PVRTC_RGB2", "PVRTC_RGBA2", "PVRTC_RGB4", "PVRTC_RGBA4",
					"ASTC_RGB_4x4", "ASTC_RGB_5x5", "ASTC_RGB_6x6", "ASTC_RGB_8x8", "ASTC_RGB_10x10", "ASTC_RGB_12x12",
				"ASTC_RGBA_4x4", "ASTC_RGBA_5x5", "ASTC_RGBA_6x6", "ASTC_RGBA_8x8", "ASTC_RGBA_10x10", "ASTC_RGBA_12x12" };
				ImGuiEx::Label("Compression");
				int selectIndex = 1;
				if (ImGui::Combo("##FormatiOS", &selectIndex, values, ARRAY_SIZE(values), ARRAY_SIZE(values)))
				{
				}

				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("  WebGL  "))
			{
				static bool selected = false;
				ImGui::Checkbox("Override for WebGL", &selected);

				const char* values[] = { "RGB24", "RGBA32" };
				ImGuiEx::Label("Compression");
				int selectIndex = 1;
				if (ImGui::Combo("##FormatWebGL", &selectIndex, values, ARRAY_SIZE(values), ARRAY_SIZE(values)))
				{
				}

				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
	}

	ImGui::Separator();
	ImGui::NewLine();
	{
		const float width = ImGui::GetContentRegionAvail().x;
		float x = ImGui::GetCursorScreenPos().x + width - 2.0f * (ImGui::CalcTextSize("Revert").x + 15.0f);
		ImGui::SetCursorScreenPos(ImVec2(x, ImGui::GetCursorScreenPos().y - 10.0f));
		if (ImGui::Button("Revert"))
		{
			_textureMeta = *meta;
		}
		ImGui::SameLine();
		if (ImGui::Button("Apply"))
		{
			*meta = _textureMeta;
			meta->Deserialize();
		}
	}

	ImGui::NewLine();
	if (ImGui::CollapsingHeader("ImageView", ImGuiTreeNodeFlags_DefaultOpen))
	{//预览
		ImVec2 contentSize = ImGui::GetContentRegionAvail();
		Texture* tex = GUITextureCache::Get(meta->GetFullPath());
		if (tex)
		{
			ImVec2 old_cursor_pos = ImGui::GetCursorScreenPos();
			{
				int width = tex->GetWidth(); int height = tex->GetHeight();
				if (width > contentSize.x)
				{
					height = (contentSize.x * height) / width;
					width = contentSize.x;
				}

				float start_x = ImGui::GetCursorScreenPos().x;
				float start_y = ImGui::GetCursorScreenPos().y;
				if (width < contentSize.x)
				{
					start_x = ImGui::GetCursorScreenPos().x + (contentSize.x - width) * 0.5f;
				}
				if (height < contentSize.y)
				{
					start_y = ImGui::GetCursorScreenPos().y + (contentSize.y - height) * 0.5f;
				}
				ImGui::SetCursorScreenPos(ImVec2(start_x, start_y));
				ImGui::Image(tex, ImVec2(width, height));
			}
			{
				char szName[50] = { 0 };
				String format_name = GetImageFormatName(tex->GetFormat());
				Snprintf(szName, ARRAY_SIZE(szName), "%dx%d %s", tex->GetWidth(), tex->GetHeight(), format_name.c_str());
				float content_width = ImGui::CalcTextSize(szName).x;

				float offsetY = contentSize.y - 50.0f;
				if (offsetY < 0.0f)offsetY = 0.0f;
				ImGui::SetCursorScreenPos(ImVec2(old_cursor_pos.x + (contentSize.x - content_width)*0.5f, old_cursor_pos.y + offsetY));
				ImGui::TextUnformatted(szName);
			}
		}
	}
}
void EInspector_Project_Texture::RenderModal(TextureMeta* meta)
{
	static TextureMeta old_tmp_meta;
	static String old_meta_guid = "";
	if (meta->GetGUID() != _textureMeta.GetGUID())
	{//是否选择了其他文件
		TextureMeta* old_meta = dynamic_cast<TextureMeta*>(AssetsManager::GetMeta(_assetGUID));
		if (old_meta && (*old_meta != _textureMeta))
		{
			old_tmp_meta = _textureMeta;
			old_meta_guid = _assetGUID;
			ImGui::OpenPopup("Unapplied import settings");
		}
		else
		{
			_assetGUID = meta->GetGUID();
			_textureMeta = *meta;
		}
	}
	{//模态对话框
		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		if (ImGui::BeginPopupModal("Unapplied import settings", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::NewLine();
			ImGui::TextUnformatted(old_tmp_meta.GetFullPath().c_str());
			ImGui::NewLine();
			if (ImGui::Button("Apply", ImVec2(120, 0)))
			{
				TextureMeta* old_meta = dynamic_cast<TextureMeta*>(AssetsManager::GetMeta(old_meta_guid));
				if (old_meta)
				{
					*old_meta = old_tmp_meta;
					old_meta->Deserialize();
				}
				_assetGUID = meta->GetGUID();
				_textureMeta = *meta;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			if (ImGui::Button("Revert", ImVec2(120, 0)))
			{
				_assetGUID = meta->GetGUID();
				_textureMeta = *meta;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}
}
DC_END_NAMESPACE