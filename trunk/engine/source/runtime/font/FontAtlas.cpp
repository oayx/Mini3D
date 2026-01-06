#include "FontAtlas.h"
#include "Font.h"
#include "core/image/Image.h"
#include "core/time/Time.h"
#include "runtime/font/Font.h"
#include "runtime/font/FontManager.h"
#include "runtime/graphics/null/Texture.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(FontAtlasManager, Object);
void FontAtlasManager::Destroy()
{
	for (auto atlas : _atlases)
	{
		SAFE_DELETE(atlas.second);
	}
	_atlases.Clear();
}
bool FontAtlasManager::AddAtlas(const String& name, int textureWidth, int textureHeight, ushort size, bool bold, bool italic)
{
	if (!Math::Is2Power(textureWidth) || !Math::Is2Power(textureHeight))
		return false;
	String key = GetAtlasKey(name, size, bold, italic);
	auto it = _atlases.Find(key);
	if (it != _atlases.end())
	{
		Debuger::Error("the atlas already exists:%s", name.c_str());
		return false;
	}
	FontAtlas* atlas = FontAtlas::Create(name, textureWidth, textureHeight, size, bold, italic);
	AssertEx(_atlases.Add(key, atlas), "");
	return true;
}
bool FontAtlasManager::RemoveAtlas(const String& name, ushort size, bool bold, bool italic)
{
	String key = GetAtlasKey(name, size, bold, italic);
	auto it = _atlases.Find(key);
	if (it == _atlases.end())
	{
		Debuger::Warning("cannot find atlas:%s", name.c_str());
		return false;
	}
	SAFE_DELETE(it->second);
	auto ret = _atlases.Remove(it);
	return ret->second;
}
String FontAtlasManager::GetAtlasKey(const String& name, ushort size, bool bold, bool italic)
{
	if (size > MAX_FONT_SIZE)size = MAX_FONT_SIZE;
	AssertEx(name.Size() < MAX_FILE_NAME, "name out range:%s", name.c_str());
	char str[MAX_FILE_NAME] = { 0 };
	Snprintf(str, sizeof(str), "%s_%d_%d_%d", name.c_str(), (int)size, (int)bold, (int)italic);
	return String(str);
}
bool FontAtlasManager::AddFont(const String& name, const String& txt, ushort size, bool bold, bool italic)
{
	String key = GetAtlasKey(name, size, bold, italic);
	auto it = _atlases.Find(key);
	if (it == _atlases.end())
	{
		AddAtlas(name, _defaultWidth, _defaultHeight, size, bold, italic);
		it = _atlases.Find(key);
	}
	return it->second->Add(txt);
}
bool FontAtlasManager::GetFont(const String& name, char32_t ch, ushort size, bool bold, bool italic, FontAtlasInfo& atlas)
{
	String key = GetAtlasKey(name, size, bold, italic);
	auto it = _atlases.Find(key);
	if (it == _atlases.end())
	{
		AddAtlas(name, _defaultWidth, _defaultHeight, size, bold, italic);
		it = _atlases.Find(key);
	}
	return it->second->Get(ch, atlas);
}
bool FontAtlasManager::HasFont(const String& name, char32_t ch, ushort size, bool bold, bool italic)
{
	String key = GetAtlasKey(name, size, bold, italic);
	auto it = _atlases.Find(key);
	if (it == _atlases.end())
		return false;
	return it->second->Has(ch);
}
bool FontAtlasManager::SetDefaultSize(int w, int h) 
{ 
	if (!Math::Is2Power(w) || !Math::Is2Power(h))
		return false;
	_defaultWidth = Math::Clamp<int>(w, MIN_FONT_TEXTURE_SIZE, MAX_FONT_TEXTURE_SIZE);
	_defaultHeight = Math::Clamp<int>(w, MIN_FONT_TEXTURE_SIZE, MAX_FONT_TEXTURE_SIZE);
	return true;
}
Texture* FontAtlasManager::GetTexture(const String& name, ushort size, bool bold, bool italic)
{
	String key = GetAtlasKey(name, size, bold, italic);
	auto it = _atlases.Find(key);
	if (it == _atlases.end())
	{
		AddAtlas(name, _defaultWidth, _defaultHeight, size, bold, italic);
		it = _atlases.Find(key);
	}
	return it->second->_texture;
}
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(FontAtlas, Object);
FontAtlas::FontAtlas(const String& name, int textureWidth, int textureHeight, ushort size, bool bold, bool italic)
	: _name(name), _size(size), _bold(bold), _italic(italic), _width(textureWidth), _height(textureHeight)
{
	if (_size > MAX_FONT_SIZE)_size = MAX_FONT_SIZE;
	_width = Math::Clamp<int>(_width, MIN_FONT_TEXTURE_SIZE, MAX_FONT_TEXTURE_SIZE);
	_height = Math::Clamp<int>(_height, MIN_FONT_TEXTURE_SIZE, MAX_FONT_TEXTURE_SIZE);
	CreateTexture(_width, _height);
}
FontAtlas::~FontAtlas()
{
	SAFE_RELEASE(_texture);
	_atlasInfoes.Clear();
}
bool FontAtlas::Add(const String& txt)
{
	auto chs = txt.ToUnicode32();
	return Add(chs);
}
bool FontAtlas::Add(const Vector<char32_t>& chs)
{
	DC_PROFILE_FUNCTION;
	if (chs.IsEmpty())
		return true;

	Font* font = Font::GetFont(_name);
	if (font == nullptr)
	{
		Debuger::Error("cannot find font:%s", _name.c_str());
		return false;
	}
	while (IsTextureFull(chs))
	{//之所以用while，是防止扩充一次还是达不到要求
		if (_width >= MAX_FONT_TEXTURE_SIZE || _height >= MAX_FONT_TEXTURE_SIZE)
		{
			Debuger::Error("font texture is full:%s", _name.c_str());
			return false;
		}
		_width *= 2; _height *= 2;
		CreateTexture(_width, _height);
	}

	TextureLockDesc desc;
	desc.flags = GPUResourceLock::NoOverWrite;
	byte* tex_data = (byte*)_texture->Lock(desc);
	for (const auto& ch : chs)
	{
		if (Has(ch))continue;

		const GlyphInfo* glyph_info = font->GetGlyph(ch, _size, _bold, _italic, false);
		if(glyph_info == nullptr)
			continue;

		Image* image = glyph_info->data;
		int font_w = image->GetWidth();
		int font_h = image->GetHeight();

		//起始位置:由于上一步取Texture，超出当前贴图会自动创建新贴图；所以到这里当前贴图一定能满足大小要求
		if (_cursorX + OffsetX + font_w > _width)
		{//换行
			_cursorX = 0;
			_cursorY = _cursorY + _maxCurLineHeight + OffsetY;
			_maxCurLineHeight = 1;
		}

		FontAtlasInfo info;
		info.ch = ch;
		info.bearing_x = glyph_info->bearing_x;
		info.bearing_y = glyph_info->bearing_y;
		info.advance_x = glyph_info->advance_x;
		info.advance_y = glyph_info->advance_y;
		info.rect = Rect((float)_cursorX, (float)_cursorY, (float)font_w, (float)font_h);
		info.uvRect = Rect((float)_cursorX / (float)_width, (float)_cursorY / (float)_height, (float)font_w / (float)_width, (float)font_h / (float)_height);
		info.last_access_time = Time::GetRealTimeSinceStartup();
		AssertEx(_atlasInfoes.Add(ch, info),"");

		//copy rect data to texture
		const byte* img_data = image->Data();
		for (int row = _cursorY, r = 0; row < _cursorY + font_h; ++row, ++r)
		{
			for (int col = _cursorX, c = 0; col < _cursorX + font_w; ++col, ++c)
			{
				::memcpy(tex_data + (row*desc.pitch + col * 4), img_data + (r*image->GetPitch() + c * 4), 4);
			}
		}

		_maxCurLineHeight = Math::Max<int>(_maxCurLineHeight, font_h);
		_cursorX += font_w + OffsetX;
	}
	_texture->Unlock(desc);
	//_texture->SaveToFile(_texture->GetName() + ".png", ImageType::PNG);
	return true;
}
bool FontAtlas::Get(char32_t ch, FontAtlasInfo& atlas)
{
	auto it = _atlasInfoes.Find(ch);
	if (it == _atlasInfoes.end())
	{
		Add({ ch });
	}

	it = _atlasInfoes.Find(ch);
	if (it == _atlasInfoes.end())
		return false;
	atlas = it->second;
	atlas.last_access_time = Time::GetRealTimeSinceStartup();
	return true;
}
bool FontAtlas::Has(char32_t ch)
{
	auto it = _atlasInfoes.Find(ch);
	if (it == _atlasInfoes.end())
		return false;
	return true;
}
void FontAtlas::CreateTexture(int w, int h)
{
	//备份旧的
	Vector<char32_t> chs;
	chs.Reserve(_atlasInfoes.Size());
	for (const auto& info : _atlasInfoes)
	{
		chs.Add(info.first);
	}
	SAFE_RELEASE(_texture);

	uint pitch = w * 4;
	byte* by = Memory::NewArray<byte>(h * pitch);
	memset(by, 0, h * pitch);

	TextureDesc desc;
	desc.width = w; desc.height = h; desc.format = ColorFormat::R8G8B8A8; 
	desc.usage = GPUResourceUsage::Dynamic; desc.flags = TextureFlag::sRGB;
	_texture = Texture::CreateFromMemroy(by, desc);
	_texture->Retain();
	//_texture->SetName("Font_" + _name + "_" + String::ToString(w) + "_" + String::ToString(h));
	Memory::DeleteArray(by);

	//还原旧的
	_atlasInfoes.Clear();
	_cursorX = _cursorY = 0;
	_maxCurLineHeight = 1;
	Add(chs);
}
bool FontAtlas::IsTextureFull(const Vector<char32_t>& chs)
{
	Font* font = Font::GetFont(_name);
	int cursorX = _cursorX;
	int cursorY = _cursorY;
	int max_line_height = _maxCurLineHeight;
	for (const auto& ch : chs)
	{
		const GlyphInfo* glyph_info = font->GetGlyph(ch, _size, _bold, _italic, false);
		if(glyph_info == nullptr)
			continue;

		Image* image = glyph_info->data;
		int font_w = image->GetWidth();
		int font_h = image->GetHeight();
		if (cursorX + OffsetX + font_w > _width)
		{//换行
			if (cursorY + max_line_height + OffsetY + font_h > _height)
			{
				return true;
			}
		}
		else if (cursorY + font_h > _height)
		{//超过底部
			return true;
		}

		//起始位置:由于上一步取Texture，超出当前贴图会自动创建新贴图；所以到这里当前贴图一定能满足大小要求
		if (cursorX + OffsetX + font_w > _width)
		{//换行
			cursorX = 0;
			cursorY = cursorY + max_line_height + OffsetY;
			max_line_height = 1;
		}

		max_line_height = Math::Max<int>(max_line_height, font_h);
		cursorX += font_w + OffsetX;
	}

	return false;
}
bool FontAtlas::IsCurrentTextureFull(int font_w, int font_h)
{
	bool is_full = false;
	if (_cursorX + OffsetX + font_w > _width)
	{//换行
		if (_cursorY + _maxCurLineHeight + OffsetY + font_h > _height)
		{
			is_full = true;
		}
	}
	else if (_cursorY + font_h > _height)
	{//超过底部
		is_full = true;
	}
	return is_full;
}
DC_END_NAMESPACE
