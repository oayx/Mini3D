#include "QRCode.h"
#include "external/qrencode/qrencode.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
unsigned char* QRCode::Generate(const String& content, int version, int* ref_width, int* ref_data_bytes)
{
	QRcode* qrcode = QRcode_encodeString(content.c_str(), version, QR_ECLEVEL_H, QR_MODE_8, 1);
	if (qrcode)
	{
		unsigned char*  rgb_data, *source_data, *dest_data;
		int width = qrcode->width; 
		int width_adjusted = width * 8 * 3;
		if (width_adjusted % 4)
			width_adjusted = (width_adjusted / 4 + 1) * 4;
		int data_bytes = width_adjusted * width * 8;
		rgb_data = (unsigned char*)Memory::Alloc(data_bytes);
		memset(rgb_data, 0xff, data_bytes);
		source_data = qrcode->data;
		for (int y = 0; y < width; y++) 
		{
			dest_data = rgb_data + width_adjusted * y * 8;
			for (int x = 0; x < width; x++)
			{
				if (*source_data & 1)
				{
					for (int i = 0; i < 8; i++)
					{
						for (int n = 0; n < 8; n++)
						{
							//以下三行是设置三基色，三基色都设置为0x00，则生成的二维码图片就是黑色的了，要什么颜色自己调整
							*(dest_data + n * 3 + width_adjusted * i) = 0x00;
							*(dest_data + 1 + n * 3 + width_adjusted * i) = 0x00;
							*(dest_data + 2 + n * 3 + width_adjusted * i) = 0x00;
						}
					}
				}
				dest_data += 3 * 8;
				source_data++;
			}
		}
		QRcode_free(qrcode);
		*ref_width = width;
		*ref_data_bytes = data_bytes;
		return rgb_data;
	}
	else
	{
		Debuger::Error("QRCodeGenerate::Get QRcode_encodeString error");
		return nullptr;
	}
}
bool QRCode::SaveToPicture(const String& content, int version, const String& path)
{
	int width = 0, data_bytes = 0;
	unsigned char*  rgb_data = Generate(content, version, &width, &data_bytes);
	if (rgb_data == nullptr)return false;
	bool result = false;

#if DC_PLATFORM_WIN32
	FILE* file = nullptr;
	// Output the bmp file
	if (file = fopen(path.c_str(), "wb"))
	{
		BITMAPFILEHEADER kFileHeader;
		kFileHeader.bfType = 0x4d42;  // "BM"
		kFileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + data_bytes;
		kFileHeader.bfReserved1 = 0;
		kFileHeader.bfReserved2 = 0;
		kFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
		BITMAPINFOHEADER kInfoHeader;
		kInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
		kInfoHeader.biWidth = width * 8;
		kInfoHeader.biHeight = -((int)width * 8);
		kInfoHeader.biPlanes = 1;
		kInfoHeader.biBitCount = 24;
		kInfoHeader.biCompression = BI_RGB;
		kInfoHeader.biSizeImage = 0;
		kInfoHeader.biXPelsPerMeter = 0;
		kInfoHeader.biYPelsPerMeter = 0;
		kInfoHeader.biClrUsed = 0;
		kInfoHeader.biClrImportant = 0;

		fwrite(&kFileHeader, sizeof(BITMAPFILEHEADER), 1, file);
		fwrite(&kInfoHeader, sizeof(BITMAPINFOHEADER), 1, file);
		fwrite(rgb_data, sizeof(unsigned char), data_bytes, file);
		printf("qrcode has generated in %s\n", path.c_str());
		fclose(file);
		result = true;
	}
	else
	{
		Debuger::Error("QRCodeGenerate::SaveToPicture Unable to open file:%s", path.c_str());
	}
#endif

	Memory::Free(rgb_data);
	return result;
}
DC_END_NAMESPACE
