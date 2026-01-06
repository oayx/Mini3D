 
#include "Font.h"
#include "FontManager.h"
#include "core/image/Image.h"
#include "runtime/resources/Resources.h"
#include "external/freetype/FreeTypeAmalgam.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(Font, Object);
GlyphInfo::~GlyphInfo()
{
	SAFE_RELEASE(data);
}
Font::Font()
{
}
Font::~Font()
{
	if (_font)
	{
		FT_Done_Face((FT_Face)_font);
		_font = nullptr;
	}
	for (auto list_glyphs : _glyphs)
	{
		Map<int, GlyphInfo*> glyphs = list_glyphs.second;
		for (auto glyph : glyphs)
		{
			SAFE_DELETE(glyph.second);
		}
	}
	_glyphs.Clear();
}
Font* Font::GetFont(const String& name)
{
	return FontManager::GetFont(name);
}
Font* Font::GetDefaultFont()
{
	return FontManager::GetDefaultFont();
}
const GlyphInfo* Font::GetGlyph(char32_t c, ushort size, bool bold, bool italic, bool mono)
{
	int size_key = (int)size | (bold ? (1 << 31) : 0) | (italic ? (1 << 30) : 0) | (mono ? (1 << 29) : 0);

	auto p_size_glyphs = _glyphs.Find(c);
	if (p_size_glyphs == _glyphs.end())
	{
		Map<int, GlyphInfo*> size_glyphs;
		p_size_glyphs = _glyphs.Insert(c, size_glyphs).first;
	}

	auto it = p_size_glyphs->second.Find(size_key);
	GlyphInfo* p_glyph = nullptr;
	if (it == p_size_glyphs->second.end())
	{
		p_glyph = GlyphInfo::Create();
		AssertEx(p_size_glyphs->second.Add(size_key, p_glyph), "");
	}
	else
	{
		return it->second;
	}

	p_glyph->c = c;
	p_glyph->size = size;
	p_glyph->bold = bold;
	p_glyph->italic = italic;
	p_glyph->mono = mono;

	FT_Face face = (FT_Face)_font;
	int face_size = (int)size;
	FT_Error error = FT_Set_Char_Size(face, face_size << 6, face_size << 6, 0, 0);

	FT_GlyphSlot slot = face->glyph;
	auto glyphIndex = FT_Get_Char_Index(face, c);
	if (glyphIndex == 0)
	{
		Debuger::Warning("FT_Get_Char_Index:%d", c);
	}
	if (mono)
	{
		error = FT_Load_Char(face, c, FT_LOAD_MONOCHROME | FT_LOAD_TARGET_MONO);
		if (error != 0)
		{
			Debuger::Error("FT_Load_Char error:%d", error);
			return nullptr;
		}
	}
	else
	{
		error = FT_Load_Char(face, c, FT_LOAD_DEFAULT);
		if (error != 0)
		{
			Debuger::Error("FT_Load_Char error:%d", error);
			return nullptr;
		}
	}

	if (bold)
	{
		error = FT_Outline_Embolden(&face->glyph->outline, 1 << 6);
		if (error != 0)
		{
			Debuger::Error("FT_Load_Char error:%d", error);
			return nullptr;
		}
	}

	if (italic)
	{
		float lean = 0.5f;
		FT_Matrix matrix;
		matrix.xx = 1 << 16;
		matrix.xy = (int)(lean * (1 << 16));
		matrix.yx = 0;
		matrix.yy = (1 << 16);
		FT_Outline_Transform(&face->glyph->outline, &matrix);
	}

	if (mono)
	{
		error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_MONO);
		if (error != 0)
		{
			Debuger::Error("FT_Load_Char error:%d", error);
			return nullptr;
		}
	}
	else
	{
		error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
		if (error != 0)
		{
			Debuger::Error("FT_Load_Char error:%d", error);
			return nullptr;
		}
	}

	p_glyph->glyphIndex = glyphIndex;
	p_glyph->width = slot->bitmap.width;
	p_glyph->height = slot->bitmap.rows;
	p_glyph->bearing_x = slot->bitmap_left;
	p_glyph->bearing_y = slot->bitmap_top;
	p_glyph->advance_x = (int)(slot->advance.x >> 6);
	p_glyph->advance_y = (int)(slot->advance.y >> 6);
	if (p_glyph->width > 0 && p_glyph->height > 0)
	{
		Image* image = Image::Create(ColorFormat::R8G8B8A8, iSize(p_glyph->width, p_glyph->height));
		byte* pixels = image->Lock();

		if (mono || slot->bitmap.pixel_mode == FT_PIXEL_MODE_MONO)
		{
			for (int i = 0; i < p_glyph->height; ++i)
			{
				for (int j = 0; j < p_glyph->width; ++j)
				{
					unsigned char bit = slot->bitmap.buffer[i * slot->bitmap.pitch + j / 8] & (0x1 << (7 - j % 8));
					bit = bit == 0 ? 0 : 255;

					pixels[i * p_glyph->width * 4 + j * 4 + 0] = 255;
					pixels[i * p_glyph->width * 4 + j * 4 + 1] = 255;
					pixels[i * p_glyph->width * 4 + j * 4 + 2] = 255;
					pixels[i * p_glyph->width * 4 + j * 4 + 3] = bit;
				}
			}
		}
		else
		{
			for (int i = 0; i < p_glyph->height; ++i)
			{
				for (int j = 0; j < p_glyph->width; ++j)
				{
					unsigned char alpha = slot->bitmap.buffer[i * slot->bitmap.pitch + j];

					pixels[i * p_glyph->width * 4 + j * 4 + 0] = 255;
					pixels[i * p_glyph->width * 4 + j * 4 + 1] = 255;
					pixels[i * p_glyph->width * 4 + j * 4 + 2] = 255;
					pixels[i * p_glyph->width * 4 + j * 4 + 3] = alpha;
				}
			}
		}
		image->Unlock();
		p_glyph->SetImageData(image);
		//image->SaveToFile("font1.png");
	}
	else
	{//space
		p_glyph->width = 1;
		p_glyph->height = 1;
		Image* image = Image::Create(ColorFormat::R8G8B8A8, iSize(p_glyph->width, p_glyph->height));
		byte* pixels = image->Lock();
		for (int i = 0; i < p_glyph->height; ++i)
		{
			for (int j = 0; j < p_glyph->width; ++j)
			{
				pixels[i * p_glyph->width * 4 + j * 4 + 0] = 255;
				pixels[i * p_glyph->width * 4 + j * 4 + 1] = 255;
				pixels[i * p_glyph->width * 4 + j * 4 + 2] = 255;
				pixels[i * p_glyph->width * 4 + j * 4 + 3] = 0;
			}
		}
		image->Unlock();
		p_glyph->SetImageData(image);
	}
	return p_glyph;
}

bool Font::HasKerning() const
{
	FT_Face face = (FT_Face)_font;
	return FT_HAS_KERNING(face);
}

Vector2 Font::GetKerning(uint previousGlyphIndex, uint glyphIndex)
{
	FT_Face face = (FT_Face)_font;

	FT_Vector kerning;
	FT_Get_Kerning(face, previousGlyphIndex, glyphIndex, FT_KERNING_UNFITTED, &kerning);

	return Vector2((float)(kerning.x >> 6), (float)(kerning.x >> 6));
}
DC_END_NAMESPACE
