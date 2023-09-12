 
/*****************************************************************************
* Author： hannibal
* Date：2020/3/16
* Description： 格式转换
*****************************************************************************/
#pragma once

#include "core/BaseType.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
//颜色格式
enum class ColorFormat : byte
{
	UNKNOWN = 0,
	//规范化纹理：结果将处于[0,1]或_SNORM[-1,1]		输入数据			Bits per pixel
	R8,												//8位红色
	R5G6B5, B5G6R5,									//5R6G5B
	R5G5B5A1, B5G5R5A1,								//5R5G5BA1
	R8G8B8, B8G8R8,
	R8G8B8A8, B8G8R8A8,
	R16,											//16位归一化纹理
	//浮点纹理格式
	R16F,											//16位红色
	RG16F,
	RGB16F,											//16R16G16B										
	RGBA16F,										//16R16G16B16A
	R32F,											//32位红色
	RG32F,
	RGB32F,											//32R32G32B	
	RGBA32F,										//32R32G32B32A
	//深度纹理格式
	DEPTH16,
	DEPTH24,
	DEPTH32F, DEPTH24_STENCIL8,
	//Android
	ETC_RGB4,										//RGB				4
	ETC2_RGB4,										//RGB				4
	ETC2_RGBA8,										//RGBA				8
	//Mac+iOS
	PVRTC_RGB2, 									//RGB				2
	PVRTC_RGBA2,									//RGBA				2
	PVRTC_RGB4, 									//RGB				4
	PVRTC_RGBA4,									//RGBA				4
	//Windows
	DXT1,											//RGB(A)			4		//将每4×4个像素块视为一个压缩单位，压缩后的4×4个像素块占用64位，其中有2个16位的RGB颜色和16个2位索引；其中的两个RGB颜色负责表示所在压缩的4×4像素块中颜色的两个极端值，然后通过线性插值我们可以再计算出两个中间颜色值，而16个2位索引则表明了这4×4个像素块所在像素的颜色值
	DXT3,											//RGBA				8		//再DXT1的基础上，通过另附加64位数据也就是每个像素4位来表示他们的Alpha透明信息；压缩格式可以提供4:1的压缩比例, 可以支持4 bit 的 Alpha 通道, 主要用于Alpha通道较锐利, 对比强烈的材质, 比如镂空, 以及部分半透材质等
	DXT5,											//RGBA				8		//与DXT3不同的是Alpha信息是通过线性插值计算所得，类似于DXT1的颜色信息。同样的，每4×4的像素块的透明信息占用64位，所不同的是，64位中采用了2个8位的alpha值和16个3位的索引值，既然每个像素的索引占3位，那么可以表示8种不同的透明状态
	//ASTC
	ASTC_RGB_4x4,									//RGB4*4			8
	ASTC_RGB_5x5,									//					5.12
	ASTC_RGB_6x6,									//					3.56
	ASTC_RGB_8x8,									//					2
	ASTC_RGB_10x10,									//					1.28
	ASTC_RGB_12x12,									//					0.89
	ASTC_RGBA_4x4,									//RGBA4*4			8
	ASTC_RGBA_5x5,									//					5.12
	ASTC_RGBA_6x6,									//					3.56
	ASTC_RGBA_8x8,									//					2
	ASTC_RGBA_10x10,								//					1.28
	ASTC_RGBA_12x12,								//					0.89
};
DECLARE_ENUM_OPERATORS(ColorFormat);

//颜色模式转换
//1.DX9默认使用BGRA颜色模式，OpenGL是RGBA
//2.png默认读出来的是RGBA
union RGBA
{
	struct { unsigned char r, g, b, a; };
	unsigned int u;
};
inline uint RGBAtoBGRA(uint color)
{
	RGBA tmp;
	tmp.u = color;
	byte b = tmp.b;
	tmp.b = tmp.r;
	tmp.r = b;
	return tmp.u;
}
inline uint BGRAtoRGBA(uint color)
{
	RGBA tmp;
	tmp.u = color;
	byte b = tmp.b;
	tmp.b = tmp.r;
	tmp.r = b;
	return tmp.u;
}

