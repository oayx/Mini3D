#include "Image.h"
#include "core/math/Math.h"
#include "core/stream/DataStream.h"
#include "runtime/resources/Resources.h"

DC_BEGIN_NAMESPACE
enum
{ 
	ETC1_RGB_NO_MIPMAPS,				// GL_ETC1_RGB8_OES
	ETC2PACKAGE_RGB_NO_MIPMAPS,			// GL_COMPRESSED_RGB8_ETC2
	ETC2PACKAGE_RGBA_NO_MIPMAPS_OLD,	// not used 
	ETC2PACKAGE_RGBA_NO_MIPMAPS,		// GL_COMPRESSED_RGBA8_ETC2_EAC
	ETC2PACKAGE_RGBA1_NO_MIPMAPS,		// GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2
	ETC2PACKAGE_R_NO_MIPMAPS,			// GL_COMPRESSED_R11_EAC
	ETC2PACKAGE_RG_NO_MIPMAPS,			// GL_COMPRESSED_RG11_EAC
	ETC2PACKAGE_R_SIGNED_NO_MIPMAPS,	// GL_COMPRESSED_SIGNED_R11_EAC
	ETC2PACKAGE_RG_SIGNED_NO_MIPMAPS,	// GL_COMPRESSED_SIGNED_RG11_EAC
	ETC2PACKAGE_sRGB_NO_MIPMAPS,		//
	ETC2PACKAGE_sRGBA_NO_MIPMAPS,		//
	ETC2PACKAGE_sRGBA1_NO_MIPMAPS		//
};

#ifndef ETC_PKM_HEADER_SIZE
#define ETC_PKM_HEADER_SIZE 16
#endif

static const char kMagic[]  = { 'P', 'K', 'M', ' ', '1', '0' };
static const char kMagic2[] = { 'P', 'K', 'M', ' ', '2', '0' };

static const uint ETC1_PKM_FORMAT_OFFSET = 6;
static const uint ETC1_PKM_ENCODED_WIDTH_OFFSET = 8;
static const uint ETC1_PKM_ENCODED_HEIGHT_OFFSET = 10;
static const uint ETC1_PKM_WIDTH_OFFSET = 12;
static const uint ETC1_PKM_HEIGHT_OFFSET = 14;
static uint ReadBEUint16(const byte* pIn) 
{
	return (pIn[0] << 8) | pIn[1];
}
/********************************************************************/
bool Image::LoadFromETCFile(const String& path, bool mipmap)
{
	MemoryDataStream stream;
	if (!File::ReadAllBytes(path, stream))
	{
		Debuger::Error("Cannot read file:%s", path.c_str());
		return false;
	}
	const byte* header = stream.data();
	if (::memcmp(header, kMagic, sizeof(kMagic)) != 0 && ::memcmp(header, kMagic2, sizeof(kMagic2)) != 0)
	{
		Debuger::Error("Invalid file type:%s", path.c_str());
		return false;
	}
	uint format = ReadBEUint16(header + ETC1_PKM_FORMAT_OFFSET);
	uint encodedWidth = ReadBEUint16(header + ETC1_PKM_ENCODED_WIDTH_OFFSET);
	uint encodedHeight = ReadBEUint16(header + ETC1_PKM_ENCODED_HEIGHT_OFFSET);
	uint width = ReadBEUint16(header + ETC1_PKM_WIDTH_OFFSET);
	uint height = ReadBEUint16(header + ETC1_PKM_HEIGHT_OFFSET);
	if (!(encodedWidth >= width && encodedWidth - width < 4 && encodedHeight >= height && encodedHeight - height < 4))
	{
		Debuger::Error("Invalid file size:%s", path.c_str());
		return false;
	}

	uint dataLen = stream.Size() - ETC_PKM_HEADER_SIZE;
	_size = iSize(width, height);
	switch (format)
	{
	case ETC1_RGB_NO_MIPMAPS:_format = ColorFormat::ETC_RGB4; break;
	case ETC2PACKAGE_RGB_NO_MIPMAPS:_format = ColorFormat::ETC2_RGB4; break;
	case ETC2PACKAGE_RGBA_NO_MIPMAPS:_format = ColorFormat::ETC2_RGBA8; break;
	default: Debuger::Error("Invalid file format:%s", path.c_str()); return false;
	}

	this->Clear();
	uint row_pitch = uint(dataLen / height);
	ImageMipData* mip_data = DBG_NEW ImageMipData(row_pitch, _size.width, _size.height, dataLen);
	_imageData.Add(mip_data);
	::memcpy(mip_data->Data, stream.data() + ETC_PKM_HEADER_SIZE, dataLen);

	return true;
}
bool Image::SaveToETCFile(const String& path)
{
	return false;
}
DC_END_NAMESPACE