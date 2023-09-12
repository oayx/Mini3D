 #include "FontManager.h"
#include "Font.h"
#include "runtime/resources/AssetsManager.h"
#include "runtime/resources/Resources.h"
#include "external/freetype/FreeTypeAmalgam.h"
//#include <ft2build.h>
//#include FT_FREETYPE_H

DC_BEGIN_NAMESPACE
/********************************************************************/
static FT_Library g_ft_lib;
String FontManager::_defaultFont = "msyh";
FontManager::Fonts FontManager::_fonts;
IMPL_REFECTION_TYPE(FontManager);
void FontManager::Initialize()
{
	FT_Error err = FT_Init_FreeType(&g_ft_lib);
	if (err)
	{
		Debuger::Error("FT_Init_FreeType error:%d", err);
	}
	else
	{
		Debuger::Log("freetype init success");
	}
}
void FontManager::Destroy()
{
	DestroyFont();
	FT_Error err = FT_Done_FreeType(g_ft_lib);
	if (err)
	{
		Debuger::Error("FT_Done_FreeType error:%d", err);
	}
}
bool FontManager::LoadFont(const String& name, const String& file)
{
	String full_path = Resource::GetFullDataPath(file);
	String guid = name;
	int font_size = DEFAULT_FONT_SIZE;

	FontMeta* meta = dynamic_cast<FontMeta*>(AssetsManager::GetMetaByFile(file));
	if (meta)
	{
		guid = meta->GetGUID();
		font_size = meta->GetFontSize();
	}

	if (_fonts.Contains(guid))
	{
		Debuger::Warning("FontManager::LoadFont font exist: %s", name.c_str());
		return false;
	}

	if (!File::Exist(full_path))
	{
		Debuger::Error("FontManager::LoadFont font file not exist: %s", full_path.c_str());
		return false;
	}

	FT_Face face;
	FT_Error err = FT_New_Face(g_ft_lib, full_path.c_str(), 0, &face);
	if (err)
	{
		Debuger::Error("FontManager::LoadFont FT_New_Memory_Face error: %d", err);
	}
	if (FT_Select_Charmap(face, FT_ENCODING_UNICODE))
	{
		int foundIndex = -1;
		for (int charmapIndex = 0; charmapIndex < face->num_charmaps; charmapIndex++)
		{
			if (face->charmaps[charmapIndex]->encoding != FT_ENCODING_NONE)
			{
				foundIndex = charmapIndex;
				break;
			}
		}

		if (foundIndex == -1)
		{
			return false;
		}

		if (FT_Select_Charmap(face, face->charmaps[foundIndex]->encoding))
		{
			return false;
		}
	}

	int dpi = 72;
	int fontSizePoints = 64 * font_size;
	if (FT_Set_Char_Size(
		face, 
		fontSizePoints,		//字符宽度,单位为1/64点
		fontSizePoints,		//字符高度,单位为1/64点
		dpi,				//水平分辨率
		dpi))				//垂直分辨率
		return false;

	Font* font = Font::Create();
	font->_font = (void*)face;
	_fonts.Add(guid, font);

	Debuger::Log("freetype add font succeed, name:%s, file:%s", name.c_str(), file.c_str());
	return true;
}
Font* FontManager::GetFont(const String& name)
{
	String guid = name;
	FontMeta* meta = AssetsManager::GetFontMeta(name);
	if (meta)
	{
		guid = meta->GetGUID();
	}

	Fonts::const_iterator it = _fonts.Find(guid);
	if (it != _fonts.end())
	{
		return it->second;
	}
	else if(name == _defaultFont && !_defaultFont.IsEmpty())
	{
		if (LoadFont(_defaultFont, String::Format("internal/font/{0}.ttf", _defaultFont.c_str())))
		{
			return GetFont(name);
		}
	}
	else
	{
		String guid;
		if (meta && LoadFont(name, meta->GetFullPath()))
		{
			return GetFont(name);
		}
	}
	return nullptr;
}
Font* FontManager::GetDefaultFont()
{
	return GetFont(_defaultFont);
}
void FontManager::DestroyFont(const String& name)
{
	String guid = name;
	FontMeta* meta = AssetsManager::GetFontMeta(name);
	if (meta)
	{
		guid = meta->GetGUID();
	}

	Fonts::iterator it = _fonts.Find(guid);
	if (it != _fonts.end())
	{
		SAFE_DELETE(it->second);
		_fonts.Remove(it);
	}
}
void FontManager::DestroyFont()
{
	for (auto obj : _fonts)
	{
		SAFE_DELETE(obj.second);
	}
	_fonts.Clear();
}
DC_END_NAMESPACE
