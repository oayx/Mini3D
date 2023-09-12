#include "Shader.h"
#include "Pass.h"
#include "core/stream/DataStream.h"
 
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
	Shader* shader = DBG_NEW Shader(material);
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
	this->m_nRenderQueue = serialize->RenderQueue;
	this->_enableLight = serialize->EnableLight;
	this->_scissorEnable = serialize->ScissorEnable;
	this->_shaderVariables = serialize->ShaderVariables;
	this->_shaderTextures = serialize->ShaderTextures;
	this->_shaderDefines = serialize->ShaderDefines;

	//多Pass
	for (int i = 0; i < serialize->mPassSerializes.Size(); ++i)
	{
		Pass* pass = CreatePass(i);
		pass->Serialize(serialize->mPassSerializes[i]);
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
	String file_path = Resource::GetFullDataPath(file);

	tinyxml2::XMLDocument doc;
	FileDataStream stream(file_path, "rb+");
	tinyxml2::XMLError error = doc.LoadFile(stream.GetHandle());
	if (error != tinyxml2::XML_SUCCESS)
	{
		Debuger::Error("Shader::Serialize -  open XML error (%s)", file_path.c_str());
		return;
	}
	else
	{
		tinyxml2::XMLElement* root_node = doc.RootElement();
		if (root_node->Attribute("Hide"))
		{
			this->HideInspector = String(root_node->Attribute("Hide")).ToBool();
		}
		if (root_node->Attribute("Group"))
		{
			this->GroupInspector = String(root_node->Attribute("Group"));
		}

		//tag
		tinyxml2::XMLElement* node = root_node->FirstChildElement("Tag");
		if (node)
		{
			if (node->Attribute("RenderType"))
			{
				this->RenderType= node->Attribute("RenderType");
			}
			if (node->Attribute("RenderQueue"))
			{
				this->RenderQueue = String(node->Attribute("RenderQueue")).ToInt();
			}
			if (node->Attribute("EnableLight"))
			{
				this->EnableLight = String(node->Attribute("EnableLight")).ToBool();
			}
			if (node->Attribute("CastShadow"))
			{
				this->CastShadow = String(node->Attribute("CastShadow")).ToBool();
			}
			if (node->Attribute("ReceiveShadow"))
			{
				this->ReceiveShadow = String(node->Attribute("ReceiveShadow")).ToBool();
			}
		}

		//Properties
		node = root_node->FirstChildElement("Properties");
		if (node)
		{
			tinyxml2::XMLElement* variable_node = node->FirstChildElement("Propertie");
			while (variable_node)
			{
				ShaderVariable var_info;
				if (variable_node->Attribute("Name"))
				{
					var_info.Name = variable_node->Attribute("Name");
					AssertEx(!var_info.Name.IsEmpty(), "the name is empty");
				}
				if (variable_node->Attribute("Alias"))
				{
					var_info.Alias = variable_node->Attribute("Alias");
				}
				else
				{
					var_info.Alias = var_info.Name;
				}
				if (variable_node->Attribute("Type"))
				{
					String type = variable_node->Attribute("Type");
					if (type.Equals("Int", true))var_info.Type = ShaderVariableType::Int;
					else if (type.Equals("Float", true))var_info.Type = ShaderVariableType::Float;
					else if (type.Equals("Bool", true))var_info.Type = ShaderVariableType::Bool;
					else if (type.Equals("Slider", true))var_info.Type = ShaderVariableType::Slider;
					else if (type.Equals("Color", true))var_info.Type = ShaderVariableType::Color;
					else if (type.Equals("Vector", true))var_info.Type = ShaderVariableType::Vector;
					else if (type.Equals("Rect", true))var_info.Type = ShaderVariableType::Rect;
					else if (type.Equals("2D", true))var_info.Type = ShaderVariableType::D2;
					else if (type.Equals("3D", true))var_info.Type = ShaderVariableType::D3;
					else if (type.Equals("Cube", true))var_info.Type = ShaderVariableType::Cube;
					else AssertEx(false, "parse error:%s", type.c_str());
				}
				if (variable_node->Attribute("Value"))
				{
					var_info.Value = variable_node->Attribute("Value");
					if (var_info.Type == ShaderVariableType::D2 || var_info.Type == ShaderVariableType::D3 || var_info.Type == ShaderVariableType::Cube)
					{
						AssertEx(var_info.Value == "" || var_info.Value == "white" || var_info.Value == "black", "error color%s", var_info.Value.c_str());
					}
				}
				if (variable_node->Attribute("Range"))
				{
					var_info.Range = Vector2(variable_node->Attribute("Range"));
				}
				if (variable_node->Attribute("HideInspector"))
				{
					var_info.HideInspector = String(variable_node->Attribute("HideInspector")).ToBool();
				}
				if (!var_info.Name.IsEmpty())ShaderVariables.Add(var_info);
				variable_node = variable_node->NextSiblingElement("Propertie");
			}
		}

		//Defines
		node = root_node->FirstChildElement("Defines");
		if (node)
		{
			tinyxml2::XMLElement* defines_node = node->FirstChildElement("Define");
			while (defines_node)
			{
				if (defines_node->Attribute("Name"))
				{
					String name = defines_node->Attribute("Name");
					if (!name.IsEmpty())
					{
						VecString defines = name.Split(",");
						for (const auto& define : defines)
						{
							ShaderDefines.Add(define);
						}
					}
				}
				defines_node = defines_node->NextSiblingElement("Define");
			}
		}

		//TextureUnits
		node = root_node->FirstChildElement("TextureUnits");
		if (node)
		{
			tinyxml2::XMLElement* child_node = node->FirstChildElement("TextureUnit");
			while (child_node)
			{
				ShaderTexture texture_info;
				if (child_node->Attribute("Name") != 0)
				{
					texture_info.Name = child_node->Attribute("Name");
				}
				if (child_node->Attribute("File") != 0)
				{
					texture_info.File = child_node->Attribute("File");
				}
				if (child_node->Attribute("Type") != 0)
				{
					String tex_type = child_node->Attribute("Type");
					if (tex_type.Equals("2D", true))texture_info.Type = TextureType::D2;
					else if (tex_type.Equals("3D", true))texture_info.Type = TextureType::D3;
					else if (tex_type.Equals("Cube", true))texture_info.Type = TextureType::Cube;
					else if (tex_type.Equals("Volume", true))texture_info.Type = TextureType::D2Array;
					else AssertEx(0, "%s", tex_type.c_str());
				}
				if (child_node->Attribute("Scale") != 0)
				{
					texture_info.Scale = Vector2(child_node->Attribute("Scale"));
				}
				if (child_node->Attribute("Offset") != 0)
				{
					texture_info.Offset = Vector2(child_node->Attribute("Offset"));
				}
				child_node = child_node->NextSiblingElement("TextureUnit");
				ShaderTextures.Add(texture_info);
			}
		}

		//Pass
		mPassSerializes.Clear();
		node = root_node->FirstChildElement("Pass");
		while (node)
		{
			mPassSerializes.Add(PassSerialize());
			mPassSerializes.Last().Serialize(file, node);
			node = node->NextSiblingElement("Pass");
		}
	}
}
/********************************************************************/
ShaderSerializePools::ShaderSerializes ShaderSerializePools::_shaderSerializes;
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