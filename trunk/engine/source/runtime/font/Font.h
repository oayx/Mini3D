 
/*****************************************************************************************************/
// @author hannibal
// @date   2020/08/11
// @brief  字体
// Font glyph metrics:
//
//                       xmin                     xmax
//                        |                         |
//                        |<-------- width -------->|
//                        |                         |
//              |         +-------------------------+----------------- ymax
//              |         |    ggggggggg   ggggg    |     ^        ^
//              |         |   g:::::::::ggg::::g    |     |        |
//              |         |  g:::::::::::::::::g    |     |        |
//              |         | g::::::ggggg::::::gg    |     |        |
//              |         | g:::::g     g:::::g     |     |        |
//    offsetX  -|-------->| g:::::g     g:::::g     |  offsetY     |
//              |         | g:::::g     g:::::g     |     |        |
//              |         | g::::::g    g:::::g     |     |        |
//              |         | g:::::::ggggg:::::g     |     |        |
//              |         |  g::::::::::::::::g     |     |      height
//              |         |   gg::::::::::::::g     |     |        |
//  baseline ---*---------|---- gggggggg::::::g-----*--------      |
//            / |         |             g:::::g     |              |
//     origin   |         | gggggg      g:::::g     |              |
//              |         | g:::::gg   gg:::::g     |              |
//              |         |  g::::::ggg:::::::g     |              |
//              |         |   gg:::::::::::::g      |              |
//              |         |     ggg::::::ggg        |              |
//              |         |         gggggg          |              v
//              |         +-------------------------+----------------- ymin
//              |                                   |
//              |------------- advanceX ----------->|

/*****************************************************************************************************/
#pragma once

#include "core/image/Image.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
enum
{
	MAX_FONT_SIZE = 300,
	DEFAULT_FONT_SIZE = 20,
	DEFAULT_FONT_TEXTURE_SIZE = 256,
	MIN_FONT_TEXTURE_SIZE = 64,
	MAX_FONT_TEXTURE_SIZE = 4096,
};

class ENGINE_DLL GlyphInfo Final : public object
{
	friend class Font;
	DEFAULT_CREATE(GlyphInfo);
	FRIEND_CONSTRUCT_DESTRUCT(GlyphInfo);
	~GlyphInfo();
public:
	char32_t c = 0;
	ushort size = 0;
	uint glyph_index = 0;
	int width = 0;
	int height = 0;
	int bearing_x = 0;
	int bearing_y = 0;
	int advance_x = 0;
	int advance_y = 0;
	bool bold = false;
	bool italic = false;
	bool mono = false;
	Image* data = nullptr;
	void SetImageData(Image* d)
	{
		data = d;
		data->Retain();
	}
};

class ENGINE_DLL Font Final : public Object
{
	friend class FontManager;
	DEFAULT_CREATE(Font);
	FRIEND_CONSTRUCT_DESTRUCT(Font);
	DISALLOW_COPY_ASSIGN(Font);
	BEGIN_DERIVED_REFECTION_TYPE(Font, Object)
	END_DERIVED_REFECTION_TYPE;

private:
	Font();
	virtual ~Font();
public:
	static Font* GetFont(const String& name);
	static Font* GetDefaultFont();

	const GlyphInfo* GetGlyph(char32_t c, ushort size, bool bold, bool italic, bool mono);
	bool HasKerning() const;
	Vector2 GetKerning(uint previous_glyph_index, uint glyph_index);

private:
	void* _font = nullptr;
	Map<char32_t, Map<int, GlyphInfo*>> _glyphs;
};
DC_END_NAMESPACE
