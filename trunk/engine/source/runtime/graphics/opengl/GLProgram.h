 
/*****************************************************************************
* Author： hannibal
* Date：2020/3/18
* Description：
*****************************************************************************/
#pragma once

#include "runtime/graphics/null/CGProgram.h"
#include "GLDefine.h"

DC_BEGIN_NAMESPACE
class Texture;
class TextureUnit;
/********************************************************************/
class GLProgram Final : public CGProgram
{
	friend class GLDevice;
	typedef Map<String, uint> UniformIndexMaps;
	DEFAULT_CREATE(GLProgram);
	FRIEND_CONSTRUCT_DESTRUCT(GLProgram);
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(GLProgram);
	BEGIN_DERIVED_REFECTION_TYPE(GLProgram, CGProgram)
	END_DERIVED_REFECTION_TYPE;

public:
	~GLProgram();
	GLuint GetShaderProgram()const { return _shaderProgram; }

	virtual bool LoadFromFile(const ShaderDesc& info)override;
	virtual bool LoadFromMemory(const String& name, const VecString& codes, const VecString& defines)override;

	virtual void PreRender()override;
	virtual void PostRender()override;

public:
	virtual bool SetVariable(const String& name, const Matrix4& mat)override;
	virtual bool SetVariable(const String& name, const Matrix4* mats, int count)override;
	virtual bool SetVariable(const String& name, const float* floats, int count)override;
	virtual bool SetVariable(const String& name, const Vector4* vecs, int count)override;
	virtual bool SetVariable(const String& name, const int* ints, int count)override;
	virtual bool SetVariable(const String& name, const ShaderLight& light, int index)override;
	virtual bool SetVariable(const String& name, const bool bools)override;
	virtual bool SetVariable(byte index, TextureUnit* tex_unit)override;
	virtual bool SetVariable(byte index, const String& name, Texture* texture)override;
	virtual bool SetPassVariable(byte index, const String& name, Texture* texture)override;

private:
	uint GetUniformIndex(const String& name)const;

private:
	GLuint _shaderProgram = 0;
	UniformIndexMaps _uniformIndexMaps;
};
GLuint GL_CompileShader(const String& name, const VecString& codes);
DC_END_NAMESPACE
