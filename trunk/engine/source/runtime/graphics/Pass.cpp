#include "Pass.h"
#include "Shader.h"
#include "TextureUnit.h"
#include "runtime/graphics/ShaderlabUtils.h"
#include "runtime/graphics/GraphicsDefine.h"
#include "runtime/graphics/null/CGProgram.h"
#include "runtime/Application.h"

using namespace shaderlab;

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(Pass, Object);
Pass::Pass(Shader* shader, int pass)
	: PassIdx(pass)
	, _shader(shader)
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
void Pass::SerializeFromInfo(const PassSerialize& serialize)
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
bool Pass::SetTexture(const String& shaderName, Texture* tex)
{
	return SetTexture(0, shaderName, tex);
}
bool Pass::SetTexture(byte layer, const String& shaderName, Texture* tex)
{
	TextureUnit* unit = GetTexUnit(layer);
	if (!unit)return false;
	unit->SetTexture(shaderName, tex);
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
void PassSerialize::Serialize(const String& file, tinyxml2::XMLElement* rootNode)
{
	Debuger::Log("PassSerialize::Serialize xml - %s", file.c_str());
	tinyxml2::XMLElement* childNode = nullptr;
	childNode = rootNode->FirstChildElement("CG");
	if (childNode)
	{
		tinyxml2::XMLElement* reflectNode = childNode->FirstChildElement("Reflect");
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
}
void PassSerialize::Serialize(const String& file, const std::shared_ptr<ASTPassNode>& passNode)
{
	Debuger::Log("PassSerialize::Serialize shaderlab - %s", file.c_str());
	for (const auto& childNode : passNode->children)
	{
		//tags
		switch (childNode->Type)
		{
		case ASTNodeType::Tags:
		{
			const auto& node = std::dynamic_pointer_cast<ASTTagsNode>(childNode);
			for (const auto& tag : node->tags)
			{
				if (String(tag.first).Equals("LightMode", true))
					this->LightMode = tag.second;
				else
					AssertEx(false, "parse error:%s", tag.first.c_str());
			}
			break;
		}
		case ASTNodeType::Cull://剔除模式
		{
			const auto& node = std::dynamic_pointer_cast<ASTCullNode>(childNode);
			String value = node->Value;
			if (value.Equals("off", true))Cullmode = CullMode::Off;
			else if (value.Equals("front", true))Cullmode = CullMode::Front;
			else if (value.Equals("back", true))Cullmode = CullMode::Back;
			else AssertEx(false, "parse error:%s", value.c_str());
			break;
		}
		case ASTNodeType::ColorMask://color mask
		{
			ColorMask = Vector4::zero;
			const auto& node = std::dynamic_pointer_cast<ASTColorMaskNode>(childNode);
			String value = node->Value;
			if (value.IndexOf("R") >= 0)
				ColorMask[0] = 1;
			if (value.IndexOf("G") >= 0)
				ColorMask[1] = 1;
			if (value.IndexOf("B") >= 0)
				ColorMask[2] = 1;
			if (value.IndexOf("A") >= 0)
				ColorMask[3] = 1;
			break;
		}
		case ASTNodeType::ZTest://深度测试
		{
			const auto& node = std::dynamic_pointer_cast<ASTZTestNode>(childNode);
			String value = node->Value;
			if (value.Equals("Off", true) || value.Equals("Disabled", true))
			{
				DepthEnable = false;
			}
			else
			{
				DepthEnable = true;
				if (value.Equals("Never", true))DepthCmpFun = StencilCmp::Never;
				else if (value.Equals("Less", true))DepthCmpFun = StencilCmp::Less;
				else if (value.Equals("Equal", true))DepthCmpFun = StencilCmp::Equal;
				else if (value.Equals("LEqual", true))DepthCmpFun = StencilCmp::LEqual;
				else if (value.Equals("Greater", true))DepthCmpFun = StencilCmp::Greater;
				else if (value.Equals("NotEqual", true))DepthCmpFun = StencilCmp::NotEqual;
				else if (value.Equals("GEqual", true))DepthCmpFun = StencilCmp::GEqual;
				else if (value.Equals("Always", true))DepthCmpFun = StencilCmp::Always;
				else AssertEx(false, "parse error:%s", value.c_str());
			}
			break;
		}
		case ASTNodeType::ZWrite://深度写入
		{
			const auto& node = std::dynamic_pointer_cast<ASTZWriteNode>(childNode);
			String value = node->Value;
			if (value.Equals("On", true))
				DepthWriteEnable = true;
			else if (value.Equals("Off", true))
				DepthWriteEnable = false;
			else
				AssertEx(false, "parse error:%s", value.c_str());
			break;
		}
		case ASTNodeType::Stencil://Stencil
		{
			StencilEnable = true;
			const auto& node = std::dynamic_pointer_cast<ASTStencilNode>(childNode);
			for (const auto& obj : node->Values)
			{
				String key = obj.first;
				String value = obj.second;
				if (key.Equals("Ref", true))
				{
					StencilRef = value.ToByte();
				}
				else if (key.Equals("Comp", true))
				{
					if (value.Equals("Never", true))StencilFunc = StencilCmp::Never;
					else if (value.Equals("Less", true))StencilFunc = StencilCmp::Less;
					else if (value.Equals("Equal", true))StencilFunc = StencilCmp::Equal;
					else if (value.Equals("LEqual", true))StencilFunc = StencilCmp::LEqual;
					else if (value.Equals("Greater", true))StencilFunc = StencilCmp::Greater;
					else if (value.Equals("NotEqual", true))StencilFunc = StencilCmp::NotEqual;
					else if (value.Equals("GEqual", true))StencilFunc = StencilCmp::GEqual;
					else if (value.Equals("Always", true))StencilFunc = StencilCmp::Always;
					else AssertEx(0, "parse error:%s", value.c_str());
				}
				else if (key.Equals("ReadMask", true))
				{
					StencilReadMask = value.ToByte();
				}
				else if (key.Equals("WriteMask", true))
				{
					StencilWriteMask = value.ToByte();
				}
				else if (key.Equals("Pass", true))
				{
					if (value.Equals("Keep", true))StencilPassOp = StencilOp::Keep;
					else if (value.Equals("Zero", true))StencilPassOp = StencilOp::Zero;
					else if (value.Equals("Replace", true))StencilPassOp = StencilOp::Replace;
					else if (value.Equals("Incr", true))StencilPassOp = StencilOp::Incr;
					else if (value.Equals("Incr_Wrap", true))StencilPassOp = StencilOp::Incr_Wrap;
					else if (value.Equals("Decr", true))StencilPassOp = StencilOp::Decr;
					else if (value.Equals("Decr_Wrap", true))StencilPassOp = StencilOp::Decr_Wrap;
					else AssertEx(0, "parse error:%s", value.c_str());
				}
				else if (key.Equals("Fail", true))
				{
					if (value.Equals("Keep", true))StencilFailOp = StencilOp::Keep;
					else if (value.Equals("Zero", true))StencilFailOp = StencilOp::Zero;
					else if (value.Equals("Replace", true))StencilFailOp = StencilOp::Replace;
					else if (value.Equals("Incr", true))StencilFailOp = StencilOp::Incr;
					else if (value.Equals("Incr_Wrap", true))StencilFailOp = StencilOp::Incr_Wrap;
					else if (value.Equals("Decr", true))StencilFailOp = StencilOp::Decr;
					else if (value.Equals("Decr_Wrap", true))StencilFailOp = StencilOp::Decr_Wrap;
					else AssertEx(0, "parse error:%s", value.c_str());
				}
				else if (key.Equals("ZFail", true))
				{
					if (value.Equals("Keep", true))StencilDepthFailOp = StencilOp::Keep;
					else if (value.Equals("Zero", true))StencilDepthFailOp = StencilOp::Zero;
					else if (value.Equals("Replace", true))StencilDepthFailOp = StencilOp::Replace;
					else if (value.Equals("Incr", true))StencilDepthFailOp = StencilOp::Incr;
					else if (value.Equals("Incr_Wrap", true))StencilDepthFailOp = StencilOp::Incr_Wrap;
					else if (value.Equals("Decr", true))StencilDepthFailOp = StencilOp::Decr;
					else if (value.Equals("Decr_Wrap", true))StencilDepthFailOp = StencilOp::Decr_Wrap;
					else AssertEx(0, "parse error:%s", value.c_str());
				}
				else
					AssertEx(false, "parse error:%s", key.c_str());
			}
			break;
		}
		case ASTNodeType::Blend://深度写入
		{
			const auto& node = std::dynamic_pointer_cast<ASTBlendNode>(childNode);
			this->BlendEnable = !node->blendOff;
			if (this->BlendEnable)
			{
				const ASTBlendNode::BlendFactors& factors = node->factors;
				if (!factors.src.empty())
				{
					String value = factors.src;
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
				if (!factors.dst.empty())
				{
					String value = factors.dst;
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
				if (!factors.srcAlpha.empty())
				{
					String value = factors.srcAlpha;
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
				if (!factors.dstAlpha.empty())
				{
					String value = factors.dstAlpha;
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
			}
			break;
		}
		case ASTNodeType::BlendOp://混合操作
		{
			const auto& node = std::dynamic_pointer_cast<ASTBlendOpNode>(childNode);
			String value = node->Value;
			if (value.Equals("Add", true))
				BlendOp = AlphaBlendOp::Add;
			else if (value.Equals("Sub", true))
				BlendOp = AlphaBlendOp::Sub;
			else if (value.Equals("RevSub", true))
				BlendOp = AlphaBlendOp::RevSub;
			else if (value.Equals("Min", true))
				BlendOp = AlphaBlendOp::Min;
			else if (value.Equals("Max", true))
				BlendOp = AlphaBlendOp::Max;
			else
				AssertEx(false, "parse error:%s", value.c_str());
			break;
		}
		case ASTNodeType::ShaderBlock:
		{
			const auto& node = std::dynamic_pointer_cast<ASTShaderBlockNode>(childNode);
			CGProgramData data = ShaderlabUtils::ParseCG(node->ShaderCode);
			if (data.Includes.size() > 0)
			{
				String shaderFile = data.Includes[0];
				shaderFile = Path::Combine(Path::GetDirectoryName(file), shaderFile);
				for (auto& define : data.Defines)
				{
					ShaderConfig.ShaderDefines.Add(define);
				}
				for (auto& enter : data.Pragmas)
				{
					String type = enter.first;
					if (type.Equals("vertex", true))
					{
						ShaderConfig.ShaderFile[int(ShaderType::Vertex)] = shaderFile;
						ShaderConfig.Enter[int(ShaderType::Vertex)] = enter.second;
					}
					else if (type.Equals("fragment", true))
					{
						ShaderConfig.ShaderFile[int(ShaderType::Pixel)] = shaderFile;
						ShaderConfig.Enter[int(ShaderType::Pixel)] = enter.second;
					}
					else if (type.Equals("geometry", true))
					{
						ShaderConfig.ShaderFile[int(ShaderType::Geometry)] = shaderFile;
						ShaderConfig.Enter[int(ShaderType::Geometry)] = enter.second;
					}
					else if (type.Equals("hull", true))
					{
						ShaderConfig.ShaderFile[int(ShaderType::Hull)] = shaderFile;
						ShaderConfig.Enter[int(ShaderType::Hull)] = enter.second;
					}
					else if (type.Equals("domain", true))
					{
						ShaderConfig.ShaderFile[int(ShaderType::Domain)] = shaderFile;
						ShaderConfig.Enter[int(ShaderType::Domain)] = enter.second;
					}
					else if (type.Equals("compute", true))
					{
						ShaderConfig.ShaderFile[int(ShaderType::Compute)] = shaderFile;
						ShaderConfig.Enter[int(ShaderType::Compute)] = enter.second;
					}
				}
			}
			break;
		}
		default:
		{
			AssertEx(false, "parse error, unknow type:%d", (int)childNode->Type);
			break;
		}
		}
	}
}
DC_END_NAMESPACE