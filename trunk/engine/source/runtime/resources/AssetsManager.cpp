#include "AssetsManager.h"
#include "core/Utility.h"
#include "runtime/graphics/Shader.h"
#include "runtime/resources/Resources.h"
#include "runtime/Application.h"
#include "editor/common/EditorIcon.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(AssetsManager, Object);
EventController	AssetsManager::_eventController;
AssetMeta* AssetsManager::_internalFile = nullptr;
Map<String, AssetMeta*>	AssetsManager::_internalGuid2Metas;
HashMap<uint64, AssetMeta*>	AssetsManager::_internalFile2Metas;
AssetMeta* AssetsManager::_assetsFile = nullptr;
Map<String, AssetMeta*> AssetsManager::_guid2Metas;
HashMap<uint64, AssetMeta*> AssetsManager::_file2Metas;
ShaderGroup AssetsManager::_shaderGroup;
HashMap<ResourceType, Vector<AssetMeta*>> AssetsManager::_internalGroup;
HashMap<ResourceType, Vector<AssetMeta*>> AssetsManager::_assetsGroup;
void AssetsManager::Initialize()
{
	for (int i = 0; i < (int)ResourceType::Max; ++i)
	{
		_internalGroup.Add((ResourceType)i, Vector<AssetMeta*>());
		_assetsGroup.Add((ResourceType)i, Vector<AssetMeta*>());
	}

#if defined(DC_PLATFORM_WIN32) || defined(DC_PLATFORM_LINUX)
	String full_path = Resource::GetInternalDataPath();
	Debuger::Log("AssetsManager::Initialize - %s", full_path.c_str());
	FileInfo file_info(full_path, SearchOption::AllDirectories);
	_internalFile = AssetMeta::Create(ResourceType::Undefined);
	RefreshAsset(_internalFile, file_info, full_path, true);
#endif
}
void AssetsManager::Destroy()
{
	_internalGroup.Clear();
	_internalGuid2Metas.Clear();
	_internalFile2Metas.Clear();
	SAFE_DELETE(_internalFile);
	
	_assetsGroup.Clear();
	_guid2Metas.Clear();
	_file2Metas.Clear();
	SAFE_DELETE(_assetsFile);
}
void AssetsManager::AddSpriteAtlas(const String& file)
{
	AssetMeta* meta = GetMetaByFile(file);
	if (meta)
	{
		Vector<AssetMeta*>& group = _assetsGroup[ResourceType::SpriteAtlas];
		bool has = false;
		for (int i = 0; i < group.Size(); ++i)
		{
			if (group[i]->GetFullPath().Equals(file))
			{
				has = true;
				break;
			}
		}
		if (!has)
		{
			group.Add(meta);
		}
	}
}
void AssetsManager::RemoveSpriteAtlas(const String& file)
{
	Vector<AssetMeta*>& group = _assetsGroup[ResourceType::SpriteAtlas];
	for (int i = 0; i < group.Size(); ++i)
	{
		if (group[i]->GetFullPath().Equals(file))
		{
			group.RemoveAt(i);
			break;
		}
	}
}
AssetMeta* AssetsManager::GetMeta(const String& guid)
{
	if (guid.IsEmpty())return nullptr;

	AssetMeta* meta = nullptr;
	if (_internalGuid2Metas.TryGet(guid, &meta))
		return meta;

	if (_guid2Metas.TryGet(guid, &meta))
		return meta;
	else
		return nullptr;
}
AssetMeta* AssetsManager::GetMetaByFile(const String& file)
{
	if (file.IsEmpty())return nullptr;

	String file_path = Path::ReplaceSplit(file);
	uint64 hash = file_path.GetHash();
	AssetMeta* meta = nullptr;
	if (_internalFile2Metas.TryGet(hash, &meta))
		return meta;

	if (_file2Metas.TryGet(hash, &meta))
		return meta;
	else
		return nullptr;
}
AssetMeta* AssetsManager::GetMetaByPath(const String& path)
{
	VecString splits = path.Split("/");
	if (splits.Size() == 0)
		return _assetsFile;

	int i = 0;
	AssetMeta* file_info = _assetsFile;
	for (; i < splits.Size(); ++i)
	{
		for (int j = 0; j < file_info->GetChildrenCount(); ++j)
		{
			AssetMeta* child_file_info = file_info->GetChildren(j);
			if (child_file_info->IsFolder() && Path::GetFileNameWithoutExtension(child_file_info->GetFileName()) == splits[i])
			{
				file_info = child_file_info;
				break;
			}
		}
	}

	if (i == splits.Size())
		return file_info;
	else
		return nullptr;
}
FontMeta* AssetsManager::GetFontMeta(const String& font_name)
{
	const Vector<AssetMeta*>& list = GetAssetGroup(ResourceType::Font);
	for (const auto& meta : list)
	{
		FontMeta* font_meta = dynamic_cast<FontMeta*>(meta);
		if (font_meta && font_meta->GetFontName().Equals(font_name, true))
		{
			return font_meta;
		}
	}
	return nullptr;
}
bool AssetsManager::AddEventListener(const String& type, Object *object, const EventHandler &selector)
{
	return _eventController.AddEventListener(type, object, selector);
}
bool AssetsManager::RemoveEventListener(const String& type, Object *object, const EventHandler &selector)
{
	return _eventController.RemoveEventListener(type, object, selector);
}
void AssetsManager::Refresh()
{
	DC_PROFILE_FUNCTION();
	Debuger::Log("AssetsManager::Refresh:%s", Application::GetAssetsPath().c_str());
	if (Application::GetAssetsPath().IsEmpty())return;

	//_eventController.TriggerEvent(RefreshAssetsEvt);

	for (int i = 0; i < (int)ResourceType::Max; ++i)
	{
		_assetsGroup[(ResourceType)i].Clear();
	}

	_shaderGroup.Files.Clear();
	_shaderGroup.Folders.Clear();
	_guid2Metas.Clear();
	_file2Metas.Clear();
	SAFE_DELETE(_assetsFile);

	FileInfo file_info(Application::GetAssetsPath(), SearchOption::AllDirectories);
	_assetsFile = AssetMeta::Create(ResourceType::Undefined);
	RefreshAsset(_assetsFile, file_info, Application::GetAssetsPath(), false);

	//复制内部数据
	for (int i = 0; i < (int)ResourceType::Max; ++i)
	{
		_assetsGroup[(ResourceType)i].AddRange(_internalGroup[(ResourceType)i]);
	}
	//shader分组
	for (const auto& obj : _internalGuid2Metas)
	{
		AssetMeta* meta = obj.second;
		if (meta && meta->_resType == ResourceType::Shader)
		{
			ShaderSerialize* ser = ShaderSerializePools::Get(meta->GetFullPath());
			if (ser && !ser->HideInspector)
			{
				String group = ser->GroupInspector;
				if (group.IsEmpty())group = Path::GetFileNameWithoutExtension(meta->GetFileName());
				AddShaderGroup(group, _shaderGroup, meta->GetGUID());
			}
		}
	}
}
void AssetsManager::RefreshAsset(AssetMeta* asset_meta, const FileInfo& file_info, const String& assets_root_path, bool is_internal)
{
	asset_meta->SetFileInfo(file_info, assets_root_path);
	if (file_info.FileType == FileInfoType::File)
	{
		//meta
		if (!file_info.Extension.Equals("meta", true))
		{
			ResourceType resource_type = asset_meta->GetResType();
			if (resource_type != ResourceType::Undefined)
			{
				InitMeta(asset_meta, file_info, assets_root_path, is_internal);

				//是否隐藏
				bool is_hidden = asset_meta->IsHideInspector();
				//初始化一些特殊资源
				switch (resource_type)
				{
				case ResourceType::Shader:
				{
					ShaderSerialize* ser = ShaderSerializePools::Get(asset_meta->GetFullPath());
					if (ser)
					{
						if (!is_hidden)is_hidden = ser->HideInspector;
						if (!is_internal && !is_hidden)
						{
							String group = ser->GroupInspector;
							if (group.IsEmpty())group = Path::GetFileNameWithoutExtension(asset_meta->GetFileName());
							AddShaderGroup(group, _shaderGroup, asset_meta->GetGUID());
						}
					}
					break;
				}
				case ResourceType::Texture:
				{
					TextureMeta* texture_meta = dynamic_cast<TextureMeta*>(asset_meta);
					if (texture_meta->GetTextureGroup() == TextureGroup::SpriteAtlas)
					{
						if (is_internal)
							_internalGroup[ResourceType::SpriteAtlas].Add(asset_meta);
						else
							_assetsGroup[ResourceType::SpriteAtlas].Add(asset_meta);
					}
					else
					{
						is_hidden = is_internal;//内部纹理不对外使用
					}
					break;
				}
				}
				//加入资源分组
				if (!is_hidden)
				{
					if (is_internal)
						_internalGroup[resource_type].Add(asset_meta);
					else
						_assetsGroup[resource_type].Add(asset_meta);
				}
			}
		}
	}

	if (file_info.FileType == FileInfoType::Dir)
	{
		for (int i = 0; i < asset_meta->GetChildrenCount(); ++i)
		{
			RefreshAsset(asset_meta->GetChildren(i), file_info.GetChildren(i), assets_root_path, is_internal);
		}

		{//删除无效meta文件(如果只有meta文件，没用对应的原始文件，则删除)
			for (int i = asset_meta->GetChildrenCount() - 1; i >= 0; --i)
			{
				const AssetMeta* child_file_info = asset_meta->GetChildren(i);
				if (child_file_info->GetExtension().Equals("meta", true))
				{
					bool is_valid = false;
					String asset_file = Path::GetFileNameWithoutExtension(child_file_info->GetFileName());
					for (int j = 0; j < asset_meta->GetChildrenCount(); ++j)
					{//遍历目录，查看是否有
						const AssetMeta* child_child_file_info = asset_meta->GetChildren(j);
						if (child_child_file_info->GetFileName().Equals(asset_file, true))
						{
							is_valid = true;
							break;
						}
					}
					if (!is_valid)
					{
						File::Delete(Path::Combine(assets_root_path, child_file_info->GetFullPath()));
						asset_meta->DeleteChild(i);
					}
				}
			}
		}
	}
}
void AssetsManager::InitMeta(AssetMeta* asset_meta, const FileInfo& file_info, const String& assets_root_path, bool is_internal)
{
	String meta_relative_path = asset_meta->GetFullPath() + ".meta";
	String meta_path = Path::Combine(assets_root_path, meta_relative_path);
	ResourceType resource_type = Resource::GetResourceType(asset_meta->GetFileName());
	if (!File::Exist(meta_path))
	{
		if (Application::IsEditor())
		{
			File::Create(meta_path);

			String guid = Utility::GUID();
			asset_meta->SetGUID(guid);
			asset_meta->Deserialize();
		}
	}
	else
	{
		asset_meta->Serialize();
	}
	const String& guid = asset_meta->GetGUID();
	uint64 file_hash = asset_meta->GetFullPath().GetHash();
	if (guid.IsEmpty())
	{
		Debuger::Error("The file meta is empty:%s", asset_meta->GetFullPath().c_str());
		//重新生成meta文件
		if (Application::IsEditor() && File::Delete(meta_path))
		{
			return InitMeta(asset_meta, file_info, assets_root_path, is_internal);
		}
	}
	else if (_guid2Metas.Contains(guid) || _internalGuid2Metas.Contains(guid))
	{
		Debuger::Error("The same guid has exist:%s", asset_meta->GetFullPath().c_str());
		//重新生成meta文件
		if (Application::IsEditor() && File::Delete(meta_path))
		{
			return InitMeta(asset_meta, file_info, assets_root_path, is_internal);
		}
	}
	else
	{
		if (is_internal)
		{
			bool result = _internalGuid2Metas.Add(guid, asset_meta);
			if (!result)Debuger::Error("The same guid has exist:%s", asset_meta->GetFullPath().c_str());
			result = _internalFile2Metas.Add(file_hash, asset_meta);
			if (!result)Debuger::Error("The same file has exist:%s", asset_meta->GetFullPath().c_str());
		}
		else
		{
			bool result = _guid2Metas.Add(guid, asset_meta);
			if (!result)Debuger::Error("The same guid has exist:%s", asset_meta->GetFullPath().c_str());
			result = _file2Metas.Add(file_hash, asset_meta);
			if (!result)Debuger::Error("The same file has exist:%s", asset_meta->GetFullPath().c_str());
		}
	}

	//缓存icon
	if (Application::IsEditor())
	{
		EditorIconCache::CacheIcon(asset_meta);
	}
}
void AssetsManager::AddShaderGroup(String group_path, ShaderGroup& shader_group, const String& meta_guid)
{
	if (group_path.IsEmpty())return;

	int index = group_path.IndexOf('/');
	if (index == -1 || index == 0)
	{//只是文件
		shader_group.Files.Add({ group_path, meta_guid});
		return;
	}

	//还有目录
	String folder = group_path.Substring(0, index);
	if (!shader_group.Folders.Contains(folder))
	{
		ShaderGroup group;
		AddShaderGroup(group_path.Substring(index+1), group, meta_guid);
		shader_group.Folders.Add(folder, group);
	}
	else
	{
		AddShaderGroup(group_path.Substring(index + 1), shader_group.Folders[folder], meta_guid);
	}
}
DC_END_NAMESPACE