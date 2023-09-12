#include "Image.h" 
#include "core/math/Math.h"
#include "core/stream/DataStream.h"
#include "runtime/resources/Resources.h"
#ifndef STB_IMAGE_IMPLEMENTATION
	#define STB_IMAGE_IMPLEMENTATION
#endif
#include "external/stb/stb_image.h"
#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
	#define STB_IMAGE_WRITE_IMPLEMENTATION
#endif
#include "external/stb/stb_image_write.h"
#include "external/astc/astc_codec_internals.h"
#include "external/soil2/stbi_DDS.h"
#include "external/soil2/stbi_DDS_c.h"
#include "external/soil2/stbi_pkm.h"
#include "external/soil2/stbi_pkm_c.h"
#include "external/soil2/stbi_pvr.h"
#include "external/soil2/stbi_pvr_c.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
bool Image::LoadFromSTBFile(const String& path, ImageType type)
{
	switch (type)
	{
	case ImageType::PNG:
	case ImageType::BMP:
	case ImageType::GIF:
	case ImageType::JPEG:
	case ImageType::HDR:
	case ImageType::TGA:
	case ImageType::DDS:
	case ImageType::PKM:
	case ImageType::PVR:
	{
		int width, height, n;
		MemoryDataStream stream;
		if (!File::ReadAllBytes(path, stream))
		{
			Debuger::Error("Cannot read file:%s", path.c_str());
			return false;
		}

		byte* std_data = nullptr;
		switch (type)
		{
		case ImageType::DDS:
			std_data = stbi__dds_load_from_memory(stream.data(), stream.Size(), &width, &height, &n, 4);
			break;
		case ImageType::PKM:
			std_data = stbi__pkm_load_from_memory(stream.data(), stream.Size(), &width, &height, &n, 4);
			break;
		case ImageType::PVR:
			std_data = stbi__pvr_load_from_memory(stream.data(), stream.Size(), &width, &height, &n, 4);
			break;
		default:
			std_data = stbi_load_from_memory(stream.data(), stream.Size(), &width, &height, &n, 4);
			break;
		}
		if (!std_data)
		{
			Debuger::Error("Load file error:%s", path.c_str());
			return false;
		}

		_size = iSize(width, height);
		_format = ColorFormat::R8G8B8A8;
		uint row_pitch = _size.width * 4;//读取的图片，统一转成R8G8B8A8
		uint bytes = row_pitch * _size.height;//得到图像位深

		this->Clear();
		ImageMipData* mip_data = DBG_NEW ImageMipData(row_pitch, _size.width, _size.height, bytes);
		_imageData.Add(mip_data);
#if defined(DC_GRAPHICS_API_DX9)
		ColorConverter::FromRGBA8toBGRA8(std_data, _size.width*_size.height, mip_data->Data);
#else
		Memory::Copy(mip_data->Data, std_data, bytes);
#endif

		stbi_image_free(std_data);//卸载图像函数
	}
	break;
	case ImageType::ASTC:
	{
		astc_codec_image* image = astc_load_file(path.c_str());
		if (!image)
		{
			Debuger::Error("Cannot read file:%s", path.c_str());
			destroy_image(image);//卸载图像函数
			return false;
		}
		if (!image->imagedata8)
		{
			Debuger::Error("Invalid format:%s", path.c_str());
			destroy_image(image);//卸载图像函数
			return false;
		}
		int image_channels = determine_image_channels(image);
		if (image_channels != 3 && image_channels != 4)
		{
			Debuger::Error("Invalid channels:%s, %d", path.c_str(), image_channels);
			destroy_image(image);//卸载图像函数
			return false;
		}

		_size = iSize(image->xsize, image->ysize);
		_format = ColorFormat::R8G8B8A8;
		uint row_pitch = _size.width * 4;//读取的图片，统一转成R8G8B8A8
		uint bytes = row_pitch * _size.height;//得到图像位深

		this->Clear();
		ImageMipData* mip_data = DBG_NEW ImageMipData(row_pitch, _size.width, _size.height, bytes);
		_imageData.Add(mip_data);
		for (int y = 0; y < _size.height; y++)
		{
			switch (image_channels)
			{
			case 3:
				for (int x = 0; x < _size.width; x++)
				{
#if defined(DC_GRAPHICS_API_DX9)
					mip_data->Data[y * row_pitch + 4 * x + 0] = image->imagedata8[0][y][4 * x + 2];
					mip_data->Data[y * row_pitch + 4 * x + 1] = image->imagedata8[0][y][4 * x + 1];
					mip_data->Data[y * row_pitch + 4 * x + 2] = image->imagedata8[0][y][4 * x];
					mip_data->Data[y * row_pitch + 4 * x + 3] = 255;
#else
					mip_data->Data[y * row_pitch + 4 * x + 0] = image->imagedata8[0][y][4 * x];
					mip_data->Data[y * row_pitch + 4 * x + 1] = image->imagedata8[0][y][4 * x + 1];
					mip_data->Data[y * row_pitch + 4 * x + 2] = image->imagedata8[0][y][4 * x + 2];
					mip_data->Data[y * row_pitch + 4 * x + 3] = 255;
#endif
				}
				break;
			case 4:
				for (int x = 0; x < _size.width; x++)
				{
#if defined(DC_GRAPHICS_API_DX9)
					mip_data->Data[y * row_pitch + 4 * x + 0] = image->imagedata8[0][y][4 * x + 2];
					mip_data->Data[y * row_pitch + 4 * x + 1] = image->imagedata8[0][y][4 * x + 1];
					mip_data->Data[y * row_pitch + 4 * x + 2] = image->imagedata8[0][y][4 * x];
					mip_data->Data[y * row_pitch + 4 * x + 3] = image->imagedata8[0][y][4 * x + 3];
#else
					mip_data->Data[y * row_pitch + 4 * x + 0] = image->imagedata8[0][y][4 * x];
					mip_data->Data[y * row_pitch + 4 * x + 1] = image->imagedata8[0][y][4 * x + 1];
					mip_data->Data[y * row_pitch + 4 * x + 2] = image->imagedata8[0][y][4 * x + 2];
					mip_data->Data[y * row_pitch + 4 * x + 3] = image->imagedata8[0][y][4 * x + 3];
#endif
				}
				break;
			}
		}
		destroy_image(image);//卸载图像函数
	}
	break;
	case ImageType::RAW:
	{
		MemoryDataStream stream;
		if (!File::ReadAllBytes(path, stream))
		{
			Debuger::Error("Cannot read file:%s", path.c_str());
			return false;
		}
		// Check size
		int size = (int)Math::Sqrt(stream.Size() / 2.0f);
		if (stream.Size() != size * size * 2)
		{
			Debuger::Error("Invalid RAW file data size or format. Use 16-bit .RAW file in little-endian byte order (square dimensions):%s", path.c_str());
			return false;
		}

		_size = iSize(size, size);
		_format = ColorFormat::R16;
		uint row_pitch = uint(stream.Size() / size);

		this->Clear();
		ImageMipData* mip_data = DBG_NEW ImageMipData(row_pitch, _size.width, _size.height, stream.Size());
		_imageData.Add(mip_data);
		Memory::Copy(mip_data->Data, stream.data(), stream.Size());
	}
	break;
	default:
		Debuger::Error("Unknown format:%s", path.c_str());
		return false;
	}

	return true;
}
bool Image::SaveToSTBFile(const String& path, ImageType type)
{
	String dir = Path::GetDirectoryName(path);
	if (!dir.IsEmpty() && !Directory::Exist(dir))
	{
		Directory::Create(dir);
	}
	ImageMipData* mip_data = this->GetMipData(0);
	int result = 0;
	switch (type)
	{
	case dc::ImageType::BMP:
		result = stbi_write_bmp(path.c_str(), _size.width, _size.height, BitsPerPixel(_format)/8, mip_data->Data);
		break;
	case dc::ImageType::JPEG:
		result = stbi_write_jpg(path.c_str(), _size.width, _size.height, BitsPerPixel(_format)/8, mip_data->Data, mip_data->RowPitch);
		break;
	case dc::ImageType::TGA:
		result = stbi_write_tga(path.c_str(), _size.width, _size.height, BitsPerPixel(_format)/8, mip_data->Data);
		break;
	case dc::ImageType::PNG:
		result = stbi_write_png(path.c_str(), _size.width, _size.height, BitsPerPixel(_format)/8, mip_data->Data, mip_data->RowPitch);
		break;
	case dc::ImageType::HDR:
		result = stbi_write_hdr(path.c_str(), _size.width, _size.height, BitsPerPixel(_format)/8, (float*)mip_data->Data);
		break;
	default:
		Debuger::Error("image save to file error, format is not supported by stb library:%d", (int)type);
		return false;
	}
	Debuger::Log("image save to file:%s, result:%s", path.c_str(), result != 0 ? "true" : "false");
	return result != 0;
}
DC_END_NAMESPACE