inline bool IsTransparentColor(const ColorFormat format)
{
	switch (format)
	{
	case ColorFormat::R5G5B5A1:
	case ColorFormat::B5G5R5A1:
	case ColorFormat::R8G8B8A8:
	case ColorFormat::B8G8R8A8:
	case ColorFormat::RGBA16F:
	case ColorFormat::RGBA32F:
	case ColorFormat::DXT3:
	case ColorFormat::DXT5:
		return true;
	default:
		return false;
	}
}
inline bool IsCompressedFormat(const ColorFormat format)
{
	if (format == ColorFormat::ETC_RGB4 || format == ColorFormat::ETC2_RGB4 || format == ColorFormat::ETC2_RGBA8 ||
		format == ColorFormat::PVRTC_RGB2 || format == ColorFormat::PVRTC_RGBA2 || format == ColorFormat::PVRTC_RGB4 || format == ColorFormat::PVRTC_RGBA4 ||
		format == ColorFormat::DXT1 || format == ColorFormat::DXT3 || format == ColorFormat::DXT5 ||
		format == ColorFormat::ASTC_RGB_4x4 || format == ColorFormat::ASTC_RGB_5x5 || format == ColorFormat::ASTC_RGB_6x6 || format == ColorFormat::ASTC_RGB_8x8 || format == ColorFormat::ASTC_RGB_10x10 || format == ColorFormat::ASTC_RGB_12x12 ||
		format == ColorFormat::ASTC_RGBA_4x4 || format == ColorFormat::ASTC_RGBA_5x5 || format == ColorFormat::ASTC_RGBA_6x6 || format == ColorFormat::ASTC_RGBA_8x8 || format == ColorFormat::ASTC_RGBA_10x10 || format == ColorFormat::ASTC_RGBA_12x12)
	{
		return true;
	}
	return false;
}
inline uint BitsPerPixel(const ColorFormat format)
{
	switch (format)
	{
	case ColorFormat::R8:
		return 8;
	case ColorFormat::R5G6B5:
	case ColorFormat::R5G5B5A1:
	case ColorFormat::B5G6R5:
	case ColorFormat::B5G5R5A1:
	case ColorFormat::R16:
	case ColorFormat::R16F:
	case ColorFormat::DEPTH16:
		return 16;
	case ColorFormat::R8G8B8:
	case ColorFormat::B8G8R8:
	case ColorFormat::DEPTH24:
		return 24;
	case ColorFormat::R8G8B8A8:
	case ColorFormat::B8G8R8A8:
	case ColorFormat::RG16F:
	case ColorFormat::R32F:
	case ColorFormat::DEPTH32F:
	case ColorFormat::DEPTH24_STENCIL8:
		return 32;
	case ColorFormat::RGB16F:
		return 48;
	case ColorFormat::RGBA16F:
	case ColorFormat::RG32F:
		return 64;
	case ColorFormat::RGB32F:
		return 96;
	case ColorFormat::RGBA32F:
		return 128;
	case ColorFormat::DXT1:
		return 4;
	case ColorFormat::DXT3:
	case ColorFormat::DXT5:
		return 8;
	case ColorFormat::ETC_RGB4:
	case ColorFormat::ETC2_RGB4:
		return 4;
	case ColorFormat::ETC2_RGBA8:
		return 8;
	default:
		AssertEx(false, "Cannot find color format:%d", format);
		return 0;
	}
}
inline uint BytesRowPitch(const ColorFormat format, int width)
{
	bool bc = false;
	uint bpe = 0;
	switch (format)
	{
	case ColorFormat::DXT1:
	case ColorFormat::ETC_RGB4:
	case ColorFormat::ETC2_RGB4:
		bc = true;
		bpe = 8;
		break;
	case ColorFormat::DXT3:
	case ColorFormat::DXT5:
	case ColorFormat::ETC2_RGBA8:
		bc = true;
		bpe = 16;
		break;
	default:
		break;
	}
	if (bc)
	{
		uint numBlocksWide = std::max<uint>(1u, (uint(width) + 3u) / 4u);
		return numBlocksWide * bpe;
	}
	else
	{
		uint bpp = BitsPerPixel(format);
		return (uint(width) * bpp + 7u) / 8u; // round up to nearest byte
	}
}
inline uint TotalBytes(const ColorFormat format, int width, int height)
{
	bool bc = false;
	uint bpe = 0;
	switch (format)
	{
	case ColorFormat::DXT1:
	case ColorFormat::ETC_RGB4:
	case ColorFormat::ETC2_RGB4:
		bc = true;
		bpe = 8;
		break;
	case ColorFormat::DXT3:
	case ColorFormat::DXT5:
	case ColorFormat::ETC2_RGBA8:
		bc = true;
		bpe = 16;
		break;
	default:
		break;
	}
	if (bc)
	{
		uint numBlocksWide = std::max<uint>(1u, (uint(width) + 3u) / 4u);
		uint numBlocksHigh = std::max<uint>(1u, (uint(height) + 3u) / 4u);
		return numBlocksWide * numBlocksHigh * bpe;
	}
	else
	{
		uint bpp = BitsPerPixel(format);
		uint row_pitch = (uint(width) * bpp + 7u) / 8u; // round up to nearest byte
		return row_pitch * height;
	}
}
inline const char* GetImageFormatname(const ColorFormat format)
{
	switch (format)
	{
	case ColorFormat::R8:return "R8";
	case ColorFormat::R5G6B5:return "R5G6B5";
	case ColorFormat::B5G6R5:return "B5G6R5";
	case ColorFormat::R5G5B5A1:return "R5G5B5A1";
	case ColorFormat::B5G5R5A1:return "B5G5R5A1";
	case ColorFormat::R8G8B8:return "R8G8B8";
	case ColorFormat::B8G8R8:return "B8G8R8";
	case ColorFormat::R8G8B8A8:return "R8G8B8A8";
	case ColorFormat::B8G8R8A8:return "B8G8R8A8";
	case ColorFormat::R16:return "R16";
	case ColorFormat::R16F:return "R16F";
	case ColorFormat::RG16F:return "RG16F";
	case ColorFormat::RGB16F:return "RGB16F";
	case ColorFormat::RGBA16F:return "RGBA16F";
	case ColorFormat::R32F:return "R32F";
	case ColorFormat::RG32F:return "RG32F";
	case ColorFormat::RGB32F:return "RGB32F";
	case ColorFormat::RGBA32F:return "RGBA32F";
	case ColorFormat::DEPTH16:return "DEPTH16";
	case ColorFormat::DEPTH24:return "DEPTH24";
	case ColorFormat::DEPTH32F:return "DEPTH32F";
	case ColorFormat::DEPTH24_STENCIL8:return "DEPTH24_STENCIL8";
	case ColorFormat::ETC_RGB4:return "ETC_RGB4";
	case ColorFormat::ETC2_RGB4:return "ETC2_RGB4";
	case ColorFormat::ETC2_RGBA8:return "ETC2_RGBA8";
	case ColorFormat::PVRTC_RGB2:return "PVRTC_RGB2";
	case ColorFormat::PVRTC_RGB4:return "PVRTC_RGB4";
	case ColorFormat::PVRTC_RGBA2:return "PVRTC_RGBA2";
	case ColorFormat::PVRTC_RGBA4:return "PVRTC_RGBA4";
	case ColorFormat::DXT1:return "DXT1";
	case ColorFormat::DXT3:return "DXT3";
	case ColorFormat::DXT5:return "DXT5";
	case ColorFormat::ASTC_RGB_4x4:return "ASTC_RGB_4x4";
	case ColorFormat::ASTC_RGB_5x5:return "ASTC_RGB_5x5";
	case ColorFormat::ASTC_RGB_6x6:return "ASTC_RGB_6x6";
	case ColorFormat::ASTC_RGB_8x8:return "ASTC_RGB_8x8";
	case ColorFormat::ASTC_RGB_10x10:return "ASTC_RGB_10x10";
	case ColorFormat::ASTC_RGB_12x12:return "ASTC_RGB_12x12";
	case ColorFormat::ASTC_RGBA_4x4:return "ASTC_RGBA_4x4";
	case ColorFormat::ASTC_RGBA_5x5:return "ASTC_RGBA_5x5";
	case ColorFormat::ASTC_RGBA_6x6:return "ASTC_RGBA_6x6";
	case ColorFormat::ASTC_RGBA_8x8:return "ASTC_RGBA_8x8";
	case ColorFormat::ASTC_RGBA_10x10:return "ASTC_RGBA_10x10";
	case ColorFormat::ASTC_RGBA_12x12:return "ASTC_RGBA_12x12";
	default:
		AssertEx(false, "Unknow color format:%d", format);
		return "Unknow format";
	}
}
// Converts a 32bit (RGBA8) color to a 16bit R5G5B5A1 color
inline ushort RGBA8toR5G5B5A1(uint color)
{
	return (ushort)((color & 0x80000000) >> 16 |
		(color & 0x00F80000) >> 9 |
		(color & 0x0000F800) >> 6 |
		(color & 0x000000F8) >> 3);
}

