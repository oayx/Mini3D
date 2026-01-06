 
/*****************************************************************************
* Author： hannibal
* Date：2020/8/29
* Description：把字体碎图拼成一张大图集
*****************************************************************************/
#pragma once

#include "core/Object.h"
#include "Font.h"

DC_BEGIN_NAMESPACE
class Texture;
class FontAtlas;
class Image;
/********************************************************************/
struct FontAtlasInfo
{
	char32_t ch = 0;
	Rect	 rect;
	Rect	 uvRect;	
	int		 bearing_x = 0;
	int		 bearing_y = 0;
	int		 advance_x = 0;
	int		 advance_y = 0;

	float	last_access_time = 0;//TODO:如果贴图超出最大，则使用最近最少使用算法移除
};

class ENGINE_DLL FontAtlasManager final : public Object
{
	friend class Application;
	typedef Map<String, FontAtlas*> Atlases;
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(FontAtlasManager);
	BEGIN_DERIVED_REFECTION_TYPE(FontAtlasManager, Object)
	END_REFECTION_TYPE;

private:
	static void Destroy();

public:
	static bool AddAtlas(const String& name, int textureWidth, int textureHeight, ushort size, bool bold, bool italic);
	static bool RemoveAtlas(const String& name, ushort size, bool bold, bool italic);
	static String GetAtlasKey(const String& name, ushort size, bool bold, bool italic);
	static bool AddFont(const String& name, const String& txt, ushort size, bool bold, bool italic);
	static bool GetFont(const String& name, char32_t ch, ushort size, bool bold, bool italic, FontAtlasInfo& atlas);
	static bool HasFont(const String& name, char32_t ch, ushort size, bool bold, bool italic);
	static bool SetDefaultSize(int w, int h);
	static Texture* GetTexture(const String& name, ushort size, bool bold, bool italic);

private:
	inline static Atlases _atlases;
	inline static int _defaultWidth = DEFAULT_FONT_TEXTURE_SIZE;
	inline static int _defaultHeight = DEFAULT_FONT_TEXTURE_SIZE;
};

//每种字体一张贴图，如果超出一张范围，再另外创建一张
class FontAtlas final : public Object
{
	friend class FontAtlasManager;
	typedef Map<char32_t, FontAtlasInfo> AtlasInfoes;
	DEFAULT_CREATE(FontAtlas);
	FRIEND_CONSTRUCT_DESTRUCT(FontAtlas);
	DISALLOW_COPY_ASSIGN(FontAtlas);
	BEGIN_DERIVED_REFECTION_TYPE(FontAtlas, Object)
	END_REFECTION_TYPE;

private:
	explicit FontAtlas(const String& name, int textureWidth, int textureHeight, ushort size, bool bold, bool italic);
	~FontAtlas();

	bool Add(const String& txt);
	bool Add(const Vector<char32_t>& chs);
	bool Get(char32_t ch, FontAtlasInfo& atlas);
	bool Has(char32_t ch);

	void CreateTexture(int w, int h);
	bool IsTextureFull(const Vector<char32_t>& chs);
	bool IsCurrentTextureFull(int font_w, int font_h);

private:
	String		_name;
	ushort		_size;
	bool		_bold;
	bool		_italic;

	int			_width = 0;
	int			_height = 0;

	const int	OffsetX = 1;			//每个碎图上下空隙
	const int	OffsetY = 1;
	int			_cursorX = 0;			//当前操作位置
	int			_cursorY = 0;			
	int			_maxCurLineHeight = 1;	//当前行最大高度，换行用

	Texture*	_texture = nullptr;
	AtlasInfoes _atlasInfoes;
};
DC_END_NAMESPACE
