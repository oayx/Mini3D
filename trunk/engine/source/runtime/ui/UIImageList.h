
/*****************************************************************************
* Author： hannibal
* Date：2020/9/9
* Description：image list
*****************************************************************************/
#pragma once

#include "UIImage.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL UIImageList Final : public UIImage
{
	typedef Vector<std::pair<String, String>> Atlases;
	friend class UICanvas;
	friend class GameObject;
	DECLARE_OBJECT_CLONE;
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(UIImageList);
	FRIEND_CONSTRUCT_DESTRUCT(UIImageList);
	BEGIN_DERIVED_REFECTION_TYPE(UIImageList, UIImage)
		CTORS(DEFAULT_CTOR(UIImageList))
	END_DERIVED_REFECTION_TYPE;

protected:
	virtual void Start()override;

public:
	void		AddImage(const String& file, const String& name) { _atlases.Add({ file, name }); }
	void		SetImage(int index);
	int			GetImage()const { return _imgIndex; }

	void		SetNativeSize(bool b) { _setNativeSize = b; }

protected:
	bool		_setNativeSize = true;

	int			_imgIndex = 0;
	Atlases		_atlases;
};
DC_END_NAMESPACE