// Converts a 32bit (RGBA8) color to a 16bit R5G6B5 color
inline ushort RGBA8toR5G6B5(uint color)
{
	return (ushort)((color & 0x00F80000) >> 8 |
		(color & 0x0000FC00) >> 5 |
		(color & 0x000000F8) >> 3);
}
inline uint RGBA8toRGB8(uint color)
{
	return (color & 0x00FF0000) |
		(color & 0x0000FF00) |
		(color & 0x000000FF);
}

// Convert RGBA8 Color from R5G5B5A1 color
inline uint R5G5B5A1toRGBA8(ushort color)
{
	return (((-((int)color & 0x00008000) >> (int)31) & 0xFF000000) |
		((color & 0x00007C00) << 9) | ((color & 0x00007000) << 4) |
		((color & 0x000003E0) << 6) | ((color & 0x00000380) << 1) |
		((color & 0x0000001F) << 3) | ((color & 0x0000001C) >> 2)
		);
}

// Returns RGBA8 Color from R5G6B5 color
inline uint R5G6B5toRGBA8(ushort color)
{
	return 0xFF000000 |
		((color & 0xF800) << 8) |
		((color & 0x07E0) << 5) |
		((color & 0x001F) << 3);
}

// Returns R5G5B5A1 Color from R5G6B5 color
inline ushort R5G6B5toR5G5B5A1(ushort color)
{
	return 0x8000 | (((color & 0xFFC0) >> 1) | (color & 0x1F));
}

