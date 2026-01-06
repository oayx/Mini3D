#include "GLTexture.h"
#include "GLCaps.h"
#include "GLDevice.h"
#include "core/image/Image.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(GLTexture, Texture);
GLTexture::GLTexture(const TextureDesc& desc)
	: base(desc)
{
	AssertEx(_textureType == TextureType::D2 || _textureType == TextureType::Cube, "");
	GLenum texture_type = GL_GetTextureType(_textureType, _antiAlias > 1);
	
	GL_ERROR(glGenTextures(1, &_colorTexture));
	GL_ERROR(glBindTexture(texture_type, _colorTexture));

#if defined(GL_GENERATE_MIPMAP)
	if(_enableMips)GL_ERROR(glTexParameteri(texture_type, GL_GENERATE_MIPMAP, GL_TRUE));
#endif

	GL_ERROR(glBindTexture(texture_type, 0));
}
GLTexture::~GLTexture()
{
	if (_colorTexture > 0)
	{
		GL_ERROR(glDeleteTextures(1, &_colorTexture));
		_colorTexture = 0;
	}
}
bool GLTexture::Fill(Image* image)
{
	GLenum texture_type = GL_GetTextureType(_textureType, _antiAlias > 1);
	GL_ERROR(glBindTexture(texture_type, _colorTexture));
	for (int i = 0; i < (int)image->MipLevels(); ++i)
	{
		ImageMipData* mip_data = image->GetMipData(i);
		_totalBytes = mip_data->Size;
		_rowPitch = mip_data->RowPitch;
		UploadTexture(mip_data->Width, mip_data->Height, mip_data, i);

		if (_textureType == TextureType::D2 && !_enableMips)break;
	}

	if (_enableMips && image->MipLevels() == 1 && GetGraphicsCaps()->hasAutoMipMapGen)
	{//自动计算
		GL_ERROR(glGenerateMipmap(GL_GetTextureType(_textureType, _antiAlias > 1)));
	}
	GL_ERROR(glBindTexture(texture_type, 0));
	return true;
}
bool GLTexture::UploadTexture(int w, int h, ImageMipData* mip_image, int idx)
{
	//生成纹理数据
	GLenum color_format = GL_GetTextureFormat(_imageFormat);
	GLenum internal_format = GL_GetInternalFormat(_imageFormat, this->IsSRGB());
	bool is_compressed = IsCompressedFormat(_imageFormat);
	const byte* data = mip_image->Data;
	if (is_compressed)
	{
		if (_textureType == TextureType::D2)
			GL_ERROR(glCompressedTexImage2D(GL_TEXTURE_2D, idx, internal_format, w, h, 0, mip_image->Size, data));
		else if (_textureType == TextureType::Cube)
			GL_ERROR(glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + idx, 0, internal_format, w, h, 0, mip_image->Size, data));
	}
	else
	{
		GLenum format_type = GL_GetTextureFormatType(_imageFormat);
		if (_textureType == TextureType::D2)
			GL_ERROR(glTexImage2D(GL_TEXTURE_2D, idx, internal_format, w, h, 0, color_format, format_type, data));
		else if (_textureType == TextureType::Cube)
			GL_ERROR(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + idx, 0, internal_format, w, h, 0, color_format, format_type, data));
	}
	return true;
}
byte* GLTexture::Lock(TextureLockDesc& lock_desc)
{
	if (_textureType != TextureType::D2)
		return nullptr;

	lock_desc.pitch = this->GetPitch();
	if (_imageData == nullptr)
	{
		uint size = this->GetBytes();
		_imageData = Memory::NewArray<byte>(size);
		Memory::Clear(_imageData, size);
	}
	return _imageData;
}
void GLTexture::Unlock(const TextureLockDesc& lock_desc)
{
	if (_textureType != TextureType::D2)
		return;

	GLenum color_format = GL_GetTextureFormat(_imageFormat);
	GLenum internal_format = GL_GetInternalFormat(_imageFormat, this->IsSRGB());
	GLenum format_type = GL_GetTextureFormatType(_imageFormat);

	GL_ERROR(glBindTexture(GL_TEXTURE_2D, _colorTexture));
	GL_ERROR(glTexImage2D(GL_TEXTURE_2D, 0, internal_format, _imageWidth, _imageHeight, 0, color_format, format_type, _imageData));
	GL_ERROR(glBindTexture(GL_TEXTURE_2D, 0));
}
bool GLTexture::GetData(MemoryDataStream& stream)
{
	if (_textureType != TextureType::D2)
		return false;

	stream.Resize(this->GetBytes());
	GLint read_type, read_format;
	GL_ERROR(glBindTexture(GL_TEXTURE_2D, _colorTexture));
	GL_ERROR(glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_TYPE, &read_type));
	GL_ERROR(glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_FORMAT, &read_format));
	GL_ERROR(glReadPixels(0, 0, this->GetWidth(), this->GetHeight(), read_format, read_type, stream.Buffer()));//TODO:有溢出可能(如果存在边界填充)
	GL_ERROR(glBindTexture(GL_TEXTURE_2D, 0));

	return true;
}
bool GLTexture::Copy(Texture* dst)
{
	AssertEx(dst->GetWidth() == this->GetWidth(), "");
	AssertEx(dst->GetHeight() == this->GetHeight(), "");
	AssertEx(dst->GetFormat() == this->GetFormat(), "");

	{//拷贝
		TextureLockDesc lock_desc;
		void* bits = dst->Lock(lock_desc);
		if (bits)
		{
			MemoryDataStream stream;
			if (this->GetData(stream))
			{
				Memory::Copy(bits, stream.Buffer(), stream.Size());
			}
		}
		dst->Unlock(lock_desc);
	}

	return true;
}
void GLTexture::SaveToFile(const String& name, ImageType type)
{
	Image* image = Image::Create(_imageFormat, iSize(_imageWidth, _imageHeight), false);
	MemoryDataStream stream;
	if (this->GetData(stream) && image->GetSize() == stream.Size())
	{
		image->Fill(stream.Buffer(), stream.Size());
		image->SaveToFile(name);
	}
}
DC_END_NAMESPACE