#include "AssetsManager.h"
#include "core/Utility.h"
#include "runtime/graphics/Shader.h"
#include "runtime/resources/Resources.h"
#include "runtime/Application.h"
#include "editor/common/EditorIcon.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(AssetsManager, Object);
void AssetsManager::Initialize()
{
	for (int i = 0; i < (int)ResourceType::Max; ++i)
	{
		_internalGroup.Add((ResourceType)i, Vector<AssetMeta*>());
		_assetsGroup.Add((ResourceType)i, Vector<AssetMeta*>());
	}

#if defined(DC_PLATFORM_WIN32) || defined(DC_PLATFORM_LINUX)
	String fullPath = Resource::GetInternalDataPath();
	Debuger::Log("AssetsManager::Initialize - %s", fullPath.c_str());
	FileInfo fileInfo(fullPath, SearchOption::AllDirectories);
	_internalFile = AssetMeta::Create(ResourceType::Undefined);
	RefreshAsset(_internalFile, fileInfo, fullPath, true);
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

	String filePath = Path::ReplaceSplit(file);
	uint64 hash = filePath.GetHash();
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
	AssetMeta* fileInfo = _assetsFile;
	for (; i < splits.Size(); ++i)
	{
		for (int j = 0; j < fileInfo->GetChildrenCount(); ++j)
		{
			AssetMeta* childFileInfo = fileInfo->GetChildren(j);
			if (childFileInfo->IsFolder() && Path::GetFileNameWithoutExtension(childFileInfo->GetFileName()) == splits[i])
			{
				fileInfo = childFileInfo;
				break;
			}
		}
	}

	if (i == splits.Size())
		return fileInfo;
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
	DC_PROFILE_FUNCTION;
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

	FileInfo fileInfo(Application::GetAssetsPath(), SearchOption::AllDirectories);
	_assetsFile = AssetMeta::Create(ResourceType::Undefined);
	RefreshAsset(_assetsFile, fileInfo, Application::GetAssetsPath(), false);

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
void AssetsManager::RefreshAsset(AssetMeta* assetMeta, const FileInfo& fileInfo, const String& assetsRootPath, bool isInternal)
{
	assetMeta->SetFileInfo(fileInfo, assetsRootPath);
	if (fileInfo.FileType == FileInfoType::File)
	{
		//meta
		if (!fileInfo.Extension.Equals(".meta", true))
		{
			ResourceType resourceType = assetMeta->GetResType();
			if (resourceType != ResourceType::Undefined)
			{
				InitMeta(assetMeta, fileInfo, assetsRootPath, isInternal);

				//是否隐藏
				bool is_hidden = assetMeta->IsHideInspector();
				//初始化一些特殊资源
				switch (resourceType)
				{
				case ResourceType::Shader:
				{
					ShaderSerialize* ser = ShaderSerializePools::Get(assetMeta->GetFullPath());
					if (ser)
					{
						if (!is_hidden)is_hidden = ser->HideInspector;
						if (!isInternal && !is_hidden)
						{
							String group = ser->GroupInspector;
							if (group.IsEmpty())group = Path::GetFileNameWithoutExtension(assetMeta->GetFileName());
							AddShaderGroup(group, _shaderGroup, assetMeta->GetGUID());
						}
					}
					break;
				}
				case ResourceType::Texture:
				{
					TextureMeta* texture_meta = dynamic_cast<TextureMeta*>(assetMeta);
					if (texture_meta->GetTextureGroup() == TextureGroup::SpriteAtlas)
					{
						if (isInternal)
							_internalGroup[ResourceType::SpriteAtlas].Add(assetMeta);
						else
							_assetsGroup[ResourceType::SpriteAtlas].Add(assetMeta);
					}
					else
					{
						is_hidden = isInternal;//内部纹理不对外使用
					}
					break;
				}
				}
				//加入资源分组
				if (!is_hidden)
				{
					if (isInternal)
						_internalGroup[resourceType].Add(assetMeta);
					else
						_assetsGroup[resourceType].Add(assetMeta);
				}
			}
		}
	}

	if (fileInfo.FileType == FileInfoType::Dir)
	{
		for (int i = 0; i < assetMeta->GetChildrenCount(); ++i)
		{
			RefreshAsset(assetMeta->GetChildren(i), fileInfo.GetChildren(i), assetsRootPath, isInternal);
		}

		{//删除无效meta文件(如果只有meta文件，没用对应的原始文件，则删除)
			for (int i = assetMeta->GetChildrenCount() - 1; i >= 0; --i)
			{
				const AssetMeta* childFileInfo = assetMeta->GetChildren(i);
				if (childFileInfo->GetExtension().Equals(".meta", true))
				{
					bool is_valid = false;
					String asset_file = Path::GetFileNameWithoutExtension(childFileInfo->GetFileName());
					for (int j = 0; j < assetMeta->GetChildrenCount(); ++j)
					{//遍历目录，查看是否有
						const AssetMeta* child_child_file_info = assetMeta->GetChildren(j);
						if (child_child_file_info->GetFileName().Equals(asset_file, true))
						{
							is_valid = true;
							break;
						}
					}
					if (!is_valid)
					{
						File::Delete(Path::Combine(assetsRootPath, childFileInfo->GetFullPath()));
						assetMeta->DeleteChild(i);
					}
				}
			}
		}
	}
}
void AssetsManager::InitMeta(AssetMeta* assetMeta, const FileInfo& fileInfo, const String& assetsRootPath, bool isInternal)
{
	String meta_relative_path = assetMeta->GetFullPath() + ".meta";
	String meta_path = Path::Combine(assetsRootPath, meta_relative_path);
	if (!File::Exist(meta_path))
	{
		if (Application::IsEditor())
		{
			File::Create(meta_path);

			String guid = Utility::GUID();
			assetMeta->SetGUID(guid);
			assetMeta->Deserialize();
		}
	}
	else
	{
		assetMeta->Serialize();
	}
	const String& guid = assetMeta->GetGUID();
	uint64 file_hash = assetMeta->GetFullPath().GetHash();
	if (guid.IsEmpty())
	{
		Debuger::Error("The file meta is empty:%s", assetMeta->GetFullPath().c_str());
		//重新生成meta文件
		if (Application::IsEditor() && File::Delete(meta_path))
		{
			return InitMeta(assetMeta, fileInfo, assetsRootPath, isInternal);
		}
	}
	else if (_guid2Metas.Contains(guid) || _internalGuid2Metas.Contains(guid))
	{
		Debuger::Error("The same guid has exist:%s", assetMeta->GetFullPath().c_str());
		//重新生成meta文件
		if (Application::IsEditor() && File::Delete(meta_path))
		{
			return InitMeta(assetMeta, fileInfo, assetsRootPath, isInternal);
		}
	}
	else
	{
		if (isInternal)
		{
			bool result = _internalGuid2Metas.Add(guid, assetMeta);
			if (!result)Debuger::Error("The same guid has exist:%s", assetMeta->GetFullPath().c_str());
			result = _internalFile2Metas.Add(file_hash, assetMeta);
			if (!result)Debuger::Error("The same file has exist:%s", assetMeta->GetFullPath().c_str());
		}
		else
		{
			bool result = _guid2Metas.Add(guid, assetMeta);
			if (!result)Debuger::Error("The same guid has exist:%s", assetMeta->GetFullPath().c_str());
			result = _file2Metas.Add(file_hash, assetMeta);
			if (!result)Debuger::Error("The same file has exist:%s", assetMeta->GetFullPath().c_str());
		}
	}

	//缓存icon
	if (Application::IsEditor())
	{
		EditorIconCache::CacheIcon(assetMeta);
	}
}
void AssetsManager::AddShaderGroup(String groupPath, ShaderGroup& shaderGroup, const String& metaGuid)
{
	if (groupPath.IsEmpty())return;

	int index = groupPath.IndexOf('/');
	if (index == -1 || index == 0)
	{//只是文件
		shaderGroup.Files.Add({ groupPath, metaGuid});
		return;
	}

	//还有目录
	String folder = groupPath.Substring(0, index);
	if (!shaderGroup.Folders.Contains(folder))
	{
		ShaderGroup group;
		AddShaderGroup(groupPath.Substring(index+1), group, metaGuid);
		shaderGroup.Folders.Add(folder, group);
	}
	else
	{
		AddShaderGroup(groupPath.Substring(index + 1), shaderGroup.Folders[folder], metaGuid);
	}
}
DC_END_NAMESPACE