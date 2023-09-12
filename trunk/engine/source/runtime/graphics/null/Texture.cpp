#include "Texture.h"
#include "runtime/resources/AssetsManager.h"
#include "runtime/Application.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
Texture* Texture::_whiteTexture = nullptr;
Texture* Texture::_blackTexture = nullptr;
IMPL_DERIVED_REFECTION_TYPE(Texture, Resource);
Texture::Texture(const TextureDesc& desc)
	: base(ResourceType::Texture), _textureType(desc.type), _imageWidth(desc.width), _imageHeight(desc.height), _imageFormat(desc.format)
	, _filterType(desc.filter), _anisotropy(desc.anisotropy), _antiAlias(desc.antiAlias), _addressMode(desc.address), _borderColor(desc.border), _usage(desc.usage), _flags(desc.flags)
{
	this->_antiAlias = _antiAlias < 1 ? 1 : _antiAlias;
	this->_enableMips = _flags & TextureFlag::MipMap;
	if (_antiAlias > 1)
	{
		_enableMips = false;
	}
}
Texture* Texture::Create(const TextureDesc& desc)
{
	DC_PROFILE_FUNCTION();

	Texture* tex = Application::GetGraphics()->CreateTexture(desc);
	if (tex != nullptr)
	{
		tex->AutoRelease();
	}
	return tex;
}
Texture* Texture::Create(const String &file)
{
	DC_PROFILE_FUNCTION();

	if (file.IsEmpty())return nullptr;
	TextureDesc desc;
	TextureMeta* meta = dynamic_cast<TextureMeta*>(AssetsManager::GetMetaByFile(file));
	if (meta)
	{
		desc.type = meta->GetTextureType();
		desc.address = meta->GetAddressMode();
		desc.filter = meta->GetFilterType();
		desc.anisotropy = meta->GetAnisotropy();
		desc.flags |= meta->IssRGB() ? TextureFlag::sRGB : TextureFlag::NONE;
		desc.flags |= meta->IsGenerateMipMaps() ? TextureFlag::MipMap : TextureFlag::NONE;
	}

	Texture* texture = nullptr;
	switch (desc.type)
	{
	case TextureType::D2:
		texture = Create2D(file, desc);
		break;
	case TextureType::Cube:
		texture = CreateCube(file, desc);
		break;
	}
	return texture;
}
Texture* Texture::Create2D(const String &file, const TextureDesc& desc)
{
	DC_PROFILE_FUNCTION();

	if (file.IsEmpty())return nullptr;
	CHECK_RETURN_PTR_NULL(desc.width <= Application::GetGraphics()->GetCaps()->maxTextureSize);
	CHECK_RETURN_PTR_NULL(desc.height <= Application::GetGraphics()->GetCaps()->maxTextureSize);
	Image* image = Image::Create(file, desc.flags & TextureFlag::MipMap);
	if (image)
	{
		Texture* tex = CreateFromImage(image, desc);
		tex->SetResFile(file);
		return tex;
	}
	else
	{
		Debuger::Error("texture data load error:%s", file.c_str());
		return nullptr;
	}
}
Texture* Texture::CreateCube(const String &file, const TextureDesc& desc)
{
	DC_PROFILE_FUNCTION();
	
	if (file.IsEmpty())return nullptr;
	CHECK_RETURN_PTR_NULL(desc.width == desc.height);
	CHECK_RETURN_PTR_NULL(desc.width <= Application::GetGraphics()->GetCaps()->maxCubeMapSize);
	String file_path = Resource::GetFullDataPath(file);
	Image* image = Image::Create(file, desc.flags & TextureFlag::MipMap);
	if (image)
	{
		Texture* tex = CreateFromImage(image, desc);
		tex->SetResFile(file);
		return tex;
	}
	else
	{
		Debuger::Error("texture data load error:%s", file_path.c_str());
		return nullptr;
	}
}
Texture* Texture::CreateFromMemroy(const byte* data, const TextureDesc& desc)
{
	DC_PROFILE_FUNCTION();

	CHECK_RETURN_PTR_NULL(data);

	Image* image = Image::Create(desc.format, iSize(desc.width, desc.height));
	image->Fill(data, image->GetSize());
	return CreateFromImage(image, desc);
}
Texture* Texture::CreateFromImage(Image* image, const TextureDesc& desc)
{
	DC_PROFILE_FUNCTION();

	CHECK_RETURN_PTR_NULL(image);
	CHECK_RETURN_PTR_NULL(image->GetWidth() <= Application::GetGraphics()->GetCaps()->maxTextureSize);
	CHECK_RETURN_PTR_NULL(image->GetHeight() <= Application::GetGraphics()->GetCaps()->maxTextureSize);

	TextureType type = image->IsCube() ? TextureType::Cube : TextureType::D2;
	TextureDesc sd;
	sd.type = type; sd.width = image->GetWidth(); sd.height = image->GetHeight(); sd.format = image->GetFormat(); 
	sd.address = desc.address; sd.filter = desc.filter; sd.usage = desc.usage; sd.flags = desc.flags;
	Texture* tex = Create(sd);
	if (tex != nullptr)
	{
		tex->Fill(image);
	}
	return tex;
}
Texture* Texture::GetWhiteTexture()
{
	if (_whiteTexture == nullptr)
	{
		byte* by = NewArray<byte>(4 * 9);
		for (int i = 0; i < 9; ++i)
		{
			by[i * 4 + 0] = 255;
			by[i * 4 + 1] = 255;
			by[i * 4 + 2] = 255;
			by[i * 4 + 3] = 255;
		}	
		Image* image = Image::Create(ColorFormat::R8G8B8A8, iSize(3, 3));
		image->Fill(by, image->GetSize());
		TextureDesc desc;
		desc.type = TextureType::D2; desc.address = TextureAddress::Wrap; desc.filter = TextureFilter::Nearest;
		_whiteTexture = CreateFromImage(image, desc);
		_whiteTexture->Retain();//防止自动释放
		_whiteTexture->SetResName("WhiteTexture");
		DeleteArray(by);
	}
	return _whiteTexture;
}
Texture* Texture::GetBlackTexture()
{
	if (_blackTexture == nullptr)
	{
		byte* by = NewArray<byte>(4 * 9);
		for (int i = 0; i < 9; ++i)
		{
			by[i * 4 + 0] = 0;
			by[i * 4 + 1] = 0;
			by[i * 4 + 2] = 0;
			by[i * 4 + 3] = 255;
		}
		Image* image = Image::Create(ColorFormat::R8G8B8A8, iSize(3, 3));
		image->Fill(by, image->GetSize());
		TextureDesc desc;
		desc.type = TextureType::D2; desc.address = TextureAddress::Wrap; desc.filter = TextureFilter::Nearest;
		_blackTexture = CreateFromImage(image, desc);
		_blackTexture->Retain();//防止自动释放
		_blackTexture->SetResName("BlackTexture");
		DeleteArray(by);
	}
	return _blackTexture;
}
uint Texture::GetBytes()const 
{ 
	if(_totalBytes == 0)
		return _imageHeight * this->GetPitch();
	else
		return _totalBytes;
}
uint Texture::GetPitch()const 
{ 
	if (_rowPitch == 0)
		return BytesRowPitch(_imageFormat, _imageWidth);
	else
		return _rowPitch;
}
DC_END_NAMESPACE