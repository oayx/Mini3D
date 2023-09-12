#include "Image.h"
#include "core/math/Math.h"
#include "core/stream/DataStream.h"
#include "runtime/resources/Resources.h"

DC_BEGIN_NAMESPACE
struct astc_header
{
	uint8_t magic[4];
	uint8_t blockdim_x;
	uint8_t blockdim_y;
	uint8_t blockdim_z;
	uint8_t xsize[3];			// x-size = xsize[0] + xsize[1] + xsize[2]
	uint8_t ysize[3];			// x-size, y-size and z-size are given in texels;
	uint8_t zsize[3];			// block count is inferred
};
#define MAGIC_FILE_CONSTANT 0x5CA1AB13
/********************************************************************/
bool Image::LoadFromASTCFile(const String& path, bool mipmap)
{
	MemoryDataStream stream;
	if (!File::ReadAllBytes(path, stream) || stream.Size() < sizeof(astc_header))
	{
		Debuger::Error("Cannot read file:%s", path.c_str());
		return false;
	}
	astc_header* header = (astc_header*)stream.data();
	uint32_t magicval = header->magic[0] + 256 * (uint32_t)(header->magic[1]) + 65536 * (uint32_t)(header->magic[2]) + 16777216 * (uint32_t)(header->magic[3]);
	if (magicval != MAGIC_FILE_CONSTANT)
	{
		Debuger::Error("File is invalid astc:%s", path.c_str());
		return false;
	}
	if (header->blockdim_x != header->blockdim_y)
	{
		Debuger::Error("File %s is invalid block %d*%d", path.c_str(), header->blockdim_x, header->blockdim_y);
		return false;
	}

	int xdim = header->blockdim_x;
	int ydim = header->blockdim_y;
	int zdim = header->blockdim_z;
	if ((xdim < 3 || xdim > 6 || ydim < 3 || ydim > 6 || zdim < 3 || zdim > 6) && (xdim < 4 || xdim == 7 || xdim == 9 || xdim == 11 || xdim > 12 || ydim < 4 || ydim == 7 || ydim == 9 || ydim == 11 || ydim > 12 || zdim != 1))
	{
		Debuger::Error("File %s not recognized", path.c_str());
		return false;
	}

	int width = header->xsize[0] + 256 * header->xsize[1] + 65536 * header->xsize[2];
	int height = header->ysize[0] + 256 * header->ysize[1] + 65536 * header->ysize[2];
	int zsize = header->zsize[0] + 256 * header->zsize[1] + 65536 * header->zsize[2];
	if (width == 0 || height == 0 || zsize == 0)
	{
		Debuger::Error("File %s has zero dimension", path.c_str());
		return false;
	}
	int xblocks = (width + xdim - 1) / xdim;
	int yblocks = (height + ydim - 1) / ydim;
	int zblocks = (zsize + zdim - 1) / zdim;

	uint row_pitch = xblocks * 16;
	uint dataLen = row_pitch * yblocks;
	if (dataLen != stream.Size() - sizeof(astc_header))
	{
		Debuger::Error("File %s has error size", path.c_str());
		return false;
	}

	_size = iSize(width, height);
	switch (header->blockdim_x)
	{
	case 4:_format = ColorFormat::ASTC_RGBA_4x4; break;
	case 5:_format = ColorFormat::ASTC_RGBA_5x5; break;
	case 6:_format = ColorFormat::ASTC_RGBA_6x6; break;
	case 8:_format = ColorFormat::ASTC_RGBA_8x8; break;
	case 10:_format = ColorFormat::ASTC_RGBA_10x10; break;
	case 12:_format = ColorFormat::ASTC_RGBA_12x12; break;
	default:AssertEx(false, ""); return false;
	}

	this->Clear();
	ImageMipData* mip_data = DBG_NEW ImageMipData(row_pitch, _size.width, _size.height, dataLen);
	_imageData.Add(mip_data);
	::memcpy(mip_data->Data, stream.data() + sizeof(astc_header), dataLen);

	return true;
}
bool Image::SaveToASTCFile(const String& path)
{
	return false;
}
#undef MAGIC_FILE_CONSTANT
DC_END_NAMESPACE