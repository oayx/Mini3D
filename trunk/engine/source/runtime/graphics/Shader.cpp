#include "Shader.h"
#include "Pass.h"
#include "core/stream/DataStream.h"
#include "runtime/graphics/ShaderlabUtils.h"
#include "external/shaderlab/ShaderParser.h"

using namespace shaderlab;
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(Shader, Resource); 
Shader::Shader(Material* material)
	: base(ResourceType::Shader), _material(material)
{

}
Shader::~Shader()
{
	for (auto pass : _passes)
	{
		SAFE_DELETE(pass);
	}
	_passes.Clear();
	_material = nullptr;
}
Shader* Shader::Create(Material* material)
{
	Shader* shader = Memory::New<Shader>(material);
	shader->AutoRelease();
	return shader;
}
Pass* Shader::CreatePass(int pass)
{
	CHECK_RETURN_PTR_NULL(pass >= 0);
	if (pass < _passes.Size())
		return _passes[pass];

	Pass* new_pass = Pass::Create(this, pass);
	_passes.Add(new_pass);
	return new_pass;
}
Pass* Shader::GetPass(int pass)const
{
	if (pass < 0 || pass >= _passes.Size())
		return nullptr;
	return _passes[pass];
}
void Shader::SetTexture(const ShaderTexture& info)
{
	for (int i = 0; i < _passes.Size(); ++i)
	{
		_passes[i]->SetTexture(info);
	}
}
bool Shader::ParseFromFile(const String& file)
{
	ShaderSerialize* serialize = ShaderSerializePools::Get(file);
	if (serialize == nullptr)return false;

	this->_groupInspector = serialize->GroupInspector;
	this->_castShadow = serialize->CastShadow;
	this->_receiveShadow = serialize->ReceiveShadow;
	this->_renderType = serialize->RenderType;
	this->_nRenderQueue = serialize->RenderQueue;
	this->_enableLight = serialize->EnableLight;
	this->_scissorEnable = serialize->ScissorEnable;
	this->_shaderVariables = serialize->ShaderVariables;
	this->_shaderTextures = serialize->ShaderTextures;

	//多Pass
	for (int i = 0; i < serialize->mPassSerializes.Size(); ++i)
	{
		Pass* pass = CreatePass(i);
		pass->SerializeFromInfo(serialize->mPassSerializes[i]);
	}

	//初始化贴图文件
	for (int i = 0; i < _shaderTextures.Size(); ++i)
	{
		this->SetTexture(_shaderTextures[i]);
	}

	return true;
}
bool Shader::LoadFromFile(const String& file)
{
	return this->ParseFromFile(file);
}
/********************************************************************/
void ShaderSerialize::Serialize(const String& file)
{
	Debuger::Log("ShaderSerialize::Serialize - %s", file.c_str());
	String filePath = Resource::GetFullDataPath(file);
	{//.shader
		std::shared_ptr<ASTNode> root = ShaderlabUtils::ParseShader(filePath);
		if (root == nullptr)
			return;

		const String& shaderName = std::dynamic_pointer_cast<ASTShaderNode>(root)->Name;
		if (shaderName.StartsWith("Hidden/", true))
			this->HideInspector = true;
		this->GroupInspector = shaderName;

		for (const auto& node : root->children)
		{
			switch (node->Type)
			{
			case ASTNodeType::Properties:
			{
				for (auto childNode : node->children)
				{
					const auto& propertyNode = std::dynamic_pointer_cast<ASTPropertyNode>(childNode);
					MyAssert(propertyNode != nullptr);
					if (propertyNode->PropType == PropertyType::Rect || propertyNode->PropType == PropertyType::Texture2D ||
						propertyNode->PropType == PropertyType::Texture2DArray || propertyNode->PropType == PropertyType::Texture3D ||
						propertyNode->PropType == PropertyType::TextureCube || propertyNode->PropType == PropertyType::TextureCubeArray)
					{
						ShaderTexture texture_info;
						switch (propertyNode->PropType)
						{
						case PropertyType::Rect: texture_info.Type = TextureType::D2; break;
						case PropertyType::Texture2D: texture_info.Type = TextureType::D2; break;
						case PropertyType::Texture2DArray: texture_info.Type = TextureType::D2Array; break;
						case PropertyType::Texture3D: texture_info.Type = TextureType::D3; break;
						case PropertyType::TextureCube: texture_info.Type = TextureType::Cube; break;
						default: MyAssert(false); break;
						}
						texture_info.Name = propertyNode->Name;
						String texFile = propertyNode->DefaultValue;
						texture_info.File = texFile.Replace("{}", "");
						ShaderTextures.Add(texture_info);
					}
					else
					{
						ShaderVariable var_info;
						var_info.Name = propertyNode->Name;
						var_info.Alias = propertyNode->DisplayName;
						var_info.Value = propertyNode->DefaultValue;
						switch (propertyNode->PropType)
						{
						case PropertyType::Int: var_info.Type = ShaderVariableType::Int; break;
						case PropertyType::Float: var_info.Type = ShaderVariableType::Float; break;
						case PropertyType::Range:
						{
							var_info.Type = ShaderVariableType::Slider;
							var_info.Range = Vector2(propertyNode->Range.min, propertyNode->Range.max);
							break;
						}
						case PropertyType::Vector:
						{
							var_info.Type = ShaderVariableType::Vector; 
							var_info.Value = var_info.Value.Substring(1, var_info.Value.Size() - 2);
							break;
						}
						case PropertyType::Color:
						{
							var_info.Type = ShaderVariableType::Color;
							var_info.Value = var_info.Value.Substring(1, var_info.Value.Size() - 2);
							break;
						}
						default: MyAssert(false); break;
						}

						if (!propertyNode->Header.empty())
						{
							if (String(propertyNode->Header).Contains("HideInInspector"))
								this->HideInspector = true;
						}

						ShaderVariables.Add(var_info);
					}

				}
				break;
			}
			case ASTNodeType::SubShader:
			{
				mPassSerializes.Clear();
				for (const auto& childNode : node->children)
				{
					switch (childNode->Type)
					{
					case ASTNodeType::Tags:
					{
						const auto& tagNode = std::dynamic_pointer_cast<ASTTagsNode>(childNode);
						for (const auto& tag : tagNode->tags)
						{
							String key = tag.first;
							if (key.Equals("RenderType", true))
								this->RenderType = tag.second;
							else if (key.Equals("Queue", true))
								this->RenderQueue = StringUtils::ParseShaderlabQueue(tag.second);
							else if (key.Equals("EnableLight", true))
								this->EnableLight = String(tag.second).ToBool();
							else if (key.Equals("CastShadow", true))
								this->CastShadow = String(tag.second).ToBool();
							else if (key.Equals("ReceiveShadow", true))
								this->ReceiveShadow = String(tag.second).ToBool();
							else
								Debuger::Error("Shader::Serialize -  unknow type: (%s)", tag.first.c_str());
						}
						break;
					}
					case ASTNodeType::Pass:
					{
						mPassSerializes.Add(PassSerialize());
						mPassSerializes.Last().Serialize(file, std::dynamic_pointer_cast<ASTPassNode>(childNode));
						break;
					}
					default:
						break;
					}
				}
				break;
			}
			case ASTNodeType::FallBack:
			{
				//TODO
				break;
			}
			default:
			{
				AssertEx(false, "parse error, unknow type:%d", (int)node->Type);
				break;
			}
			}
		}
	}
	{//.reflect
		filePath = filePath.Replace(".shader", ".cache");
		tinyxml2::XMLDocument doc;
		FileDataStream stream(filePath, "rb+");
		tinyxml2::XMLError error = doc.LoadFile(stream.GetHandle());
		if (error != tinyxml2::XML_SUCCESS)
		{
			Debuger::Error("Shader::Serialize -  open XML error (%s)", filePath.c_str());
			return;
		}
		else
		{
			tinyxml2::XMLElement* rootNode = doc.RootElement();
			tinyxml2::XMLElement* node = nullptr;

			//Pass
			int index = 0;
			node = rootNode->FirstChildElement("Pass");
			while (node)
			{
				mPassSerializes[index++].Serialize(file, node);
				node = node->NextSiblingElement("Pass");
			}
		}
	}
}
/********************************************************************/
void ShaderSerializePools::Destroy()
{
	for (auto obj : _shaderSerializes)
	{
		SAFE_DELETE(obj.second);
	}
	_shaderSerializes.Clear();
}
ShaderSerialize* ShaderSerializePools::Get(const String& file)
{
	auto it = _shaderSerializes.Find(file);
	if (it != _shaderSerializes.end())
	{
		return it->second;
	}
	else
	{
		Serialize(file);
		it = _shaderSerializes.Find(file);
		if (it != _shaderSerializes.end())
		{
			return it->second;
		}
	}
	return nullptr;
}
void ShaderSerializePools::Serialize(const String& file)
{
	if (_shaderSerializes.Find(file) != _shaderSerializes.end())
	{
		Debuger::Warning("Have already same file:%s", file.c_str());
		return;
	}
	ShaderSerialize* Shader = ShaderSerialize::Create();
	Shader->Serialize(file);
	_shaderSerializes.Add(file, Shader);
}
DC_END_NAMESPACE