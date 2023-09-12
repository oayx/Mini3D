 #include "ImageManager.h"
#include "Image.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
ImageManager::Images ImageManager::_images;
IMPL_REFECTION_TYPE(ImageManager);
void ImageManager::Destroy()
{
	for (auto obj : _images)
	{
		SAFE_RELEASE(obj.second);
	}
	_images.Clear();
}
bool ImageManager::AddImage(const String& file, Image* image)
{
	AssertEx(!_images.Contains(file), "");
	_images.Add(file, image);
	image->Retain();
	return true;
}
Image* ImageManager::GetImage(const String& file)
{
	Images::const_iterator it = _images.Find(file);
	if (it != _images.end())
	{
		return it->second;
	}
	return nullptr;
}
void ImageManager::DestroyImage(const String& file)
{
	Images::iterator it = _images.Find(file);
	if (it != _images.end())
	{
		SAFE_RELEASE(it->second);
		_images.Remove(it);
	}
}
DC_END_NAMESPACE