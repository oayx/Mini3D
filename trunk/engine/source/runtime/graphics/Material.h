
/*****************************************************************************
* Author： hannibal
* Date：2009/11/30
* Description： 材质
*****************************************************************************/
#pragma once

#include "Shader.h"
#include "RenderDefine.h"
#include "runtime/resources/Resources.h"
#include "runtime/scene/SceneUtils.h"

DC_BEGIN_NAMESPACE
class Light;
class Texture;
class TextureUnit;
/********************************************************************/
class ENGINE_DLL Material Final : public Resource
{
	friend class Shader;
	friend class CGProgram;
	typedef List<Light*> Lights;
	FRIEND_CONSTRUCT_DESTRUCT(Material);
	DISALLOW_COPY_ASSIGN(Material);
	DECLARE_OBJECT_CLONE;
	DECLARE_OBJECT_SERIALIZE(Material);
	BEGIN_DERIVED_REFECTION_TYPE(Material, Resource)
	END_DERIVED_REFECTION_TYPE;

	Material();

public:
	~Material();
	static Material* Create(const String& file);
	static Material* GetDefault();
	static void		 ReleaseDefault();

public:
	Shader*			GetShader()const { return _shader; }
	Shader*			SetShaderFile(const String& file);
	const String&	GetShaderFile()const { return _shaderFile; }
	CGProgram*		GetProgram(int pass = 0)const;	

	void			SetShaderVariable(const VecShaderVariable& vars) { _shaderVariables = vars; }
	int				GetVariableCount()const { return _shaderVariables.Size(); }
	ShaderVariable& GetVariable(int index) { return _shaderVariables[index]; }

	void			SetShaderTexture(const VecShaderTexture& texs) { _shaderTextures = texs; }
	int				GetTextureCount()const { return _shaderTextures.Size(); }
	ShaderTexture&  GetTexture(int index) { return _shaderTextures[index]; }

	Pass*			GetPass(int pass = 0)const { return _shader->GetPass(pass); }
	uint			GetPassCount()const { return _shader->GetPassCount(); }

	const Lights&	GetLightes()const { return _lightes; }
	void			SetLightes(const Lights& lightes);

	virtual void	Serialize()override;
	virtual void	Deserialize()override;

public:
	// pass 如果设置小于0，则针对所有pass
	bool SetCullMode(CullMode cull_mode, int pass = 0);
	bool SetColorMask(const Vector4& color_maskint, int pass = 0);
	bool SetDepth(bool enable, bool write_enable = false, StencilCmp fun = StencilCmp::LessEqual, int pass = 0);

	bool SetStencilEnable(bool enable, int pass = 0);
	bool SetStencilCmpFun(StencilCmp fun, int pass = 0);
	bool SetStencilRef(byte ref, int pass = 0);
	bool SetStencilReadMask(byte mask, int pass = 0);
	bool SetStencilWriteMask(byte mask = 0xff, int pass = 0);
	bool SetStencilOp(StencilOp fail_op, StencilOp depth_fail_op = StencilOp::Keep, StencilOp pass_op = StencilOp::Keep, int pass = 0);

	bool SetTextureFile(const String& file, TextureType type = TextureType::D2, TextureFlag flags = TextureFlag::sRGB, int pass = 0, byte layer = 0);
	bool SetTexture(Texture* value, int pass = 0, byte layer = 0);
	bool SetTexture(const String& shader_name, Texture* value, int pass = 0, byte layer = 0);
	bool SetTextureFilter(TextureFilter type, uint anisotropy = 1, int pass = 0, byte layer = 0);
	bool SetTextureAddress(TextureAddress mode, const Vector2& scale = Vector2::one, int pass = 0, byte layer = 0);
	Texture* GetTexture(int pass = 0, byte layer = 0)const;
	TextureUnit* GetTextureUnit(int pass = 0, byte layer = 0)const;

	bool SetAlphaBlend(bool enable, AlphaBlend arg1 = AlphaBlend::SrcAlpha, AlphaBlend arg2 = AlphaBlend::OneMinusSrcAlpha, int pass = 0, byte layer = 0);
	bool SetAlphaTest(bool enable, float ref, StencilCmp fun = StencilCmp::Greater, int pass = 0, byte layer = 0);

public://shader参数
	void SetInt(const String& name, int value){ _shaderIntTable.AddOrUpdate(name, value); }
	void SetFloat(const String& name, float value){ _shaderFloatTable.AddOrUpdate(name, value); }
	void SetColor(const String& name, const Color& value){ _shaderColorTable.AddOrUpdate(name, value); }
	void SetVector(const String& name, const Vector4& value){ _shaderVector4Table.AddOrUpdate(name, value); }
	void SetMatrix(const String& name, const Matrix4& value) {_shaderMatrix4Table.AddOrUpdate(name, value); }
	void SetFloatArray(const String& name, const Vector<float>& values){ _shaderFloatsTable.AddOrUpdate(name, values); }
	void SetVectorArray(const String& name, const Vector<Vector4>& values){ _shaderVector4sTable.AddOrUpdate(name, values); }
	void SetMatrixArray(const String& name, const Vector<Matrix4>& values){ _shaderMatrix4sTable.AddOrUpdate(name, values); }

public:
	void SetCastShadow(bool b)	 { _castShadow = b; }
	void SetReceiveShadow(bool b){ _receiveShadow = b; }
	bool IsCastShadow()const	 { return _castShadow; }
	bool IsReceiveShadow()const	 { return _receiveShadow; }

	int  GetRenderQueue()const;
	void SetRenderQueue(int quque);

	void SetScissorEnable(bool b);
	bool IsScissorEnable()const;

	void  SetDepthOffset(float factor, float units);
	bool  IsDepthOffsetEnable()const;
	float GetDepthOffsetFactor()const;
	float GetDepthOffsetUnits()const;

private:
	virtual bool LoadFromFile(const String& file)override;
	bool ParseFromFile(const String& file);

private:
	String	_shaderFile = "";
	Shader* _shader = nullptr;
	Lights	_lightes;					//受影响的灯光

	bool	_castShadow = true;		//是否产生阴影
	bool	_receiveShadow = false;	//是否接受阴影

	VecShaderVariable _shaderVariables;//变量列表
	VecShaderTexture _shaderTextures;	//纹理

	static	Material* _defaultMaterial;

private://shader参数
	Map<String, int>	_shaderIntTable;
	Map<String, float>	_shaderFloatTable;
	Map<String, Color>	_shaderColorTable;
	Map<String, Vector4> _shaderVector4Table;
	Map<String, Matrix4> _shaderMatrix4Table;
	Map<String, Vector<float>>	_shaderFloatsTable;
	Map<String, Vector<Vector4>> _shaderVector4sTable;
	Map<String, Vector<Matrix4>> _shaderMatrix4sTable;
};//Material

/********************************************************************/
class MaterialManager Final : public object
{
	friend class Material;
	friend class Application;
	typedef Map<String, Material*> Materiales;
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(MaterialManager);
	BEGIN_REFECTION_TYPE(MaterialManager)
	END_FINAL_REFECTION_TYPE;

private:
	~MaterialManager() {}
	static void Destroy();

	static bool AddMaterial(const String& name, Material* material);

public:
	static Material* GetMaterial(const String& name);
	static void DestroyMaterial(const String& name);
	static void DestroyMaterial();

private:
	static Materiales _materiales;
};
DC_END_NAMESPACE
