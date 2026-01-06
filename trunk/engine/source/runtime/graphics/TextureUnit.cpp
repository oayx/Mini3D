#include "TextureUnit.h"
#include "runtime/graphics/null/Texture.h"
#include "runtime/resources/AssetsManager.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(TextureUnit, Object);
TextureUnit::TextureUnit(TextureType type, const String& name)
	: Name(name), _type(type)
{
}
TextureUnit::TextureUnit(const String& name, Texture* tex)
{
	this->SetTexture(name, tex);
}
TextureUnit::~TextureUnit()
{
	SAFE_RELEASE(_texture);
}
Texture* TextureUnit::GetTexture() 
{ 
	if (_texture == nullptr && !_file.IsEmpty())
	{
		this->LoadTexture();
	}
	if (_texture == nullptr)
	{
		this->SetTexture(this->Name, Texture::GetWhiteTexture());
	}
	return _texture; 
}
void TextureUnit::SetTexture(const String& shaderName, Texture* tex)
{
	if (tex == _texture)return;

	SAFE_RELEASE(_texture);
	this->Name = shaderName;
	this->_texture = tex;
	if (tex != nullptr)
	{
		tex->Retain();
		this->_file = tex->GetResFile();
		this->_type = tex->GetTextureType();
		this->AddressMode = tex->GetAddressMode();
		this->BorderColor = tex->GetBorderColor();
		this->FilterType = tex->GetFilterType();
		this->Anisotropy = tex->GetAnisotropy();
		this->_flags |= tex->IsSRGB() ? TextureFlag::sRGB : TextureFlag::NONE;
		this->_flags |= tex->EnableMips() ? TextureFlag::MipMap : TextureFlag::NONE;
	}
	else
	{
		this->_file = "";
		this->_type = TextureType::None;
		this->_flags = TextureFlag::NONE;
	}
}
void TextureUnit::SetTexture(const String& file, TextureType type, TextureFlag flags)
{
	SAFE_RELEASE(_texture);
	_file = file; 
	_type = type; 
	_flags = flags;
}
void TextureUnit::SetTexture(const ShaderTexture& info)
{
	SAFE_RELEASE(_texture);
	this->_file = info.File;
	this->_type = info.Type;
	this->TextureScale = info.Scale;
	this->TextureOffset = info.Offset;
	TextureMeta* meta = dynamic_cast<TextureMeta*>(AssetsManager::GetMetaByFile(info.File));
	if (meta)
	{
		this->_type = meta->GetTextureType();
		this->AddressMode = meta->GetAddressMode();
		this->BorderColor = meta->GetBorderColor();
		this->FilterType = meta->GetFilterType();
		this->Anisotropy = meta->GetAnisotropy();
		this->_flags |= meta->IssRGB() ? TextureFlag::sRGB : TextureFlag::NONE;
		this->_flags |= meta->IsGenerateMipMaps() ? TextureFlag::MipMap : TextureFlag::NONE;
		this->EnableReadWrite = meta->IsEnableReadWrite();
	}
}
void TextureUnit::LoadTexture()
{
	DC_PROFILE_FUNCTION;
	SAFE_RELEASE(_texture);
	if (!this->_file.IsEmpty())
	{
		TextureDesc desc;
		desc.scale = this->TextureScale; desc.offset = this->TextureOffset;
		desc.filter = this->FilterType; desc.anisotropy = this->Anisotropy;
		desc.address = this->AddressMode;
		desc.border = this->BorderColor;
		desc.usage = GPUResourceUsage::Static; 
		desc.flags = _flags;
		desc.flags |= (this->FilterType == TextureFilter::Trilinear ? TextureFlag::MipMap : TextureFlag::NONE);
		switch (this->_type)
		{
		case TextureType::D2:
			_texture = Texture::Create2D(this->_file, desc);
			break;
		case TextureType::Cube:
			_texture = Texture::CreateCube(this->_file, desc);
			break;
		}
		if (_texture != nullptr)
		{
			_texture->Retain();
		}
	}
}
DC_END_NAMESPACE