 
/*****************************************************************************
* Author： hannibal
* Date：2009/11/21
* Description： 纹理
*****************************************************************************/
#pragma once

#include "core/image/Image.h"
#include "core/stream/DataStream.h"
#include "runtime/CommonDefine.h"
#include "runtime/graphics/RenderDefine.h"
#include "runtime/resources/Resources.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
// ClassName：纹理
// Description：Texture
class ENGINE_DLL Texture : public Resource
{
	friend class RenderContent;
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(Texture);
	BEGIN_DERIVED_REFECTION_TYPE(Texture, Resource)
	END_DERIVED_REFECTION_TYPE;

protected:
	Texture(const TextureDesc& desc);
public:
	~Texture()
	{
		if (_imageData)
		{
			DeleteArray(_imageData); _imageData = nullptr;
		}
	}
public:
	static Texture* Create(const TextureDesc& desc);
	static Texture* Create(const String &file);
	static Texture* Create2D(const String &file, const TextureDesc& desc);
	static Texture* CreateCube(const String &file, const TextureDesc& desc);
	static Texture* CreateFromMemroy(const byte* data, const TextureDesc& desc);
	static Texture* CreateFromImage(Image* image, const TextureDesc& desc);

	static Texture* GetWhiteTexture();
	static Texture* GetBlackTexture();

public:
	virtual bool	Fill(Image* image) { AssertEx(false, "unknow interface"); return false; }

	virtual byte*	Lock(TextureLockDesc& lock_desc) = 0;
	virtual void	Unlock(const TextureLockDesc& lock_desc) = 0;
	virtual bool	GetData(MemoryDataStream& stream) { return false; }

	virtual bool	Copy(Texture* dst) = 0;

	virtual void*	GetTexture() = 0;
	virtual void*	GetTextureView() { return nullptr; }
	virtual void*	GetTextureSample() { return nullptr; }

	virtual void	SaveToFile(const String& name, ImageType type) = 0;

public:
	TextureType		GetTextureType()const { return _textureType; }
	ColorFormat		GetFormat()const { return _imageFormat; }
	int				GetWidth()const { return _imageWidth; }
	int				GetHeight()const { return _imageHeight; }
	uint			GetBytes()const;
	uint			GetPitch()const;

	TextureAddress	GetAddressMode()const { return _addressMode; }
	const Color&	GetBorderColor()const { return _borderColor; }
	TextureFilter	GetFilterType()const { return _filterType; }
	uint			GetAnisotropy()const { return _anisotropy; }

	bool			IsSRGB()const { return _flags & TextureFlag::sRGB; }
	bool			EnableMips()const { return _enableMips; }
	bool			IsDynamic()const { return _usage & GPUResourceUsage::Dynamic; }
	bool			IsStaging()const { return _usage & GPUResourceUsage::Staging; }
	bool			IsRenderTexture()const { return _flags & TextureFlag::COLOR || _flags & TextureFlag::DEPTH || _flags & TextureFlag::STENCIL; }

protected:
	TextureType		_textureType = TextureType::D2;
	ColorFormat		_imageFormat = ColorFormat::R8G8B8A8;
	int				_imageWidth = 0;
	int				_imageHeight = 0;
	bool			_enableMips = false;	//是否开启mipmap
	TextureFilter	_filterType = TextureFilter::Bilinear;
	uint			_anisotropy = 1;
	byte			_antiAlias = 1;		//抗锯齿
	TextureAddress	_addressMode = TextureAddress::Clamp;
	Color			_borderColor = Color::Black;
	GPUResourceUsage _usage = GPUResourceUsage::Static;
	TextureFlag		_flags = TextureFlag::NONE;

	byte*			_imageData = nullptr;
	uint			_totalBytes = 0;
	uint			_rowPitch = 0;

private:
	static Texture*	_whiteTexture;
	static Texture*	_blackTexture;
};//Texture
DC_END_NAMESPACE
