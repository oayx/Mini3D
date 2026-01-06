
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
class ENGINE_DLL AssetsManager final : public Object
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
	static void			RefreshAsset(AssetMeta* assetMeta, const FileInfo& fileInfo, const String& assetsRootPath, bool isInternal);
	static void			InitMeta(AssetMeta* assetMeta, const FileInfo& fileInfo, const String& assetsRootPath, bool isInternal);
	static void			AddShaderGroup(String groupPath, ShaderGroup& shaderGroup, const String& metaGuid);

private:
	inline static EventController				_eventController;

	inline static AssetMeta*					_internalFile = nullptr;
	inline static Map<String, AssetMeta*>		_internalGuid2Metas;
	inline static HashMap<uint64, AssetMeta*>	_internalFile2Metas;

	inline static AssetMeta*					_assetsFile = nullptr;
	inline static Map<String, AssetMeta*>		_guid2Metas;
	inline static HashMap<uint64, AssetMeta*>	_file2Metas;
	inline static ShaderGroup					_shaderGroup;

	inline static HashMap<ResourceType, Vector<AssetMeta*>>	_internalGroup;	//value->meta guid
	inline static HashMap<ResourceType, Vector<AssetMeta*>>	_assetsGroup;		//value->meta guid
};
DC_END_NAMESPACE