#include "Image.h"
#include "ImageManager.h"
#include "core/file/File.h"
#include "core/file/Path.h"
#include "core/stream/DataStream.h"
#include "runtime/graphics/null/GraphicsCaps.h"
#include "runtime/resources/Resources.h"
#include "runtime/Application.h"
#ifndef STB_IMAGE_RESIZE_IMPLEMENTATION
	#define STB_IMAGE_RESIZE_IMPLEMENTATION
#endif
#include "external/stb/stb_image_resize.h"
#include "external/stb/stb_image_write.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(Image, Object);
Image::Image(ColorFormat format, const iSize& size, bool round)
	: _size(size), _format(format)
{
	if (round && !IsCompressedFormat(format))
	{
		_size.Set(((_size.width) + 8 - 1)&~(8 - 1), _size.height);//转换成8的倍数
	}
	uint row_pitch = BytesRowPitch(_format, _size.width);
	uint bytes = TotalBytes(_format, _size.width, _size.height);
	ImageMipData* mip_data = DBG_NEW ImageMipData(row_pitch, _size.width, _size.height, bytes);
	_imageData.Add(mip_data);
}
Image::~Image()
{
	this->Clear();
}
Image* Image::Create(ColorFormat format, const iSize& size)
{
	return Image::Create(format, size, false);
}
Image* Image::Create(ColorFormat format, const iSize& size, bool round)
{
	Image* image = DBG_NEW Image(format, size, round);
	image->AutoRelease();
	return image;
}
Image* Image::Create(const String& file, bool mipmap)
{
	DC_PROFILE_FUNCTION();

	Image* image = ImageManager::GetImage(file);
	if (image)
	{
		//可能需要计算mipmap
		if (mipmap && image->MipLevels() == 1)image->GenerateMipMap();
		return image;
	}
	
	image = DBG_NEW Image();
	if (image->LoadFromFile(Resource::GetFullDataPath(file), mipmap, false))
	{
		image->_file = file;
		image->AutoRelease();
		ImageManager::AddImage(file, image);
		return image;
	}
	SAFE_RELEASE(image);
	return nullptr;
}
Image* Image::CreateEditor(const String& file)
{
	DC_PROFILE_FUNCTION();

	Image* image = ImageManager::GetImage(file);
	if (image)
	{
		return image;
	}

	image = DBG_NEW Image();
	if (image->LoadFromFile(Resource::GetFullSavePath(file), false, true))
	{
		image->_file = file;
		image->AutoRelease();
		ImageManager::AddImage(file, image);
		return image;
	}
	SAFE_RELEASE(image);
	return nullptr;
}
ImageType Image::GetImageType(const String& file)
{
	if (file.IsEmpty())return ImageType::Undefined;
	String ext = Path::GetExtension(file).ToLower();
	if (ext.IsEmpty())
	{//可能传进来的已经是扩展名
		ext = Path::GetFileName(file);
	}
	if (ext == "png")return ImageType::PNG;
	else if (ext == "jpg" || ext == "jpeg")return ImageType::JPEG;
	else if (ext == "tga")return ImageType::TGA;
	else if (ext == "bmp")return ImageType::BMP;
	else if (ext == "gif")return ImageType::GIF;
	else if (ext == "hdr")return ImageType::HDR;
	else if (ext == "raw")return ImageType::RAW;
	else if (ext == "cube")return ImageType::CUBE;
	else if (ext == "dds")return ImageType::DDS;
	else if (ext == "pkm")return ImageType::PKM;
	else if (ext == "astc")return ImageType::ASTC;
	else return ImageType::Undefined;
}
Object* Image::Clone(Object* new_obj)
{
	Image* image = Image::Create(_format, _size);
	image->Fill(this->Data(), GetSize());
	return image;
}
bool Image::LoadFromFile(const String& file, bool mipmap, bool no_compress)
{
	_isStatic = true;

	ImageType type = Image::GetImageType(file);
	bool result = false;
	if (no_compress)
	{
		switch (type)
		{
		case ImageType::DDS:
		{
			if (Application::GetGraphics()->GetCaps()->hasDXT1Texture)
				result = this->LoadFromDDSFile(file, mipmap);
			break;
		}
		case ImageType::PKM:
		{
			if (Application::GetGraphics()->GetCaps()->hasETC1Texture)
				result = this->LoadFromETCFile(file, mipmap);
			break;
		}
		case ImageType::ASTC:
		{
			if (Application::GetGraphics()->GetCaps()->hasASTCTexture)
				result = this->LoadFromASTCFile(file, mipmap);
			break;
		}
		default:break;
		}
	}
	if (type == ImageType::CUBE)
	{
		result = this->LoadFromCubeFile(file, mipmap);
	}
	if (!result)
	{
		result = this->LoadFromSTBFile(file, type);
		if(mipmap)this->GenerateMipMap(0);
	}
	return result;
}
bool Image::LoadFromCubeFile(const String& file, bool mipmap)
{
	MemoryDataStream stream;
	if (!File::ReadAllBytes(file, stream))return false;

	String header = stream.ReadString(4);
	if (header != "cube")return false;

	uint w = stream.Read<uint>();
	uint h = stream.Read<uint>();
	byte* by_rgb = NewArray<byte>(w * h * 3);
	byte* by_rgba = NewArray<byte>(w * h * 4);

	this->Clear();
	_format = ColorFormat::R8G8B8A8;
	_size = iSize(w, h);
	_isCube = true;
	for (int i = 0; i < 6; ++i)
	{
		byte flag = stream.Read<byte>();
		uint offset = stream.Read<uint>();
		stream.Read(by_rgb, offset);
		//文件保存的是bgr格式，需要转换成rgb格式
		for (uint i = 0; i < offset; i += 3)
		{
			byte t = by_rgb[i];
			by_rgb[i] = by_rgb[i + 2];
			by_rgb[i + 2] = t;
		}

		//转成RGBA8
#if defined(DC_GRAPHICS_API_DX9)
		ColorConverter::FromRGB8toBGRA8(by_rgb, w * h, by_rgba);
#else
		ColorConverter::FromRGB8toRGBA8(by_rgb, w * h, by_rgba);
#endif
		uint row_pitch = w * 4;
		uint size = row_pitch * h;
		ImageMipData* mip_data = DBG_NEW ImageMipData(row_pitch, w, h, size);
		_imageData.Add(mip_data);
		::memcpy(mip_data->Data, by_rgba, size);
	}
	DeleteArray(by_rgb);
	DeleteArray(by_rgba);

	return true;
}
bool Image::GenerateMipMap(int levels)
{
	AssertEx(levels >= 0, "");
	if (_isCube)return false;
	if (_imageData.Size() != 1)return false;

	ImageMipData* mip_data = GetMipData(0);
	int width = mip_data->Width;
	int height = mip_data->Height;

	if (levels == 0)levels = this->GenerateMipMapLevels();
	for (int i = 1; i < levels; ++i)
	{
		width = Math::Max<int>(width / 2, 1);
		height = Math::Max<int>(height / 2, 1);

		ImageMipData* mip_data_lv = DBG_NEW ImageMipData(BytesRowPitch(_format, width), width, height, TotalBytes(_format, width, height));
		_imageData.Add(mip_data_lv);

		// 改变图片尺寸
		stbir_resize(mip_data->Data, mip_data->Width, mip_data->Height, 0, mip_data_lv->Data, width, height, 0, STBIR_TYPE_UINT8, 4, STBIR_ALPHA_CHANNEL_NONE, 0,
			STBIR_EDGE_CLAMP, STBIR_EDGE_CLAMP,
			STBIR_FILTER_BOX, STBIR_FILTER_BOX,
			STBIR_COLORSPACE_LINEAR, nullptr);

		if (width == 1 || height == 1)
			break;
	}

	return true;
}
int Image::GenerateMipMapLevels()
{
	if (_isCube)return 1;
	if (this->MipLevels() < 1)return 1;

	ImageMipData* mip_data = GetMipData(0);
	int width = mip_data->Width;
	int height = mip_data->Height;
	if (width == 1 || height == 1)return 1;

	int levels = 0;
	while (true)
	{
		levels++;
		if (width == 1 || height == 1)
			break;

		width = Math::Max<int>(width / 2, 1);
		height = Math::Max<int>(height / 2, 1);
	}
	return levels;
}
bool Image::SaveToFile(const String& file)
{
	if (file.IsEmpty())
		return false;
	
	ImageType type = Image::GetImageType(file);
	String path = Resource::GetFullSavePath(file);
	switch (type)
	{
	case ImageType::DDS:
		return SaveToDDSFile(path);
	case ImageType::PKM:
		return SaveToETCFile(path);
	default:
		return SaveToSTBFile(path, type);
	}
	return false;
}
void Image::SetPixel(uint x, uint y, const Color &color, bool blend)
{
	AssertEx(!_isStatic, "");
	if (x >= _size.width || y >= _size.height)
		return;

	ImageMipData* mip_data = this->GetMipData(0);
	switch(_format)
	{
		case ColorFormat::R5G5B5A1:
		{
			ushort* dest = (ushort*) (mip_data->Data + ( y * mip_data->RowPitch) + ( x << 1 ));
			*dest = RGBA8toR5G5B5A1( color );
		} break;

		case ColorFormat::R5G6B5:
		{
			ushort* dest = (ushort*) (mip_data->Data + ( y * mip_data->RowPitch) + ( x << 1 ));
			*dest = RGBA8toR5G6B5( color );
		} break;

		case ColorFormat::R8G8B8:
		{
			byte* dest = mip_data->Data + ( y * mip_data->RowPitch) + ( x * 3 );
			dest[0] = (byte)color.GetRed();
			dest[1] = (byte)color.GetGreen();
			dest[2] = (byte)color.GetBlue();
		} break;

		case ColorFormat::R8G8B8A8:
		{
			uint* dest = (uint*) (mip_data->Data + ( y * mip_data->RowPitch) + ( x << 2 ));
			*dest = blend ? PixelBlend32 ( *dest, (uint)color ) : (uint)color;
		} break;
#ifndef DC_DEBUG
		default:
			Debuger::Error("Image::SetPixel - error format:%d", (int)_format);
			break;
#endif
	}
	this->Unlock();
}
Color Image::GetPixel(uint x, uint y) const
{
	if (x >= _size.width || y >= _size.height)
		return Color::Black;

	ImageMipData* mip_data = this->GetMipData(0);
	switch(_format)
	{
	case ColorFormat::R5G5B5A1:
	{
		const ushort* dest = (ushort*)(mip_data->Data + (y * mip_data->RowPitch) + (x << 1));
		return R5G5B5A1toRGBA8(*dest);
	}
	case ColorFormat::R5G6B5:
	{
		const ushort* dest = (ushort*)(mip_data->Data + (y * mip_data->RowPitch) + (x << 1));
		return R5G6B5toRGBA8(*dest);
	}
	case ColorFormat::R8G8B8:
	{
		const byte* p = mip_data->Data + y * mip_data->RowPitch + (x * 3);
		return Color(p[0], p[1], p[2], 255);
	}
	case ColorFormat::R8G8B8A8:
	{
		return ((uint*)mip_data->Data)[int(y*_size.width + x)];
	}
#ifndef DC_DEBUG
	default:
		Debuger::Error("Image::GetPixel - error format:%d", (int)_format);
		break;
#endif
	}

	return Color::Black;
}
bool Image::Resize(uint width, uint height)
{
	CHECK_RETURN_PTR_FALSE(width != 0 || height != 0);
	if (_isCube)return false;
	if (_imageData.Size() != 1)return false;
	if (IsCompressedFormat(_format))return false;

	ImageMipData* src_data = GetMipData(0);
	ImageMipData* dst_data = DBG_NEW ImageMipData(BytesRowPitch(_format, width), width, height, TotalBytes(_format, width, height));
	
	stbir_resize(src_data->Data, src_data->Width, src_data->Height, 0, dst_data->Data, width, height, 0, STBIR_TYPE_UINT8, 4, STBIR_ALPHA_CHANNEL_NONE, 0,
		STBIR_EDGE_CLAMP, STBIR_EDGE_CLAMP,
		STBIR_FILTER_BOX, STBIR_FILTER_BOX,
		STBIR_COLORSPACE_LINEAR, nullptr);

	this->Clear();
	_size = iSize(width, height);
	_imageData.Add(dst_data);
	return true;
}
void Image::Fill(const Color &color)
{
	AssertEx(!_isStatic, "");
	uint c = 0;
	switch ( _format )
	{
		case ColorFormat::R5G5B5A1:
			c = RGBA8toR5G5B5A1(color);
			c |= c << 16;
			break;
		case ColorFormat::R5G6B5:
			c = RGBA8toR5G6B5( color );
			c |= c << 16;
			break;
		case ColorFormat::R8G8B8A8:
			c = color;
			break;
		case ColorFormat::R8G8B8:
		{
			byte rgb[3]; 
			ColorConverter::FromRGBA8toRGB8(color.ptr(), 1, rgb);
			const uint _size = GetSize();
			byte* data = this->Lock();
			for (uint i=0; i<_size; i+=3)
			{
				::memcpy(data +i, rgb, 3);
			}
			this->Unlock();
			return;
		}
		default:
			return;
	}
	byte* data = this->Lock();
	::memset(data, c, GetSize() );
	this->Unlock();
}
void Image::Fill(const byte* by, uint size)
{
	AssertEx(!_isStatic, "");
	byte* data = this->Lock();
	::memcpy(data, by, size);
	this->Unlock();
}
void Image::UpDownTransfer()
{
	ImageMipData* mip_data = this->GetMipData(0);
	byte* data = mip_data->Data;
	byte* new_data = NewArray<byte>(_size.height * mip_data->RowPitch);
	{
		::memcpy(new_data, data, mip_data->Size);//保证_data指针不变
		for (int row = (int)_size.height - 1; row >= 0; --row)
		{
			::memcpy(data, new_data + mip_data->RowPitch * row, mip_data->RowPitch);
		}
	}
	DeleteArray(new_data);
}
void Image::Clear()
{
	for (int i = 0; i < _imageData.Size(); ++i)
	{
		SAFE_DELETE(_imageData[i]);
	}
	_imageData.Clear();
}
DC_END_NAMESPACE