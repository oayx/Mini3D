 
/*****************************************************************************
* Author： hannibal
* Date：2009/11/30
* Description： 渲染通道
*****************************************************************************/
#pragma once

#include "RenderDefine.h"
#include "TextureUnit.h"
#include "runtime/scene/SceneUtils.h"
#include "external/tinyxml2/tinyxml2.h"

DC_BEGIN_NAMESPACE
class CGProgram;
class Shader;
class Material;
class PassSerialize;
/********************************************************************/
class ENGINE_DLL Pass Final : public Object
{
	friend class Shader;
	typedef Vector<TextureUnit*> VecTextureUnit;
	DEFAULT_CREATE(Pass);
	FRIEND_CONSTRUCT_DESTRUCT(Pass);
	DISALLOW_COPY_ASSIGN(Pass);
	BEGIN_DERIVED_REFECTION_TYPE(Pass, Object)
	END_DERIVED_REFECTION_TYPE;

private:
	Pass(Shader* shader, int pass);
	~Pass();

	void			Serialize(const PassSerialize& serialize);

public:
	Shader*			GetShader()const { return _shader; }
	CGProgram*		GetProgram()const { return _program; }
	Material*		GetMaterial()const;
	TextureUnit*	GetTexUnit(byte layer);
	byte			GetTexUnitCount()const{ return (byte)_textureUnits.Size(); }

	bool			SetTexture(Texture* tex);
	bool			SetTexture(const String& shader_name, Texture* tex);
	bool			SetTexture(byte layer, const String& shader_name, Texture* tex);
	bool			SetTexture(const ShaderTexture& info);//pass里面只要名称相同就设置
	bool			SetTextureFile(const String& file, TextureType type = TextureType::D2, TextureFlag flags = TextureFlag::sRGB);
	bool			SetTextureFile(byte layer, const String& file, TextureType type = TextureType::D2, TextureFlag flags = TextureFlag::sRGB);
	bool			SetTextureFilter(TextureFilter type, uint anisotropy = 1);
	bool			SetTextureFilter(byte layer, TextureFilter type, uint anisotropy = 1);
	bool			SetTextureAddress(TextureAddress mode, const Vector2& scale = Vector2::one, const Color& border = Color::Black);
	bool			SetTextureAddress(byte layer, TextureAddress mode, const Vector2& scale = Vector2::one, const Color& border = Color::Black);
	bool			SetAlphaBlend(bool enable, AlphaBlend arg1 = AlphaBlend::SrcAlpha, AlphaBlend arg2 = AlphaBlend::OneMinusSrcAlpha);
	bool			SetAlphaBlend(byte layer, bool enable, AlphaBlend arg1 = AlphaBlend::SrcAlpha, AlphaBlend arg2 = AlphaBlend::OneMinusSrcAlpha);
	bool			SetAlphaTest(bool enable, float ref, StencilCmp fun = StencilCmp::Greater);
	bool			SetAlphaTest(byte layer, bool enable, float ref, StencilCmp fun = StencilCmp::Greater);

public:
	int				PassIdx = 0;

	String			LightMode = "ForwardBase";

	CullMode		Cullmode = CullMode::Back;
	Vector4			ColorMask = Vector4::one;

	//深度缓冲与深度测试(如果不开启深度测试，则深度不能更新到深度缓冲区)
	bool			DepthEnable = true;
	bool			DepthWriteEnable = true;
	StencilCmp		DepthCmpFun = StencilCmp::LessEqual;

	//模板缓冲区，执行方式
	//if(StencilFunc((Value & StencilReadMask),  (StencilRef & StencilReadMask)))
	//	accept pixel
	//else
	//	reject pixel
	bool			StencilEnable = false;
	StencilCmp		StencilFunc = StencilCmp::Always;
	byte			StencilRef = 1;
	byte			StencilReadMask = 0xff;
	byte			StencilWriteMask = 0xff;						//模板缓冲区写屏蔽，
	StencilOp		StencilFailOp = StencilOp::Keep;				//模板测试失败将采取的动作
	StencilOp		StencilDepthFailOp = StencilOp::Keep;			//模板测试通过，但是深度测试失败时采取的动作
	StencilOp		StencilPassOp = StencilOp::Keep;				//深度测试和模板测试都通过，将采取的动作	

	//alpha混合
	bool			BlendEnable = false;							//Alpha混合
	AlphaBlend		SrcBlend = AlphaBlend::SrcAlpha;				//颜色混合方式:SrcColor*SrcAlpha+DstColor*DstAlpha
	AlphaBlend		DstBlend = AlphaBlend::OneMinusSrcAlpha;
	AlphaBlend		SrcAlphaSource = AlphaBlend::One;				//SrcAlpha来源
	AlphaBlend		DstAlphaSource = AlphaBlend::Zero;				//DstAlpha来源

	//alpha测试
	bool			AlphaTestEnable = false;						//Alpha测试
	float			AlphaTestRef = 0.5f;							//测试比较值(范围是0-1，传递给渲染管线会转换成0-255);
	StencilCmp		AlphaTestCmpFun = StencilCmp::Greater;			//Alpha测试比较函数

	bool			AlphaToCoverage = false;

	//shader
	ShaderDesc		ShaderConfig;

private:
	Shader*			_shader = nullptr;
	CGProgram*		_program = nullptr;
	VecTextureUnit	_textureUnits;
};//Pass

class PassSerialize
{
public:
	void			Serialize(const String& file, tinyxml2::XMLElement* root_node);

public:
	String			LightMode = "ForwardBase";

	CullMode		Cullmode = CullMode::Back;				//剔除模式
	Vector4			ColorMask = Vector4::one;

	//深度缓冲与深度测试
	bool			DepthEnable = true;						//是否启用深度测试
	bool			DepthWriteEnable = true;				//是否更新深度缓冲区
	StencilCmp		DepthCmpFun = StencilCmp::LessEqual;	//深度比较函数

	//模板缓冲区
	bool			StencilEnable = false;					//是否开启模板测试
	StencilCmp		StencilFunc = StencilCmp::Always;		//模板测试比较函数
	byte			StencilRef = 1;
	byte			StencilReadMask = 0xff;
	byte			StencilWriteMask = 0xff;
	StencilOp		StencilFailOp = StencilOp::Keep;		//模板测试失败
	StencilOp		StencilDepthFailOp = StencilOp::Keep;	//模板测试成功，深度测试失败
	StencilOp		StencilPassOp = StencilOp::Keep;		//模板测试成功，深度测试成功


	//alpha混合
	bool			BlendEnable = false;					//Alpha混合
	AlphaBlend		SrcBlend = AlphaBlend::SrcAlpha;		//颜色混合方式:SrcColor*SrcAlpha+DstColor*DstAlpha
	AlphaBlend		DstBlend = AlphaBlend::OneMinusSrcAlpha;
	AlphaBlend		SrcAlphaSource = AlphaBlend::One;		//SrcAlpha来源
	AlphaBlend		DstAlphaSource = AlphaBlend::Zero;		//DstAlpha来源

	//alpha测试
	bool			AlphaTestEnable = false;				//Alpha测试
	float			AlphaTestRef = 0.5f;					//测试比较值(范围是0-1，传递给渲染管线会转换成0-255);
	StencilCmp		AlphaTestCmpFun = StencilCmp::Greater;	//Alpha测试比较函数

	bool			AlphaToCoverage = false;				//在开启Multi-sample多重采样时，在透明和不透明之间达到抗锯齿的效果

	//shader
	ShaderDesc		ShaderConfig;							//shader程序
};
DC_END_NAMESPACE