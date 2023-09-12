 
/*****************************************************************************************************/
// @author hannibal
// @date   2021/08/3
// @brief  图片-缓存器
/*****************************************************************************************************/
#pragma once

#include "core/Object.h"

DC_BEGIN_NAMESPACE
class Image;
/********************************************************************/
class ImageManager Final : public object
{
	friend class Image;
	friend class Application;
	typedef Map<String, Image*> Images;
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(ImageManager);
	BEGIN_REFECTION_TYPE(ImageManager)
	END_FINAL_REFECTION_TYPE;
	
	static void Destroy();

public:
	static bool   AddImage(const String& file, Image* image);
	static Image* GetImage(const String& file);
	static void   DestroyImage(const String& file);

private:
	static Images _images;
};
DC_END_NAMESPACE
