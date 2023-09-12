
/*****************************************************************************
* Author： hannibal
* Date：2020/8/29
* Description：
*****************************************************************************/
#pragma once
 
#include "core/Object.h"

DC_BEGIN_NAMESPACE
class Texture;
class UIAtlas;
/********************************************************************/
struct UIAtlasInfo
{
	String	name;
	Rect	rect;
	Rect	uv_rect;
};
/********************************************************************/
class ENGINE_DLL UIAtlasManager Final : public object
{
	friend class Application;
	typedef Map<String, UIAtlas*> Atlases;
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(UIAtlasManager);
	BEGIN_REFECTION_TYPE(UIAtlasManager)
	END_FINAL_REFECTION_TYPE;

private:
	static void Destroy();

public:
	static bool LoadAtlas(const String& atlas_name);
	static bool UnloadAtlas(const String& atlas_name);
	static bool GetAtlasInfo(const String& atlas_name, const String& name, UIAtlasInfo& atlas);
	static bool HasAtlasInfo(const String& atlas_name, const String& name);
	static bool IsAtlasFile(const String& atlas_name);

	static UIAtlas* GetAtlas(const String& atlas_name);
	static String	GetAtlasJsonFile(const String& atlas_name);
	static Texture* GetAtlasTexture(const String& atlas_name);


private:
	static Atlases _atlases;
};

class ENGINE_DLL UIAtlas Final : public object
{
	friend class UIAtlasManager;
	typedef Map<String, UIAtlasInfo> AtlasInfoes;
	DEFAULT_CREATE(UIAtlas);
	FRIEND_CONSTRUCT_DESTRUCT(UIAtlas);
	DISALLOW_COPY_ASSIGN(UIAtlas);
	BEGIN_REFECTION_TYPE(UIAtlas)
	END_FINAL_REFECTION_TYPE;
	
private:
	explicit UIAtlas();
	~UIAtlas();

	bool Parse(const String& file);
	bool Get(const String& name, UIAtlasInfo& atlas);
	bool Has(const String& name);
	Texture* GetTexture() const { return _texture; }

public:
	const AtlasInfoes& GetAtlasInfoes()const { return _atlasInfoes; }

private:
	Texture*	_texture = nullptr;
	AtlasInfoes _atlasInfoes;
};
DC_END_NAMESPACE
