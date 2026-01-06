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
	DC_PROFILE_FUNCTION;
	String fullPath = Resource::GetFullDataPath(file);
	String guid = name;
	int fontSize = DEFAULT_FONT_SIZE;

	FontMeta* meta = dynamic_cast<FontMeta*>(AssetsManager::GetMetaByFile(file));
	if (meta)
	{
		guid = meta->GetGUID();
		fontSize = meta->GetFontSize();
	}

	if (_fonts.Contains(guid))
	{
		Debuger::Warning("FontManager::LoadFont font exist: %s", name.c_str());
		return false;
	}

	if (!File::Exist(fullPath))
	{
		Debuger::Error("FontManager::LoadFont font file not exist: %s", fullPath.c_str());
		return false;
	}

	FT_Face face;
	FT_Error err = FT_New_Face(g_ft_lib, fullPath.c_str(), 0, &face);
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
	int fontSizePoints = 64 * fontSize;
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
		if (LoadFont(_defaultFont, "internal/font/" + _defaultFont + ".ttf"))
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
