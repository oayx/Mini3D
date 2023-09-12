
/*****************************************************************************
* Author： hannibal
* Description：asset管理
*****************************************************************************/
#pragma once

#include "AssetMeta.h"
#include "core/event/EventController.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
// shader分组
struct ShaderGroup
{
	List<std::pair<String, String>> Files;	//first:name,second:guid
	Map<String, ShaderGroup> Folders;
};
/********************************************************************/
class ENGINE_DLL AssetsManager Final : public Object
{
	friend class Project;
	friend class Application;
	friend class EditorAppliction;
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(AssetsManager);
	BEGIN_REFECTION_TYPE(AssetsManager)
	END_FINAL_REFECTION_TYPE;

	static void Initialize();
	static void Destroy();

public:
	static Vector<AssetMeta*>&	GetAssetGroup(ResourceType type) { return _assetsGroup[type]; }
	static const		ShaderGroup* GetShaderGroup() { return &_shaderGroup; }

	static void			AddSpriteAtlas(const String& file);
	static void			RemoveSpriteAtlas(const String& file);

	static AssetMeta*	GetRootMeta() { return _assetsFile; }
	static AssetMeta*	GetMeta(const String& guid);
	static AssetMeta*	GetMetaByFile(const String& file);
	static AssetMeta*	GetMetaByPath(const String& path);
	static FontMeta*	GetFontMeta(const String& font_name);

public://事件
	static bool			AddEventListener(const String& type, Object *object, const EventHandler &selector);
	static bool			RemoveEventListener(const String& type, Object *object, const EventHandler &selector);

private:
	static void			Refresh();
	static void			RefreshAsset(AssetMeta* asset_meta, const FileInfo& file_info, const String& assets_root_path, bool is_internal);
	static void			InitMeta(AssetMeta* asset_meta, const FileInfo& file_info, const String& assets_root_path, bool is_internal);
	static void			AddShaderGroup(String group_path, ShaderGroup& shader_group, const String& meta_guid);

private:
	static EventController	_eventController;

	static AssetMeta*					_internalFile;
	static Map<String, AssetMeta*>		_internalGuid2Metas;
	static HashMap<uint64, AssetMeta*>	_internalFile2Metas;

	static AssetMeta*					_assetsFile;
	static Map<String, AssetMeta*>		_guid2Metas;
	static HashMap<uint64, AssetMeta*>	_file2Metas;
	static ShaderGroup					_shaderGroup;

	static HashMap<ResourceType, Vector<AssetMeta*>>	_internalGroup;	//value->meta guid
	static HashMap<ResourceType, Vector<AssetMeta*>>	_assetsGroup;		//value->meta guid
};
DC_END_NAMESPACE