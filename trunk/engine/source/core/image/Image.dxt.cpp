#include "Image.h"
#include "core/math/Math.h"
#include "runtime/resources/Resources.h"
#if defined(DC_GRAPHICS_API_DX9) || defined(DC_GRAPHICS_API_DX11) || defined(DC_GRAPHICS_API_DX12)
#include <directxtex/DirectXTex.h>
#	if defined(DC_GRAPHICS_API_DX9)
#		include "runtime/graphics/dx/dx9/DX9Define.h"
#	elif defined(DC_GRAPHICS_API_DX11)
#		include "runtime/graphics/dx/dx11/DX11Define.h"
#	elif defined(DC_GRAPHICS_API_DX12)
#		include "runtime/graphics/dx/dx12/DX12Define.h"
#	endif
#endif

DC_BEGIN_NAMESPACE
/********************************************************************/
bool Image::LoadFromDDSFile(const String& path, bool mipmap)
{
#if defined(DC_GRAPHICS_API_DX9) || defined(DC_GRAPHICS_API_DX11) || defined(DC_GRAPHICS_API_DX12)
	std::wstring wPath = path.ToWString();

	// Load image data
	DirectX::ScratchImage image;
	HRESULT result = DirectX::LoadFromDDSFile(wPath.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);
	if (FAILED(result))
	{
		Debuger::Error("Failed to import texture from file:%s. Result: %d", path.c_str(), static_cast<uint>(result));
		return false;
	}
	// Convert into texture data
	auto& meta = image.GetMetadata();
	this->Clear();
	_size = iSize((int)meta.width, (int)meta.height);
#if defined(DC_GRAPHICS_API_DX9)
	//_format = DX9GetColorFormat(meta.format);
#elif defined(DC_GRAPHICS_API_DX11) || defined(DC_GRAPHICS_API_DX12)
	_format = DX10GetColorFormat(meta.format);
#endif
	//for (int arrayIndex = 0; arrayIndex < (int)meta.arraySize; arrayIndex++)
	{//不支持多维数组
		for (int mipIndex = 0; mipIndex < (int)meta.mipLevels; mipIndex++)
		{
			const auto img = image.GetImage(mipIndex, 0, 0);
			ImageMipData* mip_data = Memory::New<ImageMipData>((uint)img->rowPitch, img->width, img->height, (uint)img->slicePitch);
			_imageData.Add(mip_data);
			if (_format == ColorFormat::B8G8R8)
			{
				ColorConverter::FromRGB8toBGR8(img->pixels, uint(img->width*img->height), mip_data->Data);
				_format = ColorFormat::R8G8B8;
			}
			else if (_format == ColorFormat::B8G8R8A8)
			{
				ColorConverter::FromRGBA8toBGRA8(img->pixels, uint(img->width*img->height), mip_data->Data);
				_format = ColorFormat::R8G8B8A8;
			}
			else
				Memory::Copy(mip_data->Data, img->pixels, (uint)img->slicePitch);


			if (!mipmap)break;
		}
	}
	return true;
#else
	return false;
#endif
}
bool Image::SaveToDDSFile(const String& path)
{
#if defined(DC_GRAPHICS_API_DX9) || defined(DC_GRAPHICS_API_DX11) || defined(DC_GRAPHICS_API_DX12)
	String dir = Path::GetDirectoryName(path);
	if (!dir.IsEmpty() && !Directory::Exist(dir))
	{
		Directory::Create(dir);
	}
	std::wstring wPath = path.ToWString();

	DirectX::Image image;
	image.width = _size.width;
	image.height = _size.height;
#if defined(DC_GRAPHICS_API_DX9)
	//image.format = DX9GetTextureFormat(_format);
#elif defined(DC_GRAPHICS_API_DX11) || defined(DC_GRAPHICS_API_DX12)
	image.format = DX10GetTextureFormat(_format, false);
#endif
	image.rowPitch = GetPitch();
	image.slicePitch = GetSize();
	image.pixels = (uint8_t*)this->Data();
	HRESULT result = S_FALSE;
	{
		DirectX::TexMetadata metadata;
		metadata.width = image.width;
		metadata.height = image.height;
		metadata.depth = 1;
		metadata.arraySize = 1;
		metadata.mipLevels = _imageData.Size();
		metadata.miscFlags = 0;
		metadata.miscFlags2 = 0;
		metadata.format = image.format;
		metadata.dimension = DirectX::TEX_DIMENSION_TEXTURE2D;
		Vector<DirectX::Image> images;
		images.Resize((int)(metadata.mipLevels * metadata.arraySize));
		for (int arrayIndex = 0; arrayIndex < (int)metadata.arraySize; arrayIndex++)
		{
			for (int mipIndex = 0; mipIndex < (int)metadata.mipLevels; mipIndex++)
			{
				ImageMipData* mip_data = GetMipData(mipIndex);
				auto& img = images[(int)(metadata.mipLevels * arrayIndex + mipIndex)];
				img.width = Math::Max<size_t>(1, image.width >> mipIndex);
				img.height = Math::Max<size_t>(1, image.height >> mipIndex);
				img.format = image.format;
				img.rowPitch = mip_data->RowPitch;
				img.slicePitch = mip_data->Size;
				img.pixels = (uint8_t*)mip_data->Data;
			}
		}
		result = DirectX::SaveToDDSFile(images.Data(), images.Size(), metadata, DirectX::DDS_FLAGS_NONE, wPath.c_str());
	}

	return SUCCEEDED(result);
#else
	return false;
#endif
}
DC_END_NAMESPACE