// Returns R5G6B5 Color from R5G5B5A1 color
inline ushort R5G5B5A1toR5G6B5(ushort color)
{
	return (((color & 0x7FE0) << 1) | (color & 0x1F));
}

// Pixel = dest * ( 1 - SourceAlpha ) + source * SourceAlpha
inline uint PixelBlend32(const uint c2, const uint c1)
{
	// alpha test
	uint alpha = c1 & 0xFF000000;

	if (0 == alpha)
		return c2;

	if (0xFF000000 == alpha)
	{
		return c1;
	}

	alpha >>= 24;

	// add highbit alpha, if ( alpha > 127 ) alpha += 1;
	alpha += (alpha >> 7);

	uint srcRB = c1 & 0x00FF00FF;
	uint srcXG = c1 & 0x0000FF00;

	uint dstRB = c2 & 0x00FF00FF;
	uint dstXG = c2 & 0x0000FF00;


	uint rb = srcRB - dstRB;
	uint xg = srcXG - dstXG;

	rb *= alpha;
	xg *= alpha;
	rb >>= 8;
	xg >>= 8;

	rb += dstRB;
	xg += dstXG;

	rb &= 0x00FF00FF;
	xg &= 0x0000FF00;

	return (c1 & 0xFF000000) | rb | xg;
}

/********************************************************************/
// 格式转换
class ENGINE_DLL ColorConverter Final : public object
{
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(ColorConverter);
	BEGIN_REFECTION_TYPE(ColorConverter)
	END_FINAL_REFECTION_TYPE;
	
public:
	static void FromR5G5B5A1toRGB8(const void* sP, int sN, void* dP);
	static void FromR5G5B5A1toBGR8(const void* sP, int sN, void* dP);
	static void FromR5G5B5A1toRGBA8(const void* sP, int sN, void* dP);
	static void FromR5G5B5A1toR5G5B5A1(const void* sP, int sN, void* dP);
	static void FromR5G5B5A1toR5G6B5(const void* sP, int sN, void* dP);

	static void FromRGBA8toRGB8(const void* sP, int sN, void* dP);
	static void FromRGBA8toBGR8(const void* sP, int sN, void* dP);
	static void FromRGBA8toRGBA8(const void* sP, int sN, void* dP);
	static void FromRGBA8toR5G5B5A1(const void* sP, int sN, void* dP);
	static void FromRGBA8toR5G6B5(const void* sP, int sN, void* dP);

	static void FromRGB8toRGB8(const void* sP, int sN, void* dP);
	static void FromRGB8toRGBA8(const void* sP, int sN, void* dP);
	static void FromRGB8toR5G5B5A1(const void* sP, int sN, void* dP);
	static void FromRGB8toR5G6B5(const void* sP, int sN, void* dP);
	static void FromRGB8toBGR8(const void* sP, int sN, void* dP);
	static void FromRGB8toBGRA8(const void* sP, int sN, void* dP);
	static void FromRGBA8toBGRA8(const void* sP, int sN, void* dP);

	static void FromR5G6B5toR5G6B5(const void* sP, int sN, void* dP);
	static void FromR5G6B5toRGB8(const void* sP, int sN, void* dP);
	static void FromR5G6B5toBGR8(const void* sP, int sN, void* dP);
	static void FromR5G6B5toRGBA8(const void* sP, int sN, void* dP);
	static void FromR5G6B5toR5G5B5A1(const void* sP, int sN, void* dP);

	static bool ConvertFormat(const void* sP, ColorFormat sF, int sN,void* dP, ColorFormat dF);
};//ColorConverter
DC_END_NAMESPACE
