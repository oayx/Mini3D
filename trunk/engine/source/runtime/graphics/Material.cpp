#include "Material.h"
#include "Pass.h"
#include "Shader.h"
#include "TextureUnit.h"
#include "RenderDefine.h"
#include "null/Texture.h"
#include "external/tinyxml2/tinyxml2.h"
#include "external/shaderlab/ShaderParser.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(Material, Resource);
Material::Material()
	: base(ResourceType::Material)
{
}
Material::~Material()
{
	SAFE_RELEASE(_shader);
}
Material* Material::Create(const String& file)
{
	Material* material = nullptr;
	//使用共享材质，会导致纹理共享，暂时屏蔽
	//material = MaterialManager::GetMaterial(file);
	if (!material)
	{
		material = Memory::New<Material>();
		material->AutoRelease();
		material->Load(file);
		//MaterialManager::AddMaterial(file, material);
	}
	return material;
}
Material* Material::GetDefault()
{
	if (_defaultMaterial == nullptr)
	{
		_defaultMaterial = Create("internal/material/Default.material");
		_defaultMaterial->Retain();
	}
	return _defaultMaterial;
}
void Material::ReleaseDefault()
{
	SAFE_RELEASE(_defaultMaterial);
}
Object* Material::Clone(Object* newObj)
{
	base::Clone(newObj);
	Material* obj = dynamic_cast<Material*>(newObj);
	if (!obj)return newObj;

	obj->Load(_resFile);
	obj->SetCastShadow(_castShadow);
	obj->SetReceiveShadow(_receiveShadow);

	return obj;
}
bool Material::LoadFromFile(const String& file)
{
	return ParseFromFile(file);
}
Shader* Material::SetShaderFile(const String& file)
{
	this->_shaderFile = file;

	_shaderTextures.Clear();
	_shaderVariables.Clear();

	SAFE_RELEASE(_shader);
	_shader = Shader::Create(this);
	if (_shader)
	{
		_shader->Retain();
		_shader->Load(_shaderFile);
		_shaderVariables = _shader->GetVariables();
		_shaderTextures = _shader->GetTextures();
	}
	return _shader;
}
CGProgram *Material::GetProgram(int pass)const
{
	CHECK_RETURN_PTR_NULL(_shader);
	Pass* p = _shader->GetPass(pass);
	if (!p)return nullptr;
	return p->GetProgram();
}
void Material::SetLightes(const Lights& lightes)
{
	_lightes.Clear();
	if (!lightes.IsEmpty())
	{
		_lightes.AddRange(lightes.begin(), lightes.end());
	}
}
void Material::Serialize()
{
	String fullPath = Resource::GetFullDataPath(_resFile);
	SerializeRead transfer(fullPath);
	this->Transfer(transfer);
}
void Material::Deserialize()
{
	String fullPath = Resource::GetFullDataPath(_resFile);
	SerializeWrite transfer(fullPath);
	this->Transfer(transfer);
}
bool Material::SetCullMode(CullMode cull_mode, int pass)
{
	CHECK_RETURN_PTR_FALSE(_shader);
	if (pass < 0)
	{
		for (int i = 0; i < _shader->GetPassCount(); ++i)
		{
			Pass* p = _shader->GetPass(i);
			p->Cullmode = cull_mode;
		}
	}
	else
	{
		Pass* p = _shader->GetPass(pass);
		CHECK_RETURN_PTR_FALSE(p);
		p->Cullmode = cull_mode;
	}
	return true;
}
bool Material::SetColorMask(const Vector4& color_mask, int pass)
{
	CHECK_RETURN_PTR_FALSE(_shader);
	if (pass < 0)
	{
		for (int i = 0; i < _shader->GetPassCount(); ++i)
		{
			Pass* p = _shader->GetPass(i);
			p->ColorMask = color_mask;
		}
	}
	else
	{
		Pass* p = _shader->GetPass(pass);
		CHECK_RETURN_PTR_FALSE(p);
		p->ColorMask = color_mask;
	}
	return true;
}
bool Material::SetDepth(bool enable, bool write_enable, StencilCmp fun, int pass)
{
	CHECK_RETURN_PTR_FALSE(_shader);
	if (pass < 0)
	{
		for (int i = 0; i < _shader->GetPassCount(); ++i)
		{
			Pass* p = _shader->GetPass(i);
			p->DepthEnable = enable;
			p->DepthWriteEnable = write_enable;
			p->DepthCmpFun = fun;
		}
	}
	else
	{
		Pass* p = _shader->GetPass(pass);
		CHECK_RETURN_PTR_FALSE(p);
		p->DepthEnable = enable;
		p->DepthWriteEnable = write_enable;
		p->DepthCmpFun = fun;
	}
	return true;
}
bool Material::SetStencilEnable(bool enable, int pass)
{
	CHECK_RETURN_PTR_FALSE(_shader);
	if (pass < 0)
	{
		for (int i = 0; i < _shader->GetPassCount(); ++i)
		{
			Pass* p = _shader->GetPass(i);
			p->StencilEnable = enable;
		}
	}
	else
	{
		Pass* p = _shader->GetPass(pass);
		CHECK_RETURN_PTR_FALSE(p);
		p->StencilEnable = enable;
	}
	return true;
}
bool Material::SetStencilCmpFun(StencilCmp fun, int pass)
{
	CHECK_RETURN_PTR_FALSE(_shader);
	if (pass < 0)
	{
		for (int i = 0; i < _shader->GetPassCount(); ++i)
		{
			Pass* p = _shader->GetPass(i);
			p->StencilFunc = fun;
		}
	}
	else
	{
		Pass* p = _shader->GetPass(pass);
		CHECK_RETURN_PTR_FALSE(p);
		p->StencilFunc = fun;
	}
	return true;
}
bool Material::SetStencilRef(byte ref, int pass)
{
	CHECK_RETURN_PTR_FALSE(_shader);
	if (pass < 0)
	{
		for (int i = 0; i < _shader->GetPassCount(); ++i)
		{
			Pass* p = _shader->GetPass(i);
			p->StencilRef = ref;
		}
	}
	else
	{
		Pass* p = _shader->GetPass(pass);
		CHECK_RETURN_PTR_FALSE(p);
		p->StencilRef = ref;
	}
	return true;
}
bool Material::SetStencilReadMask(byte mask, int pass)
{
	CHECK_RETURN_PTR_FALSE(_shader);
	if (pass < 0)
	{
		for (int i = 0; i < _shader->GetPassCount(); ++i)
		{
			Pass* p = _shader->GetPass(i);
			p->StencilReadMask = mask;
		}
	}
	else
	{
		Pass* p = _shader->GetPass(pass);
		CHECK_RETURN_PTR_FALSE(p);
		p->StencilReadMask = mask;
	}
	return true;
}
bool Material::SetStencilWriteMask(byte mask, int pass)
{
	CHECK_RETURN_PTR_FALSE(_shader);
	if (pass < 0)
	{
		for (int i = 0; i < _shader->GetPassCount(); ++i)
		{
			Pass* p = _shader->GetPass(i);
			p->StencilWriteMask = mask;
		}
	}
	else
	{
		Pass* p = _shader->GetPass(pass);
		CHECK_RETURN_PTR_FALSE(p);
		p->StencilWriteMask = mask;
	}
	return true;
}
bool Material::SetStencilOp(StencilOp fail_op, StencilOp depth_fail_op, StencilOp pass_op, int pass)
{
	CHECK_RETURN_PTR_FALSE(_shader);
	if (pass < 0)
	{
		for (int i = 0; i < _shader->GetPassCount(); ++i)
		{
			Pass* p = _shader->GetPass(i);
			p->StencilFailOp = fail_op;
			p->StencilDepthFailOp = depth_fail_op;
			p->StencilPassOp = pass_op;
		}
	}
	else
	{
		Pass* p = _shader->GetPass(pass);
		CHECK_RETURN_PTR_FALSE(p);
		p->StencilFailOp = fail_op;
		p->StencilDepthFailOp = depth_fail_op;
		p->StencilPassOp = pass_op;
	}
	return true;
}
Texture* Material::GetTexture(int pass, byte layer)const
{
	TextureUnit* unit = GetTextureUnit(pass, layer);
	if (unit == nullptr)return nullptr;
	return unit->GetTexture();
}
TextureUnit* Material::GetTextureUnit(int pass, byte layer)const
{
	CHECK_RETURN_PTR_NULL(_shader);
	Pass* p = _shader->GetPass(pass);
	if (p == nullptr)return nullptr;
	return p->GetTexUnit(layer);
}
bool Material::SetTextureFile(const String& file, TextureType type, TextureFlag flags, int pass, byte layer)
{
	CHECK_RETURN_PTR_FALSE(_shader);
	if (pass < 0)
	{
		for (int i = 0; i < _shader->GetPassCount(); ++i)
		{
			Pass* p = _shader->GetPass(i);
			p->SetTextureFile(layer, file, type, flags);
		}
	}
	else
	{
		Pass* p = _shader->GetPass(pass);
		CHECK_RETURN_PTR_FALSE(p);
		p->SetTextureFile(layer, file, type, flags);
	}
	return true;
}
bool Material::SetTexture(Texture* value, int pass, byte layer)
{
	return SetTexture("_MainTex", value, pass, layer);
}
bool Material::SetTexture(const String& shaderName, Texture* value, int pass, byte layer)
{
	CHECK_RETURN_PTR_FALSE(_shader);
	if (pass < 0)
	{
		for (int i = 0; i < _shader->GetPassCount(); ++i)
		{
			Pass* p = _shader->GetPass(i);
			p->SetTexture(layer, shaderName, value);
		}
	}
	else
	{
		Pass* p = _shader->GetPass(pass);
		CHECK_RETURN_PTR_FALSE(p);
		p->SetTexture(layer, shaderName, value);
	}
	return true;
}
bool Material::SetTextureFilter(TextureFilter type, uint anisotropy, int pass, byte layer)
{
	CHECK_RETURN_PTR_FALSE(_shader);
	if (pass < 0)
	{
		for (int i = 0; i < _shader->GetPassCount(); ++i)
		{
			Pass* p = _shader->GetPass(i);
			p->SetTextureFilter(layer, type, anisotropy);
		}
	}
	else
	{
		Pass* p = _shader->GetPass(pass);
		CHECK_RETURN_PTR_FALSE(p);
		p->SetTextureFilter(layer, type, anisotropy);
	}
	return true;
}
bool Material::SetTextureAddress(TextureAddress mode, const Vector2& scale, int pass, byte layer)
{
	CHECK_RETURN_PTR_FALSE(_shader);
	if (pass < 0)
	{
		for (int i = 0; i < _shader->GetPassCount(); ++i)
		{
			Pass* p = _shader->GetPass(i);
			p->SetTextureAddress(layer, mode, scale);
		}
	}
	else
	{
		Pass* p = _shader->GetPass(pass);
		CHECK_RETURN_PTR_FALSE(p);
		p->SetTextureAddress(layer, mode, scale);
	}
	return true;
}
bool Material::SetAlphaBlend(bool enable, AlphaBlend arg1, AlphaBlend arg2, int pass, byte layer)
{
	CHECK_RETURN_PTR_FALSE(_shader);
	if (pass < 0)
	{
		for (int i = 0; i < _shader->GetPassCount(); ++i)
		{
			Pass* p = _shader->GetPass(i);
			p->SetAlphaBlend(layer, enable, arg1, arg2);
		}
	}
	else
	{
		Pass* p = _shader->GetPass(pass);
		CHECK_RETURN_PTR_FALSE(p);
		p->SetAlphaBlend(layer, enable, arg1, arg2);
	}
	return true;
}
bool Material::SetAlphaTest(bool enable, float ref, StencilCmp fun, int pass, byte layer)
{
	CHECK_RETURN_PTR_FALSE(_shader);
	if (pass < 0)
	{
		for (int i = 0; i < _shader->GetPassCount(); ++i)
		{
			Pass* p = _shader->GetPass(i);
			p->SetAlphaTest(layer, enable, ref, fun);
		}
	}
	else
	{
		Pass* p = _shader->GetPass(pass);
		CHECK_RETURN_PTR_FALSE(p);
		p->SetAlphaTest(layer, enable, ref, fun);
	}
	return true;
}
int  Material::GetRenderQueue()const
{
	CHECK_RETURN_PTR_NUMBER(_shader);
	return _shader->GetRenderQueue();
}
void Material::SetRenderQueue(int quque) 
{
	CHECK_RETURN_PTR_VOID(_shader);
	_shader->SetRenderQueue(quque);
}
void Material::SetScissorEnable(bool b)
{
	CHECK_RETURN_PTR_VOID(_shader);
	_shader->SetScissorEnable(b);
}
bool Material::IsScissorEnable()const 
{
	CHECK_RETURN_PTR_FALSE(_shader);
	return _shader->IsScissorEnable();
}
void  Material::SetDepthOffset(float factor, float units) 
{
	CHECK_RETURN_PTR_VOID(_shader);
	_shader->SetDepthOffset(factor, units);
}
bool  Material::IsDepthOffsetEnable()const 
{
	CHECK_RETURN_PTR_FALSE(_shader);
	return _shader->IsDepthOffsetEnable();
}
float Material::GetDepthOffsetFactor()const
{
	CHECK_RETURN_PTR_NUMBER(_shader);
	return _shader->GetDepthOffsetFactor();
}
float Material::GetDepthOffsetUnits()const 
{
	CHECK_RETURN_PTR_NUMBER(_shader);
	return _shader->GetDepthOffsetUnits();
}
bool Material::ParseFromFile(const String& file)
{
	this->Serialize();
	return true;
}
/********************************************************************/
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(Material);
template<class TransferFunction> inline
void Material::Transfer(TransferFunction& transfer, void* ptr)
{
	//先解析shader，把变量、纹理等抽取出来；后面再解析变量、纹理，再赋值。这样如果材质有设置变量或纹理，就可以实现正常覆盖
	if (transfer.Push("Shader"))
	{
		TRANSFER_SIMPLE(_shaderFile);
		if (transfer.IsRead())
		{
			this->SetShaderFile(_shaderFile);
			if (_shader)
			{//默认用shader自带
				_shaderVariables = _shader->GetVariables();
				_shaderTextures = _shader->GetTextures();
			}
		}
		transfer.Pop();
	}

	if (_shader && transfer.IsRead())
	{
		_castShadow = _shader->IsCastShadow();
		_receiveShadow = _shader->IsReceiveShadow();
	}
	if (transfer.Push("Tag"))
	{
		TRANSFER_SIMPLE(_castShadow);
		TRANSFER_SIMPLE(_receiveShadow);
		transfer.Pop();
	}

	if (transfer.Push("Properties"))
	{
		if (transfer.IsRead())
		{
			VecShaderVariable ShaderVariables;
			TRANSFER_VECTOR(ShaderVariables, "Propertie");
			//如果有，就用材质的，否则用shader默认的
			if (ShaderVariables.Size() > 0)
			{
				for (int i = 0; i < _shaderVariables.Size(); ++i)
				{
					for (int j = 0; j < ShaderVariables.Size(); ++j)
					{
						if (_shaderVariables[i].Name == ShaderVariables[j].Name && _shaderVariables[i].Type == ShaderVariables[j].Type)
						{
							_shaderVariables[i].Value = ShaderVariables[j].Value;
							break;
						}
					}
				}
			}
		}
		else
		{
			TRANSFER_VECTOR(_shaderVariables, "Propertie");
		}
		transfer.Pop();
	}

	if (transfer.Push("TextureUnits"))
	{
		if (transfer.IsRead())
		{
			VecShaderTexture ShaderTextures;
			TRANSFER_VECTOR(ShaderTextures, "TextureUnit");
			//如果有，就用材质的，否则用shader默认的
			if (ShaderTextures.Size() > 0)
			{
				for (int i = 0; i < _shaderTextures.Size(); ++i)
				{
					for (int j = 0; j < ShaderTextures.Size(); ++j)
					{
						if (_shaderTextures[i].Name == ShaderTextures[j].Name)
						{
							_shaderTextures[i].File = ShaderTextures[j].File;
							_shaderTextures[i].Scale = ShaderTextures[j].Scale;
							_shaderTextures[i].Offset = ShaderTextures[j].Offset;
							break;
						}
					}
				}
			}

			//使用材质贴图
			for (int i = 0; i < _shaderTextures.Size(); ++i)
			{
				if (_shader)_shader->SetTexture(_shaderTextures[i]);
			}
		}
		else
		{
			TRANSFER_VECTOR(_shaderTextures, "TextureUnit");
		}
		transfer.Pop();
	}
}
/********************************************************************/
IMPL_REFECTION_TYPE(MaterialManager);
void MaterialManager::Destroy()
{
	DestroyMaterial();
}
bool MaterialManager::AddMaterial(const String& name, Material* material)
{
	CHECK_RETURN_PTR_FALSE(material);
	if (material != nullptr && _materiales.Find(name) == _materiales.end())
	{
		material->Retain();
		AssertEx(_materiales.Add(name, material), "");
		return true;
	}
	Debuger::Error("Already have Material:%s", name.c_str());
	return false;
}
Material* MaterialManager::GetMaterial(const String& name)
{
	Materiales::const_iterator it = _materiales.Find(name);
	if (it != _materiales.end())
	{
		return it->second;
	}
	return nullptr;
}
void MaterialManager::DestroyMaterial(const String& name)
{
	Materiales::iterator it = _materiales.Find(name);
	if (it != _materiales.end())
	{
		SAFE_RELEASE(it->second);
		_materiales.Remove(it);
	}
}
void MaterialManager::DestroyMaterial()
{
	for (auto obj : _materiales)
	{
		SAFE_RELEASE(obj.second);
	}
	_materiales.Clear();
}
DC_END_NAMESPACE