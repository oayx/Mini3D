#include "Pass.h"
#include "Shader.h"
#include "TextureUnit.h"
#include "runtime/graphics/GraphicsDefine.h"
#include "runtime/graphics/null/CGProgram.h"
#include "runtime/Application.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(Pass, Object);
Pass::Pass(Shader* shader, int pass)
	: _shader(shader)
	, PassIdx(pass)
{
}
Pass::~Pass()
{
	_shader = nullptr;
	SAFE_RELEASE(_program);

	for (auto unit : _textureUnits)
	{
		SAFE_DELETE(unit);
	}
	_textureUnits.Clear();
}
void Pass::Serialize(const PassSerialize& serialize)
{
	SAFE_RELEASE(_program);

	this->LightMode = serialize.LightMode;
	this->Cullmode = serialize.Cullmode;
	this->ColorMask = serialize.ColorMask;
	this->DepthEnable = serialize.DepthEnable;
	this->DepthWriteEnable = serialize.DepthWriteEnable;
	this->DepthCmpFun = serialize.DepthCmpFun;
	this->StencilEnable = serialize.StencilEnable;
	this->StencilFunc = serialize.StencilFunc;
	this->StencilRef = serialize.StencilRef;
	this->StencilReadMask = serialize.StencilReadMask;
	this->StencilWriteMask = serialize.StencilWriteMask;
	this->StencilFailOp = serialize.StencilFailOp;
	this->StencilDepthFailOp = serialize.StencilDepthFailOp;
	this->StencilPassOp = serialize.StencilPassOp;
	this->BlendEnable = serialize.BlendEnable;
	this->SrcBlend = serialize.SrcBlend;
	this->DstBlend = serialize.DstBlend;
	this->SrcAlphaSource = serialize.SrcAlphaSource;
	this->DstAlphaSource = serialize.DstAlphaSource;
	this->AlphaTestEnable = serialize.AlphaTestEnable;
	this->AlphaTestRef = serialize.AlphaTestRef;
	this->AlphaTestCmpFun = serialize.AlphaTestCmpFun;
	this->AlphaToCoverage = serialize.AlphaToCoverage;
	this->ShaderConfig = serialize.ShaderConfig;
	this->ShaderConfig.PassIdx = PassIdx;

	//program
	{
		SAFE_RELEASE(_program);

		ShaderConfig.ShaderDefines = _shader->GetShaderDefines();
		_program = CGProgram::Create(ShaderConfig);
		_program->Retain();
	}

	//texture unit
	{
		for (auto unit : _textureUnits)
		{
			SAFE_DELETE(unit);
		}
		_textureUnits.Clear();

		_textureUnits.Resize(ShaderConfig.SRTextures.Size());
		for(auto obj : ShaderConfig.SRTextures)
		{
			auto reflect_info = obj.second;
			TextureUnit* new_unit = TextureUnit::Create((TextureType)reflect_info.type, reflect_info.name);
			_textureUnits[reflect_info.slot] = new_unit;
		}
	}
}
Material* Pass::GetMaterial()const
{
	return _shader->GetMaterial();
}
TextureUnit* Pass::GetTexUnit(byte layer)
{
	if (layer >= _textureUnits.Size())return nullptr;
	return _textureUnits[layer];
}
bool Pass::SetTexture(Texture* tex)
{
	return SetTexture(0, "_MainTex", tex);
}
bool Pass::SetTexture(const String& shader_name, Texture* tex)
{
	return SetTexture(0, shader_name, tex);
}
bool Pass::SetTexture(byte layer, const String& shader_name, Texture* tex)
{
	TextureUnit* unit = GetTexUnit(layer);
	if (!unit)return false;
	unit->SetTexture(shader_name, tex);
	return true;
}
bool Pass::SetTexture(const ShaderTexture& info)
{
	bool ret = false;
	for (int i = 0; i < _textureUnits.Size(); ++i)
	{
		TextureUnit* unit = GetTexUnit(i);
		if (unit && unit->Name == info.Name)
		{
			unit->SetTexture(info);
			ret = true;
		}
	}
	return ret;
}
bool Pass::SetTextureFile(const String& file, TextureType type, TextureFlag flags)
{
	return SetTextureFile(0, file, type, flags);
}
bool Pass::SetTextureFile(byte layer, const String& file, TextureType type, TextureFlag flags)
{
	TextureUnit* unit = GetTexUnit(layer);
	if (!unit)return false;
	unit->SetTexture(file, type, flags);
	return true;
}
bool Pass::SetTextureFilter(TextureFilter type, uint anisotropy)
{
	return SetTextureFilter(0, type, anisotropy);
}
bool Pass::SetTextureFilter(byte layer, TextureFilter type, uint anisotropy)
{
	TextureUnit* unit = GetTexUnit(layer);
	if (!unit)return false;

	unit->FilterType = type;
	unit->Anisotropy = Math::Min<uint>(anisotropy, (uint)GetGraphicsCaps()->maxAnisoLevel);
	return true;
}
bool Pass::SetTextureAddress(TextureAddress mode, const Vector2& scale, const Color& border)
{
	return SetTextureAddress(0, mode, scale, border);
}
bool Pass::SetTextureAddress(byte layer, TextureAddress mode, const Vector2& scale, const Color& border)
{
	TextureUnit* unit = GetTexUnit(layer);
	if (!unit)return false;

	Vector2 repeat = scale;
	if (mode == TextureAddress::Wrap)
	{
		float max_repeat = (float)GetGraphicsCaps()->maxTextureRepeat;
		repeat.x = Math::Min<float>(repeat.x, max_repeat);
		repeat.y = Math::Min<float>(repeat.y, max_repeat);
	}

	unit->AddressMode = mode;
	unit->BorderColor = border;
	unit->TextureScale = repeat;
	return true;
}
bool Pass::SetAlphaBlend(bool enable, AlphaBlend arg1, AlphaBlend arg2)
{
	return SetAlphaBlend(0, enable, arg1, arg2);
}
bool Pass::SetAlphaBlend(byte layer, bool enable, AlphaBlend arg1, AlphaBlend arg2)
{
	this->BlendEnable = enable;
	this->SrcAlphaSource = arg1;
	this->DstAlphaSource = arg2;
	return true;
}
bool Pass::SetAlphaTest(bool enable, float ref, StencilCmp fun)
{
	return SetAlphaTest(0, enable, ref, fun);
}
bool Pass::SetAlphaTest(byte layer, bool enable, float ref, StencilCmp fun)
{
	this->AlphaTestEnable = enable;
	this->AlphaTestRef = ref;
	this->AlphaTestCmpFun = fun;
	return true;
}
/********************************************************************/
void PassSerialize::Serialize(const String& file, tinyxml2::XMLElement* root_node)
{
	tinyxml2::XMLElement* child_node = nullptr;

	//tags
#if 1
	child_node = root_node->FirstChildElement("Tag");
	if (child_node)
	{
		if (child_node->Attribute("LightMode"))
		{
			LightMode = child_node->Attribute("LightMode");
		}
	}
#endif

	//剔除模式
#if 1
	child_node = root_node->FirstChildElement("CullMode");
	if (child_node)
	{
		if (child_node->Attribute("Type"))
		{
			String value = child_node->Attribute("Type");
			if (value.Equals("off", true))Cullmode = CullMode::Off;
			else if (value.Equals("front", true))Cullmode = CullMode::Front;
			else if (value.Equals("back", true))Cullmode = CullMode::Back;
			else AssertEx(false, "parse error:%s", value.c_str());
		}
	}
#endif

	//mask
#if 1
	child_node = root_node->FirstChildElement("Mask");
	if (child_node)
	{
		if (child_node->Attribute("ColorMask"))
			ColorMask = Vector4(child_node->Attribute("ColorMask"));
	}
#endif

	//深度测试
#if 1
	child_node = root_node->FirstChildElement("Depth");
	if (child_node)
	{
		if (child_node->Attribute("ZTest"))
		{
			DepthEnable = String(child_node->Attribute("ZTest")).ToBool();
		}
		if (child_node->Attribute("ZWrite"))
		{
			DepthWriteEnable = String(child_node->Attribute("ZWrite")).ToBool();
		}
		if (child_node->Attribute("CmpFun"))
		{
			String value = child_node->Attribute("CmpFun");
			if (value.Equals("Never", true))DepthCmpFun = StencilCmp::Never;
			else if (value.Equals("Less", true))DepthCmpFun = StencilCmp::Less;
			else if (value.Equals("Equal", true))DepthCmpFun = StencilCmp::Equal;
			else if (value.Equals("LessEqual", true))DepthCmpFun = StencilCmp::LessEqual;
			else if (value.Equals("Greater", true))DepthCmpFun = StencilCmp::Greater;
			else if (value.Equals("NotEqual", true))DepthCmpFun = StencilCmp::NotEqual;
			else if (value.Equals("GreaterEqual", true))DepthCmpFun = StencilCmp::GreaterEqual;
			else if (value.Equals("Always", true))DepthCmpFun = StencilCmp::Always;
			else AssertEx(false, "parse error:%s", value.c_str());
		}
	}
#endif

	//Stencil
#if 1
	child_node = root_node->FirstChildElement("Stencil");
	if (child_node)
	{
		if (child_node->Attribute("Enable"))
		{
			StencilEnable = Bool::FromString(child_node->Attribute("Enable"));
		}
		if (child_node->Attribute("CmpFun"))
		{
			String value = child_node->Attribute("CmpFun");
			if (value.Equals("Never", true))StencilFunc = StencilCmp::Never;
			else if (value.Equals("Less", true))StencilFunc = StencilCmp::Less;
			else if (value.Equals("Equal", true))StencilFunc = StencilCmp::Equal;
			else if (value.Equals("LessEqual", true))StencilFunc = StencilCmp::LessEqual;
			else if (value.Equals("Greater", true))StencilFunc = StencilCmp::Greater;
			else if (value.Equals("NotEqual", true))StencilFunc = StencilCmp::NotEqual;
			else if (value.Equals("GreaterEqual", true))StencilFunc = StencilCmp::GreaterEqual;
			else if (value.Equals("Always", true))StencilFunc = StencilCmp::Always;
			else AssertEx(0, "parse error:%s", value.c_str());
		}
		if (child_node->Attribute("Ref"))
		{
			StencilRef = String(child_node->Attribute("Ref")).ToByte();
		}
		if (child_node->Attribute("ReadMask"))
		{
			StencilReadMask = String(child_node->Attribute("ReadMask")).ToByte();
		}
		if (child_node->Attribute("WriteMask"))
		{
			StencilWriteMask = String(child_node->Attribute("WriteMask")).ToByte();
		}
		if (child_node->Attribute("FailOp"))
		{
			String value = child_node->Attribute("FailOp");
			if (value.Equals("Keep", true))StencilFailOp = StencilOp::Keep;
			else if (value.Equals("Zero", true))StencilFailOp = StencilOp::Zero;
			else if (value.Equals("Replace", true))StencilFailOp = StencilOp::Replace;
			else if (value.Equals("Incr", true))StencilFailOp = StencilOp::Incr;
			else if (value.Equals("Incr_Wrap", true))StencilFailOp = StencilOp::Incr_Wrap;
			else if (value.Equals("Decr", true))StencilFailOp = StencilOp::Decr;
			else if (value.Equals("Decr_Wrap", true))StencilFailOp = StencilOp::Decr_Wrap;
			else AssertEx(0, "parse error:%s", value.c_str());
		}
		if (child_node->Attribute("DepthFailOp"))
		{
			String value = child_node->Attribute("DepthFailOp");
			if (value.Equals("Keep", true))StencilDepthFailOp = StencilOp::Keep;
			else if (value.Equals("Zero", true))StencilDepthFailOp = StencilOp::Zero;
			else if (value.Equals("Replace", true))StencilDepthFailOp = StencilOp::Replace;
			else if (value.Equals("Incr", true))StencilDepthFailOp = StencilOp::Incr;
			else if (value.Equals("Incr_Wrap", true))StencilDepthFailOp = StencilOp::Incr_Wrap;
			else if (value.Equals("Decr", true))StencilDepthFailOp = StencilOp::Decr;
			else if (value.Equals("Decr_Wrap", true))StencilDepthFailOp = StencilOp::Decr_Wrap;
			else AssertEx(0, "parse error:%s", value.c_str());
		}
		if (child_node->Attribute("PassOp"))
		{
			String value = child_node->Attribute("PassOp");
			if (value.Equals("Keep", true))StencilPassOp = StencilOp::Keep;
			else if (value.Equals("Zero", true))StencilPassOp = StencilOp::Zero;
			else if (value.Equals("Replace", true))StencilPassOp = StencilOp::Replace;
			else if (value.Equals("Incr", true))StencilPassOp = StencilOp::Incr;
			else if (value.Equals("Incr_Wrap", true))StencilPassOp = StencilOp::Incr_Wrap;
			else if (value.Equals("Decr", true))StencilPassOp = StencilOp::Decr;
			else if (value.Equals("Decr_Wrap", true))StencilPassOp = StencilOp::Decr_Wrap;
			else AssertEx(0, "parse error:%s", value.c_str());
		}
	}
#endif

	//Alpha混合
#if 1
	child_node = root_node->FirstChildElement("AlphaBlend");
	if (child_node)
	{
		if (child_node->Attribute("Enable") != 0)
		{
			this->BlendEnable = String(child_node->Attribute("Enable")).ToBool();
		}
		if (child_node->Attribute("SrcBlend") != 0)
		{
			String value = child_node->Attribute("SrcBlend");
			if (value.Equals("Zero", true))this->SrcBlend = AlphaBlend::Zero;
			else if (value.Equals("One", true))this->SrcBlend = AlphaBlend::One;
			else if (value.Equals("SrcColor", true))this->SrcBlend = AlphaBlend::SrcColor;
			else if (value.Equals("OneMinusSrcColor", true))this->SrcBlend = AlphaBlend::OneMinusSrcColor;
			else if (value.Equals("SrcAlpha", true))this->SrcBlend = AlphaBlend::SrcAlpha;
			else if (value.Equals("OneMinusSrcAlpha", true))this->SrcBlend = AlphaBlend::OneMinusSrcAlpha;
			else if (value.Equals("DstAlpha", true))this->SrcBlend = AlphaBlend::DestAlpha;
			else if (value.Equals("InvDstAlpha", true))this->SrcBlend = AlphaBlend::OneMinusDestAlpha;
			else if (value.Equals("DstColor", true))this->SrcBlend = AlphaBlend::DestColor;
			else if (value.Equals("InvDstColor", true))this->SrcBlend = AlphaBlend::OneMinusDestColor;
			else AssertEx(0, "%s", value.c_str());
		}
		if (child_node->Attribute("DstBlend") != 0)
		{
			String value = child_node->Attribute("DstBlend");
			if (value.Equals("Zero", true))this->DstBlend = AlphaBlend::Zero;
			else if (value.Equals("One", true))this->DstBlend = AlphaBlend::One;
			else if (value.Equals("SrcColor", true))this->DstBlend = AlphaBlend::SrcColor;
			else if (value.Equals("OneMinusSrcColor", true))this->DstBlend = AlphaBlend::OneMinusSrcColor;
			else if (value.Equals("SrcAlpha", true))this->DstBlend = AlphaBlend::SrcAlpha;
			else if (value.Equals("OneMinusSrcAlpha", true))this->DstBlend = AlphaBlend::OneMinusSrcAlpha;
			else if (value.Equals("DstAlpha", true))this->DstBlend = AlphaBlend::DestAlpha;
			else if (value.Equals("OneMinusDstAlpha", true))this->DstBlend = AlphaBlend::OneMinusDestAlpha;
			else if (value.Equals("DstColor", true))this->DstBlend = AlphaBlend::DestColor;
			else if (value.Equals("OneMinusDstColor", true))this->DstBlend = AlphaBlend::OneMinusDestColor;
			else AssertEx(0, "%s", value.c_str());
		}
		if (child_node->Attribute("SrcAlphaSource") != 0)
		{
			String value = child_node->Attribute("SrcAlphaSource");
			if (value.Equals("Zero", true))this->SrcAlphaSource = AlphaBlend::Zero;
			else if (value.Equals("One", true))this->SrcAlphaSource = AlphaBlend::One;
			else if (value.Equals("SrcColor", true))this->SrcAlphaSource = AlphaBlend::SrcColor;
			else if (value.Equals("OneMinusSrcColor", true))this->SrcAlphaSource = AlphaBlend::OneMinusSrcColor;
			else if (value.Equals("SrcAlpha", true))this->SrcAlphaSource = AlphaBlend::SrcAlpha;
			else if (value.Equals("OneMinusSrcAlpha", true))this->SrcAlphaSource = AlphaBlend::OneMinusSrcAlpha;
			else if (value.Equals("DstAlpha", true))this->SrcAlphaSource = AlphaBlend::DestAlpha;
			else if (value.Equals("InvDstAlpha", true))this->SrcAlphaSource = AlphaBlend::OneMinusDestAlpha;
			else if (value.Equals("DstColor", true))this->SrcAlphaSource = AlphaBlend::DestColor;
			else if (value.Equals("InvDstColor", true))this->SrcAlphaSource = AlphaBlend::OneMinusDestColor;
			else AssertEx(0, "%s", value.c_str());
		}
		if (child_node->Attribute("DstAlphaSource") != 0)
		{
			String value = child_node->Attribute("DstAlphaSource");
			if (value.Equals("Zero", true))this->DstAlphaSource = AlphaBlend::Zero;
			else if (value.Equals("One", true))this->DstAlphaSource = AlphaBlend::One;
			else if (value.Equals("SrcColor", true))this->DstAlphaSource = AlphaBlend::SrcColor;
			else if (value.Equals("OneMinusSrcColor", true))this->DstAlphaSource = AlphaBlend::OneMinusSrcColor;
			else if (value.Equals("SrcAlpha", true))this->DstAlphaSource = AlphaBlend::SrcAlpha;
			else if (value.Equals("OneMinusSrcAlpha", true))this->DstAlphaSource = AlphaBlend::OneMinusSrcAlpha;
			else if (value.Equals("DstAlpha", true))this->DstAlphaSource = AlphaBlend::DestAlpha;
			else if (value.Equals("OneMinusDstAlpha", true))this->DstAlphaSource = AlphaBlend::OneMinusDestAlpha;
			else if (value.Equals("DstColor", true))this->DstAlphaSource = AlphaBlend::DestColor;
			else if (value.Equals("OneMinusDstColor", true))this->DstAlphaSource = AlphaBlend::OneMinusDestColor;
			else AssertEx(0, "%s", value.c_str());
		}
		if (child_node->Attribute("SrcColorBlend") != 0)
		{
			String value = child_node->Attribute("SrcColorBlend");
			if (value.Equals("Zero", true))this->SrcBlend = AlphaBlend::Zero;
			else if (value.Equals("One", true))this->SrcBlend = AlphaBlend::One;
			else if (value.Equals("SrcColor", true))this->SrcBlend = AlphaBlend::SrcColor;
			else if (value.Equals("OneMinusSrcColor", true))this->SrcBlend = AlphaBlend::OneMinusSrcColor;
			else if (value.Equals("SrcAlpha", true))this->SrcBlend = AlphaBlend::SrcAlpha;
			else if (value.Equals("OneMinusSrcAlpha", true))this->SrcBlend = AlphaBlend::OneMinusSrcAlpha;
			else if (value.Equals("DstAlpha", true))this->SrcBlend = AlphaBlend::DestAlpha;
			else if (value.Equals("InvDstAlpha", true))this->SrcBlend = AlphaBlend::OneMinusDestAlpha;
			else if (value.Equals("DstColor", true))this->SrcBlend = AlphaBlend::DestColor;
			else if (value.Equals("InvDstColor", true))this->SrcBlend = AlphaBlend::OneMinusDestColor;
			else AssertEx(0, "%s", value.c_str());
		}
		if (child_node->Attribute("DstColorBlend") != 0)
		{
			String value = child_node->Attribute("DstColorBlend");
			if (value.Equals("Zero", true))this->DstBlend = AlphaBlend::Zero;
			else if (value.Equals("One", true))this->DstBlend = AlphaBlend::One;
			else if (value.Equals("SrcColor", true))this->DstBlend = AlphaBlend::SrcColor;
			else if (value.Equals("OneMinusSrcColor", true))this->DstBlend = AlphaBlend::OneMinusSrcColor;
			else if (value.Equals("SrcAlpha", true))this->DstBlend = AlphaBlend::SrcAlpha;
			else if (value.Equals("OneMinusSrcAlpha", true))this->DstBlend = AlphaBlend::OneMinusSrcAlpha;
			else if (value.Equals("DstAlpha", true))this->DstBlend = AlphaBlend::DestAlpha;
			else if (value.Equals("OneMinusDstAlpha", true))this->DstBlend = AlphaBlend::OneMinusDestAlpha;
			else if (value.Equals("DstColor", true))this->DstBlend = AlphaBlend::DestColor;
			else if (value.Equals("OneMinusDstColor", true))this->DstBlend = AlphaBlend::OneMinusDestColor;
			else AssertEx(0, "%s", value.c_str());
		}
	}
#endif

	//Alpha测试
#if 1
	child_node = root_node->FirstChildElement("AlphaTest");
	if (child_node)
	{
		if (child_node->Attribute("Enable") != 0)
		{
			this->AlphaTestEnable = String(child_node->Attribute("Enable")).ToBool();
		}
		if (this->AlphaTestEnable)
		{
			if (child_node->Attribute("StencilCmp") != 0)
			{
				String value = child_node->Attribute("StencilCmp");
				if (value.Equals("Never", true))this->AlphaTestCmpFun = StencilCmp::Never;
				else if (value.Equals("Less", true))this->AlphaTestCmpFun = StencilCmp::Less;
				else if (value.Equals("Equal", true))this->AlphaTestCmpFun = StencilCmp::Equal;
				else if (value.Equals("LessEqual", true))this->AlphaTestCmpFun = StencilCmp::LessEqual;
				else if (value.Equals("Greater", true))this->AlphaTestCmpFun = StencilCmp::Greater;
				else if (value.Equals("NotEqual", true))this->AlphaTestCmpFun = StencilCmp::NotEqual;
				else if (value.Equals("GreaterEqual", true))this->AlphaTestCmpFun = StencilCmp::GreaterEqual;
				else if (value.Equals("Always", true))this->AlphaTestCmpFun = StencilCmp::Always;
				else AssertEx(0, "%s", value.c_str());
			}
			if (child_node->Attribute("Ref") != 0)
			{
				this->AlphaTestRef = String(child_node->Attribute("Ref")).ToFloat();
			}
		}
	}
#endif

#if 1
	child_node = root_node->FirstChildElement("CG");
	if (child_node)
	{
		String shader_file = "";
		if (child_node->Attribute("File"))
		{
			shader_file = child_node->Attribute("File");
			shader_file = Path::Combine(Path::GetDirectoryName(file), shader_file);
		}

		tinyxml2::XMLElement* shader_node = child_node->FirstChildElement("VS");
		if (shader_node)
		{
			ShaderConfig.ShaderFile[int(ShaderType::Vertex)] = shader_file;
			if (shader_node->Attribute("Enter"))ShaderConfig.Enter[int(ShaderType::Vertex)] = shader_node->Attribute("Enter");
			if (shader_node->Attribute("Target"))ShaderConfig.Target[int(ShaderType::Vertex)] = shader_node->Attribute("Target");
		}

		shader_node = child_node->FirstChildElement("HS");
		if (shader_node)
		{
			ShaderConfig.ShaderFile[int(ShaderType::Hull)] = shader_file;
			if (shader_node->Attribute("Enter"))ShaderConfig.Enter[int(ShaderType::Hull)] = shader_node->Attribute("Enter");
			if (shader_node->Attribute("Target"))ShaderConfig.Target[int(ShaderType::Hull)] = shader_node->Attribute("Target");
		}

		shader_node = child_node->FirstChildElement("DS");
		if (shader_node)
		{
			ShaderConfig.ShaderFile[int(ShaderType::Domain)] = shader_file;
			if (shader_node->Attribute("Enter"))ShaderConfig.Enter[int(ShaderType::Domain)] = shader_node->Attribute("Enter");
			if (shader_node->Attribute("Target"))ShaderConfig.Target[int(ShaderType::Domain)] = shader_node->Attribute("Target");
		}

		shader_node = child_node->FirstChildElement("GS");
		if (shader_node)
		{
			ShaderConfig.ShaderFile[int(ShaderType::Geometry)] = shader_file;
			if (shader_node->Attribute("Enter"))ShaderConfig.Enter[int(ShaderType::Geometry)] = shader_node->Attribute("Enter");
			if (shader_node->Attribute("Target"))ShaderConfig.Target[int(ShaderType::Geometry)] = shader_node->Attribute("Target");
		}

		shader_node = child_node->FirstChildElement("PS");
		if (shader_node)
		{
			ShaderConfig.ShaderFile[int(ShaderType::Pixel)] = shader_file;
			if (shader_node->Attribute("Enter"))ShaderConfig.Enter[int(ShaderType::Pixel)] = shader_node->Attribute("Enter");
			if (shader_node->Attribute("Target"))ShaderConfig.Target[int(ShaderType::Pixel)] = shader_node->Attribute("Target");
		}

		shader_node = child_node->FirstChildElement("CS");
		if (shader_node)
		{
			ShaderConfig.ShaderFile[int(ShaderType::Compute)] = shader_file;
			if (shader_node->Attribute("Enter"))ShaderConfig.Enter[int(ShaderType::Compute)] = shader_node->Attribute("Enter");
			if (shader_node->Attribute("Target"))ShaderConfig.Target[int(ShaderType::Compute)] = shader_node->Attribute("Target");
		}

		tinyxml2::XMLElement* reflectNode = child_node->FirstChildElement("Reflect");
		if (reflectNode)
		{
			tinyxml2::XMLElement* node = reflectNode->FirstChildElement("Semantics");
			if (node)
			{
				if (node->Attribute("Value"))ShaderConfig.SRVertexSemantic = (VertexSemantic)String(node->Attribute("Value")).ToUInt();
			}

			tinyxml2::XMLElement* cbuffer_node = reflectNode->FirstChildElement("CBuffers");
			if (cbuffer_node)
			{
				node = cbuffer_node->FirstChildElement("CBuffer");
				while (node)
				{
					sShaderReflectCBuffer cbuffer;
					if (node->Attribute("Name"))cbuffer.name = node->Attribute("Name");
					if (node->Attribute("Slot"))cbuffer.slot = String(node->Attribute("Slot")).ToByte();
					if (node->Attribute("Size"))cbuffer.size = String(node->Attribute("Size")).ToUInt();
					if (node->Attribute("Stage"))cbuffer.stage = String(node->Attribute("Stage")).ToUInt();
					AssertEx(ShaderConfig.SRBuffers.Add(cbuffer.name ,cbuffer),"");

					{//变量
						tinyxml2::XMLElement* variables_node = node->FirstChildElement("Variables");
						if (variables_node)
						{
							tinyxml2::XMLElement* variable_node = variables_node->FirstChildElement("Variable");
							while (variable_node)
							{
								sShaderReflectVariable variable;
								if (variable_node->Attribute("Name"))variable.var_name = variable_node->Attribute("Name");
								if (variable_node->Attribute("Offset"))variable.offset = String(variable_node->Attribute("Offset")).ToUInt();
								if (variable_node->Attribute("Size"))variable.size = String(variable_node->Attribute("Size")).ToUInt();
								if (variable_node->Attribute("Stage"))variable.stage = (ShaderType)String(variable_node->Attribute("Stage")).ToUInt();
								variable.cb_name = cbuffer.name;
								variable.var_fullname = cbuffer.name + "." + variable.var_name;
								AssertEx(ShaderConfig.SRVariables.Add(variable.var_name ,variable),"");

								variable_node = variable_node->NextSiblingElement("Variable");
							}
						}
					}

					node = node->NextSiblingElement("CBuffer");
				}
			}

			tinyxml2::XMLElement* texture_node = reflectNode->FirstChildElement("Textures");
			if (texture_node)
			{
				node = texture_node->FirstChildElement("Texture");
				while (node)
				{
					sShaderReflectTexture texture;
					if (node->Attribute("Name"))texture.name = node->Attribute("Name");
					if (node->Attribute("Type"))texture.type = TextureType(String(node->Attribute("Type")).ToUInt());
					if (node->Attribute("Slot"))texture.slot = String(node->Attribute("Slot")).ToByte();
					if (node->Attribute("Stage"))texture.stage = String(node->Attribute("Stage")).ToUInt();
					AssertEx(ShaderConfig.SRTextures.Add(texture.name ,texture),"");

					node = node->NextSiblingElement("Texture");
				}
			}
		}
	}
#endif
}
DC_END_NAMESPACE