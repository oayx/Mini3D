
/*****************************************************************************
* Author： hannibal
* Date：2020/8/29
* Description：
*****************************************************************************/
#pragma once

#include "UIView.h"

DC_BEGIN_NAMESPACE
class Texture;
/********************************************************************/
enum class UIImageType
{
	//Displays the full Image.
	Simple = 0,
	//Displays the Image as a 9-sliced graphic.
	Sliced = 1,
	//Displays only a portion of the Image.
	Filled = 2,
};
DECLARE_ENUM_OPERATORS(UIImageType);

enum class UIImageFillMethod
{
	Horizontal = 0,
	Vertical,
};
DECLARE_ENUM_OPERATORS(UIImageFillMethod);

enum class UIImageOriginHorizontal
{
	Left = 0,
	Right,
};
DECLARE_ENUM_OPERATORS(UIImageOriginHorizontal);

enum class UIImageOriginVertical
{
	Bottom = 0,
	Top,
};
DECLARE_ENUM_OPERATORS(UIImageOriginVertical);

class ENGINE_DLL UIImage : public UIView
{
	friend class UICanvas;
	friend class GameObject;
	DECLARE_OBJECT_CLONE;
	DECLARE_OBJECT_SERIALIZE(UIImage);
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(UIImage);
	FRIEND_CONSTRUCT_DESTRUCT(UIImage);
	BEGIN_DERIVED_REFECTION_TYPE(UIImage, UIView)
		CTORS(DEFAULT_CTOR(UIImage))
		ADD_EDITOR_COMPONENT("UI/Image")
	END_DERIVED_REFECTION_TYPE;

public:
	virtual void	OnDrawEditor()override;
	virtual bool	FillMesh(VariablePrimitive* primitive, Material* material, int& vxt_offset, int& idx_offset)override;
	virtual void	SetNativeSize();

	void			SetAtlas(const String& file);
	void			SetAtlas(const String& file, const String& name);
	virtual String	GetAtlasFile()override;
	const String&	GetImageName()const { return _imageName; }

	void			SetImageType(UIImageType type) { _imageType = type; this->SetNeedRebuild(); }
	UIImageType		GetImageType()const { return _imageType; }
	void			SetImageBorder(const Vector4& border) { _imageBorder = border; this->SetNeedRebuild();}
	const Vector4&	GetImageBorder()const { return _imageBorder; }
	void			SetFillMethod(UIImageFillMethod method, int origin) {_fillMethod = method; _fillOrigin = origin; this->SetNeedRebuild();}
	void			SetFillAmount(float amount) { _fillAmount = Math::Clamp01(amount); this->SetNeedRebuild(); }
	float			GetFillAmount()const { return _fillAmount; }

protected:
	void			OnDrawEditor_Image();
	void			OnDrawEditor_ImageType();

private:
	bool			FillSimpleMesh(VariablePrimitive* primitive, int& vxt_offset, int& idx_offset);
	bool			FillSlicedMesh(VariablePrimitive* primitive, int& vxt_offset, int& idx_offset);
	bool			FillHorizontalMesh(VariablePrimitive* primitive, int& vxt_offset, int& idx_offset);
	bool			FillVerticalMesh(VariablePrimitive* primitive, int& vxt_offset, int& idx_offset);

protected:
	String				_atlasFile = "";
	String				_imageName = "";
	UIImageType			_imageType = UIImageType::Simple;
	Vector4				_imageBorder = Vector4::zero;
	UIImageFillMethod	_fillMethod = UIImageFillMethod::Horizontal;
	int					_fillOrigin = 0;
	float				_fillAmount = 1.0f;
};
DC_END_NAMESPACE
