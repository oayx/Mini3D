 
/*****************************************************************************
* Author： hannibal
* Date：2009/11/30
* Description： 渲染纹理单元
*****************************************************************************/
#pragma once

#include "RenderDefine.h"
#include "external/tinyxml2/tinyxml2.h"

DC_BEGIN_NAMESPACE
class Pass;
class Texture;
/********************************************************************/
class TextureUnit final : public Object
{
	friend class Pass;
	DEFAULT_CREATE(TextureUnit);
	FRIEND_CONSTRUCT_DESTRUCT(TextureUnit);
	DISALLOW_COPY_ASSIGN(TextureUnit);
	BEGIN_DERIVED_REFECTION_TYPE(TextureUnit, Object)
	END_REFECTION_TYPE;

private:
	TextureUnit(TextureType type, const String& name);
	TextureUnit(const String& name, Texture* tex);
	~TextureUnit();

public:
	Texture*		GetTexture();
	void			SetTexture(const String& shaderName, Texture* tex);
	void			SetTexture(const String& file, TextureType type, TextureFlag flags);
	void			SetTexture(const ShaderTexture& info);
	bool			IsSRGB()const { return _flags & TextureFlag::sRGB; }

private:
	void			LoadTexture();

public:
	String			Name = "_MainTex";
	Vector2			TextureScale = Vector2::one;					//纹理坐标缩放
	Vector2			TextureOffset = Vector2::zero;					//纹理坐标偏移

	//纹理过滤
	TextureFilter	FilterType = TextureFilter::Bilinear;			//纹理过滤方式
	uint			Anisotropy = 1;									//各向异性纹理过滤值，如果FilterType为TextureFilter::Anisotropic有效(d3d11有效范围1-16)

	//纹理寻址
	TextureAddress	AddressMode = TextureAddress::Clamp;			//纹理寻址模式
	Color			BorderColor = Color::Black;						//边框颜色

	bool			EnableReadWrite = false;

private:
	String			_file = "";
	TextureType		_type = TextureType::D2;
	TextureFlag		_flags = TextureFlag::NONE;
	Texture*		_texture = nullptr;
};//TextureUnit
DC_END_NAMESPACE