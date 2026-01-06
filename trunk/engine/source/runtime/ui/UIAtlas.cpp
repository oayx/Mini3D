#include "UIAtlas.h"
#include "runtime/graphics/null/Texture.h"
#include "runtime/resources/AssetsManager.h"
#include "external/json/nlohmann/json.hpp"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_REFECTION_TYPE(UIAtlasManager);
void UIAtlasManager::Destroy()
{
	for (auto obj : _atlases)
	{
		SAFE_DELETE(obj.second);
	}
	_atlases.Clear();
}
bool UIAtlasManager::LoadAtlas(const String& atlas_name)
{
	if (atlas_name.IsEmpty())return false;
	auto it = _atlases.Find(atlas_name);
	if (it != _atlases.end())
	{
		Debuger::Warning("the atlas already exists:%s", atlas_name.c_str());
		return false;
	}

	String file = GetAtlasJsonFile(atlas_name);
	UIAtlas* atlas = UIAtlas::Create();
	if (!atlas->Parse(file))
	{
		SAFE_DELETE(atlas);
		Debuger::Error("the atlas parse error:%s", atlas_name.c_str());
		return false;
	}

	AssertEx(_atlases.Add(atlas_name, atlas),"");
	return true;
}
bool UIAtlasManager::UnloadAtlas(const String& atlas_name)
{
	if (atlas_name.IsEmpty())return false;
	auto it = _atlases.Find(atlas_name);
	if (it == _atlases.end())
	{
		Debuger::Warning("cannot find atlas:%s", atlas_name.c_str());
		return false;
	}
	SAFE_DELETE(it->second);
	auto ret = _atlases.Remove(it);
	return ret->second;
}
bool UIAtlasManager::GetAtlasInfo(const String& atlas_name, const String& name, UIAtlasInfo& atlas)
{
	auto it = _atlases.Find(atlas_name);
	if (it == _atlases.end())
	{
		LoadAtlas(atlas_name);
		it = _atlases.Find(atlas_name);
	}
	if (it == _atlases.end())
	{
		return false;
	}
	return it->second->Get(name, atlas);
}
bool UIAtlasManager::HasAtlasInfo(const String& atlas_name, const String& name)
{
	auto it = _atlases.Find(atlas_name);
	if (it == _atlases.end())
	{
		return false;
	}
	return it->second->Has(name);
}
bool UIAtlasManager::IsAtlasFile(const String& atlas_name)
{
	const Vector<AssetMeta*>& group = AssetsManager::GetAssetGroup(ResourceType::SpriteAtlas);
	bool has = false;
	for (int i = 0; i < group.Size(); ++i)
	{
		if (group[i]->GetFullPath().Equals(atlas_name))
		{
			has = true;
			break;
		}
	}

	return has;
}
UIAtlas* UIAtlasManager::GetAtlas(const String& atlas_name)
{
	UIAtlas* atlas = nullptr;
	if (_atlases.TryGet(atlas_name, &atlas))
		return atlas;
	return nullptr;
}
String UIAtlasManager::GetAtlasJsonFile(const String& atlas_name)
{
	if (atlas_name.IsEmpty())return "";
	return Path::Combine(Path::GetDirectoryName(atlas_name), Path::GetFileNameWithoutExtension(atlas_name) + ".json");
}
Texture* UIAtlasManager::GetAtlasTexture(const String& atlas_name)
{
	auto it = _atlases.Find(atlas_name);
	if (it == _atlases.end())
	{
		LoadAtlas(atlas_name);
		it = _atlases.Find(atlas_name);
	}
	if (it == _atlases.end())
	{
		return nullptr;
	}
	return it->second->GetTexture();
}
/********************************************************************/
IMPL_REFECTION_TYPE(UIAtlas);
UIAtlas::UIAtlas()
{
}
UIAtlas::~UIAtlas()
{
	SAFE_RELEASE(_texture);
}
bool UIAtlas::Parse(const String& file)
{
	String filePath = Resource::GetFullDataPath(file);
	if (!File::Exist(filePath))
	{
		Debuger::Error("cannot find file:%s", file.c_str());
		return false;
	}

	//parse
	bool result = true;
	do 
	{
		String content = File::ReadAllText(filePath);
		nlohmann::json json = nlohmann::json::parse(content.c_str());
		if (json.is_null())
		{
			result = false; break;
		}

		auto meta = json["meta"];
		if (meta.is_null())
		{
			result = false; break;
		}
		String imageName = meta["image"].get<std::string>();
		int imageWidth = meta["size"]["w"].get<int>();
		int imageHeight = meta["size"]["h"].get<int>();

		auto frames = json["frames"];
		if (frames.is_null())
		{
			result = false; break;
		}
		for (auto frame_it = frames.begin(); frame_it != frames.end(); ++frame_it)
		{
			auto frame_node = *frame_it;
			std::string frameName = "";
			if (frames.is_array())//数组
				frameName = frame_node["filename"];
			else//字典
				frameName = frame_it.key();
			if (frameName.empty())continue;
			auto frame = frame_node["frame"];
			if (frame.is_null())
			{
				result = false; break;
			}
			float x = frame["x"].get<float>();
			float y = frame["y"].get<float>();
			float w = frame["w"].get<float>();
			float h = frame["h"].get<float>();
			//bool rotated = frame_node["rotated"].get<bool>();
			//bool trimmed = frame_node["trimmed"].get<bool>();

			UIAtlasInfo info = { frameName, Rect(x, y, w, h), Rect(x / imageWidth, y / imageHeight, w / imageWidth, h / imageHeight) };
			AssertEx(_atlasInfoes.Add(frameName, info), "");
		}

		//create texture
		if (result)
		{
			String path = Path::GetDirectoryName(file);
			_texture = Texture::Create(path + "/" + imageName);
			_texture->Retain();
			_texture->SetInstanceName(path + "/" + imageName);
		}
	} while (false);

	if (!result)
	{
		Debuger::Error("json file parse error:%s", file.c_str());
	}

	return result;
}
bool UIAtlas::Get(const String& name, UIAtlasInfo& atlas)
{
	auto it = _atlasInfoes.Find(name);
	if (it == _atlasInfoes.end())
	{
		return false;
	}
	atlas = it->second;
	return true;
}
bool UIAtlas::Has(const String& name)
{
	auto it = _atlasInfoes.Find(name);
	if (it == _atlasInfoes.end())
	{
		return false;
	}
	return true;
}
DC_END_NAMESPACE
