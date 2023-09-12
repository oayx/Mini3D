#include "UIImageList.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(UIImageList, UIImage);
Object* UIImageList::Clone(Object* new_obj)
{
	base::Clone(new_obj);
	UIImageList* obj = dynamic_cast<UIImageList*>(new_obj);
	if (!obj)return new_obj;

	for (const auto& p : _atlases)
	{
		obj->AddImage(p.first, p.second);
	}

	obj->SetNativeSize(_setNativeSize);
	obj->SetImage(_imgIndex);

	return obj;
}
void UIImageList::Start()
{
	base::Start();
	SetImage(_imgIndex);
}
void UIImageList::SetImage(int index)
{
	AssertEx(index >= 0 && index < _atlases.Size(), "image index out range");
	_imgIndex = index;
	auto atlas = _atlases[index];
	SetAtlas(atlas.first, atlas.second);
	if (_setNativeSize)
	{
		base::SetNativeSize();
	}
}
DC_END_NAMESPACE
