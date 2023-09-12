 
/*****************************************************************************************************/
// @author hannibal
// @date   2020/08/11
// @brief  字体-管理器
/*****************************************************************************************************/
#pragma once

#include "core/Object.h"

DC_BEGIN_NAMESPACE
class Font;
/********************************************************************/
class FontManager Final : public object
{
	friend class Font;
	friend class Application;
	typedef Map<String, Font*> Fonts;		//guid->font
	typedef Map<String, String> FontNames;	//name->guid
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(FontManager);
	BEGIN_REFECTION_TYPE(FontManager)
	END_FINAL_REFECTION_TYPE;
	
	static void Initialize();
	static void Destroy();

public:
	static bool LoadFont(const String& name, const String& file);
	static Font* GetFont(const String& name);
	static Font* GetDefaultFont();
	static const String& GetDefaultFontName() { return _defaultFont; }
	static void DestroyFont(const String& name);
	static void DestroyFont();

private:
	static String	 _defaultFont;
	static Fonts	 _fonts;
};
DC_END_NAMESPACE
