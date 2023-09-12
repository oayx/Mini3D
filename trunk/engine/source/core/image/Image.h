 
/*****************************************************************************
* Author： hannibal
* Date：2020/3/16
* Description： 图片数据(支持bmp,jpg,tag,png,gif,hdr,raw,dds,pkm,astc的读写)
*****************************************************************************/
#pragma once

#include "core/Object.h"
#include "runtime/CommonDefine.h"

DC_BEGIN_NAMESPACE
class ENGINE_DLL ImageMipData Final : public object
{
public:
	uint RowPitch = 0;
	uint Width = 0;
	uint Height = 0;
	uint Size = 0;
	byte* Data = nullptr;

	ImageMipData() {}
	ImageMipData(uint row_pitch, uint width, uint height, uint size)
		: RowPitch(row_pitch), Width(width), Height(height), Size(size)
	{
		Data = NewArray<byte>(Size);
	}
	~ImageMipData()
	{
		if (Data)
		{
			DeleteArray(Data);
			Data = nullptr;
		}
	}

	template<typename T>
	T& Get(int x, int y)
	{
		return *(T*)(Data + y * RowPitch + x * sizeof(T));
	}

	template<typename T>
	const T& Get(int x, int y) const
	{
		return *(const T*)(Data + y * RowPitch + x * sizeof(T));
	}

	operator byte*()
	{
		return Data;
	}
};
/********************************************************************/
class ENGINE_DLL Image Final : public Object
{
	typedef Vector<ImageMipData*> ImageDatas; 
	FRIEND_CONSTRUCT_DESTRUCT(Image);
	DECLARE_OBJECT_CLONE;
	DISALLOW_COPY_ASSIGN(Image);
	BEGIN_DERIVED_REFECTION_TYPE(Image, Object)
	END_DERIVED_REFECTION_TYPE;

	Image() {}
	Image(ColorFormat format, const iSize& size, bool round);

public:
	static Image*	 Create(ColorFormat format, const iSize& size);
	static Image*	 Create(ColorFormat format, const iSize& size, bool round);
	static Image*	 Create(const String& file, bool mipmap);
	static Image*	 CreateEditor(const String& file);//编辑器是在PersistentDataPath目录下加载资源,且只会加载出非压缩格式
	static ImageType GetImageType(const String& file);
	~Image();

public:
	bool		LoadFromFile(const String& file, bool mipmap, bool no_compress);
	bool		GenerateMipMap(int levels = 0);
	bool		SaveToFile(const String& file);

	byte* 		Lock(int level = 0) { return _imageData[level]->Data; }
	void  		Unlock() {}
	const byte* Data(int level = 0)const { return _imageData[level]->Data; }
	ImageMipData* GetMipData(int level = 0)const { return _imageData[level]; }

	const		String& GetFileName()const { return _file; }
	int 		GetWidth() const { return _size.width; }
	int 		GetHeight() const { return _size.height; }
	int 		GetPitch() const { return _imageData[0]->RowPitch; }
	uint 		GetSize() const { return _imageData[0]->Size; }
	ColorFormat GetFormat() const { return _format; }
	bool		IsCube()const { return _isCube; }
	int			MipLevels()const { return _imageData.Size(); }

	Color 		GetPixel(uint x, uint y) const;
	void  		SetPixel(uint x, uint y, const Color &color, bool blend = false );

	bool		Resize(uint w, uint h);//调整大小，只针对非压缩格式，而且levels为1的情况
	void		Fill(const Color &color);
	void		Fill(const byte* by, uint size);
	void		UpDownTransfer();//上下颠倒
	void		Clear();

private:
	int			GenerateMipMapLevels();
	bool		LoadFromSTBFile(const String& file, ImageType type);
	bool		LoadFromDDSFile(const String& file, bool mipmap);
	bool		LoadFromCubeFile(const String& file, bool mipmap);
	bool		LoadFromETCFile(const String& file, bool mipmap);
	bool		LoadFromASTCFile(const String& file, bool mipmap);
	bool		SaveToSTBFile(const String& file, ImageType type);
	bool		SaveToDDSFile(const String& file);
	bool		SaveToETCFile(const String& file);
	bool		SaveToASTCFile(const String& file);

private:
	String		_file = "";
	ImageDatas	_imageData;					//数据
	iSize		_size;							//大小
	ColorFormat _format = ColorFormat::UNKNOWN;//格式
	bool		_isCube = false;				//是否立方体
	bool		_isStatic = false;				//是否静态(从文件读取)，如果是静态不能修改
};//Image
DC_END_NAMESPACE