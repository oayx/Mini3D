
/*****************************************************************************
* Author： hannibal
* Description：编辑器图标
*****************************************************************************/
#pragma once

#include "runtime/resources/ResourcesDefine.h"

DC_BEGIN_NAMESPACE
class Texture;
class AssetMeta;
/********************************************************************/
class ENGINE_DLL EditorIcon : public Object
{
	friend class EditorIconCache;
	FRIEND_CONSTRUCT_DESTRUCT(EditorIcon);
	DISALLOW_COPY_ASSIGN(EditorIcon);
	BEGIN_DERIVED_REFECTION_TYPE(EditorIcon, Object)
	END_REFECTION_TYPE;

	EditorIcon() {}
	~EditorIcon()
	{
		if (_data)
		{
			Memory::DeleteArray(_data);
			_data = nullptr;
		}
	}
	static EditorIcon* Create(const String& fullPath);//file：完整目录
	static EditorIcon* GetDefaultFileIcon();
	static EditorIcon* GetDefaultFolderIcon();
	static void		   DestroyDefaultIcon();

	bool LoadFromFile(const String& file);

public:
	uint GetWidth()const { return _width; }
	uint GetHeight()const { return _height; }
	uint GetSize()const { return _size; }
	const byte* Data()const { return _data; }

private:
	uint   _width = 0;
	uint   _height = 0;
	uint   _size = 0;
	byte*  _data = nullptr;
	inline static EditorIcon* _defaultFileIcon = nullptr;
	inline static EditorIcon* _defaultFolderIcon = nullptr;
};

/********************************************************************/
class EditorIconCache final : public object
{
	friend class EditorIcon;
	friend class Application;
	typedef Map<String, Texture*> Textures;
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(EditorIconCache);
	BEGIN_REFECTION_TYPE(EditorIconCache)
	END_FINAL_REFECTION_TYPE;

	static void		Destroy();
	static String	GetIconFile(const AssetMeta* meta);

public:
	static void		CacheIcon(const AssetMeta* meta);
	static Texture*	GetTexture(const AssetMeta* meta);

private:
	inline static Textures	_textures;
};
DC_END_NAMESPACE