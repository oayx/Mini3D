 
/*****************************************************************************
* Author： hannibal
* Date：2020/3/18
* Description：
*****************************************************************************/
#pragma once

#include "runtime/graphics/RenderDefine.h"
#include "runtime/resources/Resources.h"

DC_BEGIN_NAMESPACE
class Pass;
class Camera;
class Renderer;
class Texture;
class TextureUnit;

//传递给shader的灯光信息
#pragma pack(push,4)
struct ShaderLight
{
	Vector3 Dir = Vector3::zero;
	int     Type = 0;
	Vector3 Color = Vector3::zero;
	Vector3 Attenuation = Vector3::zero;
	float   Range = 0.0f;
	Vector3 Position = Vector3::zero;
	float	SpotAngle = 0.0f;
};
#pragma pack(pop)
/********************************************************************/
// Gpu程序参数
class ENGINE_DLL CGProgram : public Resource
{
	typedef Vector<VertexAttrib> VertexAttribs;
	DISALLOW_COPY_ASSIGN(CGProgram);
	BEGIN_DERIVED_REFECTION_TYPE(CGProgram, Resource)
	END_DERIVED_REFECTION_TYPE;

protected:
	CGProgram() : base(ResourceType::Shader) {}

public:
	static CGProgram* Create(const ShaderDesc& info);
	static CGProgram* CreateFromMemory(const String& name, const VecString& codes, const VecString& defines);

public:
	virtual bool LoadFromFile(const ShaderDesc& info);
	virtual bool LoadFromMemory(const String& name, const VecString& codes, const VecString& defines) { return false; }
	virtual void PreRender() = 0;
	virtual void Render(Camera* camera, Pass* pass, const Matrix4& obj_mat);
	virtual void PostRender() = 0;

	VertexSemantic GetInputVertexSemantic()const { return _reflectVertexSemantic; }
	bool		   HasInputVertexSemantic(VertexSemantic sem)const { return _reflectVertexSemantic & sem; }

public:
	virtual bool SetVariable(const String& name, const Matrix4& mat) = 0;
	virtual bool SetVariable(const String& name, const Matrix4* mats, int count) = 0;
	virtual bool SetVariable(const String& name, float value) { return SetVariable(name, &value, 1); }
	virtual bool SetVariable(const String& name, const float* floats, int count) = 0;
	virtual bool SetVariable(const String& name, const Vector4* vecs, int count) = 0;
	virtual bool SetVariable(const String& name, int value) { return SetVariable(name, &value, 1); }
	virtual bool SetVariable(const String& name, const int* ints, int count) = 0;
	virtual bool SetVariable(const String& name, const bool bools) = 0;
	virtual bool SetVariable(const String& name, const ShaderLight& light, int index) = 0;
	virtual bool SetVariable(byte index, TextureUnit* tex_unit) = 0;
	virtual bool SetVariable(byte index, const String& name, Texture* texture) = 0;
	virtual bool SetPassVariable(byte index, const String& name, Texture* texture) = 0;	//只传递纹理到GPU，不修改渲染状态

protected:
	const sShaderReflectVariable* GetCBVariable(const String& name)const;
	const String& GetVariableFullName(const String& name)const;//根据名称获得变量全名，包括结构体名称，GLSL使用
	bool GetShaderTextureInfo(const String& name, sShaderReflectTexture& info)const;//根据名称获得shader纹理信息

private:
	void SetInternalTable(Camera* camera, Pass* pass, const Matrix4& mat);
	void SetExtendTable(Pass* pass);
	void SetMaterialTable(Pass* pass);

protected:
	VertexSemantic			_reflectVertexSemantic = VertexSemantic::None;//shader顶点输入语义
	ShaderReflectCBuffers	_reflectCBuffers;	//cbuffer
	ShaderReflectVariables	_reflectVariables;	//变量信息
	ShaderReflectTextures	_reflectTextures;	//纹理信息
	VertexAttribs			_vertexAttribs;	//顶点输入
};
DC_END_NAMESPACE
