#include "ColorConverter.h"
 

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_REFECTION_TYPE(ColorConverter);
void ColorConverter::FromR5G5B5A1toRGB8(const void* sP, int sN, void* dP)
{
	ushort* sB = (ushort*)sP;
	byte * dB = (byte *)dP;
	for (int x = 0; x < sN; ++x)
	{
		dB[2] = (*sB & 0x7c00) >> 7;
		dB[1] = (*sB & 0x03e0) >> 2;
		dB[0] = (*sB & 0x1f) << 3;

		sB += 1;
		dB += 3;
	}
}
void ColorConverter::FromR5G5B5A1toBGR8(const void* sP, int sN, void* dP)
{
	ushort* sB = (ushort*)sP;
	byte * dB = (byte *)dP;
	for (int x = 0; x < sN; ++x)
	{
		dB[0] = (*sB & 0x7c00) >> 7;
		dB[1] = (*sB & 0x03e0) >> 2;
		dB[2] = (*sB & 0x1f) << 3;

		sB += 1;
		dB += 3;
	}
}
void ColorConverter::FromR5G5B5A1toRGBA8(const void* sP, int sN, void* dP)
{
	ushort* sB = (ushort*)sP;
	uint* dB = (uint*)dP;
	for (int x = 0; x < sN; ++x)
		*dB++ = R5G5B5A1toRGBA8(*sB++);
}
void ColorConverter::FromR5G5B5A1toR5G5B5A1(const void* sP, int sN, void* dP)
{
	::memcpy(dP, sP, sN * 2);
}
void ColorConverter::FromR5G5B5A1toR5G6B5(const void* sP, int sN, void* dP)
{
	ushort* sB = (ushort*)sP;
	ushort* dB = (ushort*)dP;
	for (int x = 0; x < sN; ++x)
		*dB++ = R5G5B5A1toR5G6B5(*sB++);
}
void ColorConverter::FromRGBA8toRGB8(const void* sP, int sN, void* dP)
{
	byte* sB = (byte*)sP;
	byte* dB = (byte*)dP;
	for (int x = 0; x < sN; ++x)
	{
		// sB[3] is alpha
		dB[0] = sB[2];
		dB[1] = sB[1];
		dB[2] = sB[0];

		sB += 4;
		dB += 3;
	}
}
void ColorConverter::FromRGBA8toBGR8(const void* sP, int sN, void* dP)
{
	byte* sB = (byte*)sP;
	byte* dB = (byte*)dP;
	for (int x = 0; x < sN; ++x)
	{
		// sB[3] is alpha
		dB[0] = sB[0];
		dB[1] = sB[1];
		dB[2] = sB[2];

		sB += 4;
		dB += 3;
	}
}
void ColorConverter::FromRGBA8toRGBA8(const void* sP, int sN, void* dP)
{
	::memcpy(dP, sP, sN * 4);
}
void ColorConverter::FromRGBA8toR5G5B5A1(const void* sP, int sN, void* dP)
{
	uint* sB = (uint*)sP;
	ushort* dB = (ushort*)dP;
	for (int x = 0; x < sN; ++x)
		*dB++ = RGBA8toR5G5B5A1(*sB++);
}
void ColorConverter::FromRGBA8toR5G6B5(const void* sP, int sN, void* dP)
{
	byte* sB = (byte *)sP;
	ushort* dB = (ushort*)dP;
	for (int x = 0; x < sN; ++x)
	{
		int r = sB[2] >> 3;
		int g = sB[1] >> 2;
		int b = sB[0] >> 3;

		dB[0] = (r << 11) | (g << 5) | (b);

		sB += 4;
		dB += 1;
	}
}
void ColorConverter::FromRGB8toRGB8(const void* sP, int sN, void* dP)
{
	::memcpy(dP, sP, sN * 3);
}
void ColorConverter::FromRGB8toRGBA8(const void* sP, int sN, void* dP)
{
	byte* sB = (byte* )sP;
	uint* dB = (uint*)dP;
	for (int x = 0; x < sN; ++x)
	{
		*dB = 0xff000000 | (sB[2]<<16) | (sB[1]<<8) | sB[0];

		sB += 3;
		++dB;
	}
}
void ColorConverter::FromRGB8toR5G5B5A1(const void* sP, int sN, void* dP)
{
	byte* sB = (byte *)sP;
	ushort* dB = (ushort*)dP;
	for (int x = 0; x < sN; ++x)
	{
		int r = sB[0] >> 3;
		int g = sB[1] >> 3;
		int b = sB[2] >> 3;

		dB[0] = (0x8000) | (b << 10) | (g << 5) | (r);

		sB += 3;
		dB += 1;
	}
}
void ColorConverter::FromRGB8toBGR8(const void* sP, int sN, void* dP)
{
	byte* sB = (byte*)sP;
	byte* dB = (byte*)dP;
	for (int x = 0; x < sN; ++x)
	{
		dB[0] = sB[2];
		dB[1] = sB[1];
		dB[2] = sB[0];

		sB += 3;
		dB += 3;
	}
}
void ColorConverter::FromRGB8toBGRA8(const void* sP, int sN, void* dP)
{
	byte* sB = (byte* )sP;
	uint* dB = (uint*)dP;
	for (int x = 0; x < sN; ++x)
	{
		*dB = 0xff000000 | (sB[0]<<16) | (sB[1]<<8) | sB[2];

		sB += 3;
		++dB;
	}
}
void ColorConverter::FromRGBA8toBGRA8(const void* sP, int sN, void* dP)
{
	byte* sB = (byte*)sP;
	byte* dB = (byte*)dP;
	for (int x = 0; x < sN; ++x)
	{
		dB[0] = sB[2];
		dB[1] = sB[1];
		dB[2] = sB[0];
		dB[3] = sB[3];

		sB += 4;
		dB += 4;
	}
}
void ColorConverter::FromRGB8toR5G6B5(const void* sP, int sN, void* dP)
{
	byte* sB = (byte *)sP;
	ushort* dB = (ushort*)dP;
	for (int x = 0; x < sN; ++x)
	{
		int r = sB[2] >> 3;
		int g = sB[1] >> 2;
		int b = sB[0] >> 3;

		dB[0] = (b << 11) | (g << 5) | (r);

		sB += 3;
		dB += 1;
	}
}
void ColorConverter::FromR5G6B5toR5G6B5(const void* sP, int sN, void* dP)
{
	::memcpy(dP, sP, sN * 2);
}
void ColorConverter::FromR5G6B5toRGB8(const void* sP, int sN, void* dP)
{
	ushort* sB = (ushort*)sP;
	byte * dB = (byte *)dP;
	for (int x = 0; x < sN; ++x)
	{
		dB[2] = (*sB & 0xf800) >> 8;
		dB[1] = (*sB & 0x07e0) >> 3;
		dB[0] = (*sB & 0x001f) << 3;

		sB += 1;
		dB += 3;
	}
}
void ColorConverter::FromR5G6B5toBGR8(const void* sP, int sN, void* dP)
{
	ushort* sB = (ushort*)sP;
	byte * dB = (byte *)dP;
	for (int x = 0; x < sN; ++x)
	{
		dB[0] = (*sB & 0xf800) >> 8;
		dB[1] = (*sB & 0x07e0) >> 3;
		dB[2] = (*sB & 0x001f) << 3;

		sB += 1;
		dB += 3;
	}
}
void ColorConverter::FromR5G6B5toRGBA8(const void* sP, int sN, void* dP)
{
	ushort* sB = (ushort*)sP;
	uint* dB = (uint*)dP;
	for (int x = 0; x < sN; ++x)
		*dB++ = R5G6B5toRGBA8(*sB++);
}
void ColorConverter::FromR5G6B5toR5G5B5A1(const void* sP, int sN, void* dP)
{
	ushort* sB = (ushort*)sP;
	ushort* dB = (ushort*)dP;
	for (int x = 0; x < sN; ++x)
		*dB++ = R5G6B5toR5G5B5A1(*sB++);
}
bool ColorConverter::ConvertFormat(const void* sP, ColorFormat sF, int sN, void* dP, ColorFormat dF)
{
	switch (sF)
	{
		case ColorFormat::R5G5B5A1:
			switch (dF)
			{
				case ColorFormat::R5G5B5A1:
					FromR5G5B5A1toR5G5B5A1(sP, sN, dP);
				break;
				case ColorFormat::R5G6B5:
					FromR5G5B5A1toR5G6B5(sP, sN, dP);
				break;
				case ColorFormat::R8G8B8A8:
					FromR5G5B5A1toRGBA8(sP, sN, dP);
				break;
				case ColorFormat::R8G8B8:
					FromR5G5B5A1toRGB8(sP, sN, dP);
				break;
				default:return false;
			}
		break;
		case ColorFormat::R5G6B5:
			switch (dF)
			{
				case ColorFormat::R5G5B5A1:
					FromR5G6B5toR5G5B5A1(sP, sN, dP);
				break;
				case ColorFormat::R5G6B5:
					FromR5G6B5toR5G6B5(sP, sN, dP);
				break;
				case ColorFormat::R8G8B8A8:
					FromR5G6B5toRGBA8(sP, sN, dP);
				break;
				case ColorFormat::R8G8B8:
					FromR5G6B5toRGB8(sP, sN, dP);
				break;
				default:return false;
			}
		break;
		case ColorFormat::R8G8B8A8:
			switch (dF)
			{
				case ColorFormat::R5G5B5A1:
					FromRGBA8toR5G5B5A1(sP, sN, dP);
				break;
				case ColorFormat::R5G6B5:
					FromRGBA8toR5G6B5(sP, sN, dP);
				break;
				case ColorFormat::R8G8B8A8:
					FromRGBA8toRGBA8(sP, sN, dP);
				break;
				case ColorFormat::R8G8B8:
					FromRGBA8toRGB8(sP, sN, dP);
				break;
				default:return false;
			}
		break;
		case ColorFormat::R8G8B8:
			switch (dF)
			{
				case ColorFormat::R5G5B5A1:
					FromRGB8toR5G5B5A1(sP, sN, dP);
				break;
				case ColorFormat::R5G6B5:
					FromRGB8toR5G6B5(sP, sN, dP);
				break;
				case ColorFormat::R8G8B8A8:
					FromRGB8toRGBA8(sP, sN, dP);
				break;
				case ColorFormat::R8G8B8:
					FromRGB8toRGB8(sP, sN, dP);
				break;
				default:return false;
			}
		break;
		default:return false;
	}
	return true;
}
DC_END_NAMESPACE
